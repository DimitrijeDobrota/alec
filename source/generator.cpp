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

  std::cout << "\n/* Template compile-time variables */\n";

  std::cout << "\n/* Forward-declare templates */\n";
  const auto dupes = generate_dupes();
  for (const auto& dup : dupes) {
    std::cout << "template <auto... val> static const char * const " << dup
              << "_v = \"\";\n";
  }

  std::cout << "\n/* Template specialization */\n\n";
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
      std::cout << Template(record.args);
    }

    if (!record.rules.empty()) {
      std::cout << Requires(
          join(params,
               " && ",
               [&](const auto& param)
               {
                 return join(record.rules,
                             " && ",
                             [&](const auto& rule)
                             { return TemplateD(rule + "_v", param); });
               }));
    }

    std::cout << "static constexpr auto " << record.name + "_v";

    if (dupes.contains(record.name)) {
      std::cout << TemplateD("", params);
    }

    std::cout << " = ";
    if (!record.recipe.empty() && record.recipe[0][0] == '"') {
      std::cout << TemplateD("details::escape_literal", record.recipe[0]);
    } else {
      std::cout << TemplateD("details::escape", record.recipe);
    }
    std::cout << ";\n\n";
  }
}

void generate_functions()
{
  using namespace cemplate;  // NOLINT

  std::cout << "\n/* Run-time functions */\n\n";
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

    std::cout << Function(record.name, "static constexpr auto", record.args);

    if (!record.rules.empty()) {
      for (const auto& param : params) {
        std::cout << Statement(Call(
            "assert",
            join(record.rules,
                 " && ",
                 [&](const std::string& rule) { return Call(rule, param); })));
      }
    }

    if (record.args.empty()) {
      std::cout << Return(record.name + "_v");
    } else {
      std::cout << Return(Call("details::helper::make", record.recipe));
    }

    std::cout << Function(record.name);
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
