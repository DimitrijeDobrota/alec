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

int main(const int argc, char* argv[])
{
  using namespace alec;  // NOLINT

  const bool debug = argc > 1 && std::strcmp(argv[1], "--debug") == 0;
  std::ifstream ifile;
  if (argc != 1) {
    ifile.open(argv[!debug ? 1 : 2]);
  }

  driver drv = argc == 1 ? driver(std::cin, debug) : driver(ifile, debug);
  parser parser(drv, debug);
  const int res = parser();

  // print prologue section
  for (const auto line : prologue) {
    std::cout << line;
  }

  std::set<std::string> seen, dupes;
  for (const auto& record : records) {
    const auto [_, inserted] = seen.insert(record.name);
    if (!inserted) {
      dupes.insert(record.name);
    }
  }

  std::cout << "\n/* Template compile-time variables */\n\n";
  for (const auto& dup : dupes) {
    std::cout << std::format(
        "template <auto... val> static const char *{}_v;\n", dup);
  }

  for (const auto& record : records) {
    if (record.recipe.empty()) {
      // comment
      std::cout << std::format("{}\n\n", record.name);
      continue;
    }

    std::vector<std::string> params(record.args.size());
    std::transform(record.args.begin(),
                   record.args.end(),
                   params.begin(),
                   [](const std::string& str)
                   { return str.substr(str.find(' ') + 1); });

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

  std::cout << "\n/* Run-time functions */\n\n";
  for (const auto& record : records) {
    if (record.recipe.empty()) {
      // comment
      std::cout << std::format("{}\n\n", record.name);
      continue;
    }

    std::vector<std::string> params(record.args.size());
    std::transform(record.args.begin(),
                   record.args.end(),
                   params.begin(),
                   [](const std::string& str)
                   { return str.substr(str.find(' ') + 1); });

    std::cout << std::format("static constexpr auto {}({}) {{\n",
                             record.name,
                             join(record.args, ", "));

    if (!record.rules.empty()) {
      for (const auto& param : params) {
        std::vector<std::string> constraints;
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

  // print epilogue section
  for (const auto line : epilogue) {
    std::cout << line;
  }
}
