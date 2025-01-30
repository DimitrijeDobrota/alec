#pragma once

#include <cstddef>
#include <ostream>
#include <utility>

namespace alec
{

using position_t = std::size_t;
using location_t = std::pair<std::size_t, std::size_t>;

}  // namespace alec

inline std::ostream& operator<<(std::ostream& ost, const alec::location_t& loc)
{
  return ost << "[" << loc.first << "-" << loc.second << "]";
}
