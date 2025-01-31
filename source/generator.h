#pragma once

#include <string>
#include <vector>

namespace alec
{

struct record
{
  std::string name;
  std::vector<std::string> args;
  std::vector<std::string> rules;
  std::vector<std::string> recipe;

  bool operator<(const record& rhs) const { return name < rhs.name; }
};

}  // namespace alec
