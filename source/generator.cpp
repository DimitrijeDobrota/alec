#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "generator.h"

#include "driver.hpp"

namespace alec
{

extern std::vector<record> records;  // NOLINT
extern std::vector<std::string> epilogue;  // NOLINT
extern std::vector<std::string> prologue;  // NOLINT

}  // namespace alec

namespace
{

template<typename T>
std::string join(const std::vector<T>& vec, const std::string& delim)
{
  std::string res;

  if (!vec.empty()) {
    res += vec[0];

    for (size_t idx = 1; idx < vec.size(); idx++) {
      res += delim + vec[idx];
    }
  }

  return res;
}

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
  std::cout << "\n/* Template compile-time variables */\n";

  std::cout << "\n/* Forward-declare templates */\n";
  const auto dupes = generate_dupes();
  for (const auto& dup : dupes) {
    std::cout << std::format(
        "template <auto... val> static const char * const {}_v = \"\";\n", dup);
  }

  std::cout << "\n/* Template specialization */\n\n";
  for (const auto& record : alec::records) {
    if (record.recipe.empty()) {
      // comment
      std::cout << std::format("{}\n", record.name);
      continue;
    }

    std::vector<std::string> params;
    params.reserve(record.args.size());
    for (const auto& arg : record.args) {
      params.emplace_back(arg.substr(arg.find(' ') + 1));
    }

    if (!record.args.empty()) {
      std::cout << std::format("template <{}>\n", join(record.args, ", "));
    }

    if (!record.rules.empty()) {
      std::vector<std::string> constraints;
      for (const auto& param : params) {
        for (const auto& rule : record.rules) {
          constraints.emplace_back(std::format("{}_v<{}>", rule, param));
        }
      }

      std::cout << std::format("\trequires {}\n", join(constraints, " && "));
    }

    std::cout << std::format("static constexpr auto {}_v", record.name);
    if (dupes.contains(record.name)) {
      std::cout << std::format("<{}>", join(params, ", "));
    }

    if (!record.recipe.empty() && record.recipe[0][0] == '"') {
      std::cout << std::format("\n\t = details::escape_literal<{}>;\n\n",
                               record.recipe[0]);
    } else {
      std::cout << std::format("\n\t = details::escape<{}>;\n\n",
                               join(record.recipe, ", "));
    }
  }
}

void generate_functions()
{
  std::cout << "\n/* Run-time functions */\n\n";
  for (const auto& record : alec::records) {
    if (record.recipe.empty()) {
      // comment
      std::cout << std::format("{}\n", record.name);
      continue;
    }

    std::vector<std::string> params;
    params.reserve(record.args.size());
    for (const auto& arg : record.args) {
      params.emplace_back(arg.substr(arg.find(' ') + 1));
    }

    std::cout << std::format("static constexpr auto {}({})\n{{\n",
                             record.name,
                             join(record.args, ", "));

    if (!record.rules.empty()) {
      for (const auto& param : params) {
        std::vector<std::string> constraints;
        constraints.reserve(record.rules.size());
        for (const auto& rule : record.rules) {
          constraints.emplace_back(std::format("{}({})", rule, param));
        }
        std::cout << std::format("\tassert({});\n", join(constraints, " && "));
      }
    }

    if (record.args.empty()) {
      std::cout << std::format("\treturn {}_v;", record.name);
    } else {
      std::cout << std::format("\treturn details::helper::make({});",
                               join(record.recipe, ", "));
    }

    std::cout << "\n}\n\n";
  }
}

}  // namespace

int main(const int argc, char* argv[])
{
  const bool debug = argc > 1 && std::strcmp(argv[1], "--debug") == 0;
  std::ifstream ifile;
  if (argc != 1) {
    ifile.open(argv[!debug ? 1 : 2]);
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
