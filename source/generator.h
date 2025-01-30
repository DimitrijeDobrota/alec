#pragma once

#include <string>
#include <vector>

namespace alec
{

struct record
{
  std::string name;
  std::vector<char*> args;
  std::vector<char*> rules;
  std::vector<char*> recipe;

  bool operator<(const record& rhs) const { return name < rhs.name; }
};

}  // namespace alec
