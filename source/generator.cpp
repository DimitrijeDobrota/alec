#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <span>
#include <string>
#include <vector>

#include "generator.h"

#include <cemplate/cemplate.hpp>

#include "driver.hpp"

namespace alec
{

extern std::vector<record> records;  // NOLINT
extern std::vector<std::string> epilogue;  // NOLINT
extern std::vector<std::string> prologue;  // NOLINT

}  // namespace alec

namespace
{

auto generate_dupes()
{
  std::set<std::string> dupes;
  std::set<std::string> seen;
  for (const auto& record : alec::records) {
    const auto [_, inserted] = seen.insert(record.name);
    if (!inserted) {
      dupes.insert(record.name);
    }
  }
  return dupes;
}

void generate_variables()
{
  using namespace cemplate;  // NOLINT

  Program prog(std::cout);

  prog.comment("Template compile-time variables\n");
  prog.comment("Forward-declare templates\n");

  const auto dupes = generate_dupes();
  for (const auto& dup : dupes) {
    prog.template_decl({"auto... val"});
    prog.declaration("static const char* const", dup + "_v", string(""));
    prog.line_empty();
  }

  prog.comment("Template specializations\n");
  for (const auto& record : alec::records) {
    if (record.recipe.empty()) {  // comment
      std::cout << record.name << '\n';
      continue;
    }

    std::vector<std::string> params;
    params.reserve(record.args.size());
    for (const auto& arg : record.args) {
      params.emplace_back(arg.substr(arg.find(' ') + 1));
    }

    if (!record.args.empty()) {
      prog.template_decl(record.args);
    }

    if (!record.rules.empty()) {
      prog.require(join(std::begin(params),
                        std::end(params),
                        " && ",
                        [&](const auto& param)
                        {
                          return join(
                              std::begin(record.rules),
                              std::end(record.rules),
                              " && ",
                              [&](const auto& rule)
                              { return template_def(rule + "_v", {param}); });
                        }));
    }

    const auto var = record.name + "_v";

    const auto type =
        dupes.contains(record.name) ? template_def(var, params) : var;

    const auto* temp = !record.recipe.empty() && record.recipe[0][0] == '"'
        ? "details::escape_literal"
        : "details::escape";

    prog.declaration(
        "static constexpr auto", type, template_def(temp, record.recipe));

    prog.line_empty();
  }
}

void generate_functions()
{
  using namespace cemplate;  // NOLINT

  Program prog(std::cout);

  prog.comment("Run-time functions\n");
  for (const auto& record : alec::records) {
    if (record.recipe.empty()) {  // comment
      std::cout << record.name << '\n';
      continue;
    }

    std::vector<std::string> params;
    params.reserve(record.args.size());
    for (const auto& arg : record.args) {
      params.emplace_back(arg.substr(arg.find(' ') + 1));
    }

    prog.function_open(record.name, "static constexpr auto", record.args);

    if (!record.rules.empty()) {
      for (const auto& param : params) {
        prog.call(
            "assert",
            join(std::begin(record.rules),
                 std::end(record.rules),
                 " && ",
                 [&](const std::string& rule) { return call(rule, {param}); }));
      }
    }

    if (record.args.empty()) {
      prog.ret(record.name + "_v");
    } else {
      prog.ret(call("details::helper::make", record.recipe));
    }

    prog.function_close(record.name);
  }
}

}  // namespace

int main(int argc, char* argv[])
{
  const std::span args(argv, static_cast<std::size_t>(argc));

  const bool debug = argc > 1 && std::strcmp(args[1], "--debug") == 0;
  std::ifstream ifile;
  if (argc != 1) {
    ifile.open(args[!debug ? 1 : 2]);
  }

  using namespace alec;  // NOLINT

  driver drv = argc == 1 ? driver(std::cin, debug) : driver(ifile, debug);
  parser parser(drv, debug);
  const int res = parser();

  if (res != 0) {
    std::cerr << "Parser error";
    return -1;
  }

  // print prologue section
  for (const auto& line : prologue) {
    std::cout << line;
  }

  generate_variables();
  generate_functions();

  // print epilogue section
  for (const auto& line : epilogue) {
    std::cout << line;
  }
}
