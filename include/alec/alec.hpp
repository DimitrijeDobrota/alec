#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <string>

#include <based/enum/enum.hpp>
#include <based/string/literal.hpp>
#include <based/types/types.hpp>

namespace alec
{

#define ENUM_COLOR                                                             \
  black, red, green, yellow, blue, magenta, cyan, white, unused_1, def
BASED_DECLARE_ENUM(color, based::bu8, 0, ENUM_COLOR)
BASED_DEFINE_ENUM(color, based::bu8, 0, ENUM_COLOR)
#undef ENUM_COLOR

#define ENUM_DECOR                                                             \
  reset, bold, dim, italic, underline, blink, unused_1, inverse, hide, strike
BASED_DECLARE_ENUM(decor, based::bu8, 0, ENUM_DECOR)
BASED_DEFINE_ENUM(decor, based::bu8, 0, ENUM_DECOR)
#undef ENUM_DECOR

#define ENUM_MOTION end, begin, whole
BASED_DECLARE_ENUM(motion, based::bu8, 0, ENUM_MOTION)
BASED_DEFINE_ENUM(motion, based::bu8, 0, ENUM_MOTION)
#undef ENUM_MOTION

namespace detail
{

template<std::size_t n>
static constexpr std::size_t size(based::string_literal<n> /*val*/)
{
  return n;
}
static constexpr std::size_t size(char /*val*/)
{
  return 1;
}

template<class T>
static constexpr std::size_t size(T val)
{
  std::size_t len = 1;
  while ((val /= 10) != 0) {
    len++;
  }
  return len;
}

template<std::size_t n>
static constexpr char* append(char* ptr, based::string_literal<n> val)
{
  std::copy_n(val.data(), n, ptr);
  return ptr + n;  // NOLINT
}

static constexpr char* append(char* ptr, char val)
{
  *ptr++ = val;  // NOLINT
  return ptr;
}

template<class T>
static constexpr char* append(char* ptr, T val)
{
  char* tmp = ptr += size(val);  // NOLINT
  do {  // NOLINT
    *--tmp = '0' + static_cast<char>(val % 10);  // NOLINT
  } while ((val /= 10) != 0);
  return ptr;
}

static constexpr std::string make(auto... args)
{
  std::string res((size(args) + ... + 2), 0);
  res[0] = 0x1B, res[1] = '[';
  auto* ptr = res.data() + 2;  // NOLINT
  ((ptr = append(ptr, args)), ...);
  return res;
}

template<auto... args>
struct escape_t
{
  static constexpr const auto value = []()
  {
    std::array<char, (size(args) + ... + 3)> arr = {};
    arr[0] = 0x1B, arr[1] = '[';
    auto* ptr = arr.data() + 2;
    ((ptr = append(ptr, args)), ...);
    return arr;
  }();
  static constexpr auto data = value.data();
};

template<auto... args>
static constexpr auto escape = alec::detail::escape_t<args...>::data;

template<based::string_literal... strs>
static constexpr auto escape_literal = escape<strs...>;

}  // namespace detail

// Template compile-time variables

// Forward-declare templates

template<auto... val>
static const char* const background_v = "";

template<auto... val>
static const char* const foreground_v = "";

// Template specializations

// Move cursor up/down/frwd/back
template<unsigned cnt>
static constexpr auto cursor_up_v = detail::escape<cnt, 'A'>;

template<unsigned cnt>
static constexpr auto cursor_down_v = detail::escape<cnt, 'B'>;

template<unsigned cnt>
static constexpr auto cursor_frwd_v = detail::escape<cnt, 'C'>;

template<unsigned cnt>
static constexpr auto cursor_back_v = detail::escape<cnt, 'D'>;

// Move cursor to the next/prev line
template<unsigned cnt>
static constexpr auto cursor_line_next_v = detail::escape<cnt, 'E'>;

template<unsigned cnt>
static constexpr auto cursor_line_prev_v = detail::escape<cnt, 'F'>;

// Set cursor to specific column
template<unsigned col>
static constexpr auto cursor_column_v = detail::escape<col, 'G'>;

// Erase functions
template<motion::enum_type motion>
static constexpr auto erase_display_v = detail::escape<motion(), 'J'>;

template<motion::enum_type motion>
static constexpr auto erase_line_v = detail::escape<motion(), 'K'>;

// Scroll up/down
template<unsigned cnt>
static constexpr auto scroll_up_v = detail::escape<cnt, 'S'>;

template<unsigned cnt>
static constexpr auto scroll_down_v = detail::escape<cnt, 'T'>;

// Set cursor to a specific position
template<unsigned row, unsigned col>
static constexpr auto cursor_position_v = detail::escape<row, ';', col, 'H'>;

// color
// palet colors
template<color::enum_type color>
static constexpr auto foreground_v<color> = detail::escape<color() + 30, 'm'>;

template<color::enum_type color>
static constexpr auto background_v<color> = detail::escape<color() + 40, 'm'>;

// 256-color palette
template<based::bu8 idx>
static constexpr auto foreground_v<idx> =
    detail::escape<38, ';', 5, ';', idx, 'm'>;

template<based::bu8 idx>
static constexpr auto background_v<idx> =
    detail::escape<48, ';', 5, ';', idx, 'm'>;

// RGB colors
template<based::bu8 red, based::bu8 green, based::bu8 blue>
static constexpr auto foreground_v<red, green, blue> =
    detail::escape<38, ';', 2, ';', red, ';', green, ';', blue, 'm'>;

template<based::bu8 red, based::bu8 green, based::bu8 blue>
static constexpr auto background_v<red, green, blue> =
    detail::escape<48, ';', 2, ';', red, ';', green, ';', blue, 'm'>;

// Set/reset text decorators
template<decor::enum_type decor>
static constexpr auto decor_set_v = detail::escape<decor(), 'm'>;

template<decor::enum_type decor>
static constexpr auto decor_reset_v = detail::escape<decor() + 20, 'm'>;

// Save/restore cursor position;
static constexpr auto cursor_save_v = detail::escape<'s'>;

static constexpr auto cursor_restore_v = detail::escape<'u'>;

// Set screen modes
template<unsigned mode>
static constexpr auto screen_mode_set_v = detail::escape<'=', mode, 'h'>;

template<unsigned mode>
static constexpr auto screen_mode_reset_v = detail::escape<'=', mode, 'l'>;

// Private screen modes supported by most terminals
// Save/restore screen
static constexpr auto screen_save_v = detail::escape_literal<"?47h">;

static constexpr auto screen_restore_v = detail::escape_literal<"?47l">;

// Show/hide cursor
static constexpr auto cursor_show_v = detail::escape_literal<"?25h">;

static constexpr auto cursor_hide_v = detail::escape_literal<"?25l">;

// Enable/disable alternate buffer
static constexpr auto abuf_enable_v = detail::escape_literal<"?1049h">;

static constexpr auto abuf_disable_v = detail::escape_literal<"?1049l">;

// Enable/disable bracketed paste mode
static constexpr auto paste_enable_v = detail::escape_literal<"?2004h">;

static constexpr auto paste_disable_v = detail::escape_literal<"?2004l">;

// Run-time functions

// Move cursor up/down/frwd/back
static constexpr auto cursor_up(unsigned cnt)
{
  return detail::make(cnt, 'A');
}

static constexpr auto cursor_down(unsigned cnt)
{
  return detail::make(cnt, 'B');
}

static constexpr auto cursor_frwd(unsigned cnt)
{
  return detail::make(cnt, 'C');
}

static constexpr auto cursor_back(unsigned cnt)
{
  return detail::make(cnt, 'D');
}

// Move cursor to the next/prev line
static constexpr auto cursor_line_next(unsigned cnt)
{
  return detail::make(cnt, 'E');
}

static constexpr auto cursor_line_prev(unsigned cnt)
{
  return detail::make(cnt, 'F');
}

// Set cursor to specific column
static constexpr auto cursor_column(unsigned col)
{
  return detail::make(col, 'G');
}

// Erase functions
static constexpr auto erase_display(motion::enum_type motion)
{
  return detail::make(motion(), 'J');
}

static constexpr auto erase_line(motion::enum_type motion)
{
  return detail::make(motion(), 'K');
}

// Scroll up/down
static constexpr auto scroll_up(unsigned cnt)
{
  return detail::make(cnt, 'S');
}

static constexpr auto scroll_down(unsigned cnt)
{
  return detail::make(cnt, 'T');
}

// Set cursor to a specific position
static constexpr auto cursor_position(unsigned row, unsigned col)
{
  return detail::make(row, ';', col, 'H');
}

// color
// palet colors
static constexpr auto foreground(color::enum_type color)
{
  return detail::make(color() + 30, 'm');
}

static constexpr auto background(color::enum_type color)
{
  return detail::make(color() + 40, 'm');
}

// 256-color palette
static constexpr auto foreground(based::bu8 idx)
{
  return detail::make(38, ';', 5, ';', idx, 'm');
}

static constexpr auto background(based::bu8 idx)
{
  return detail::make(48, ';', 5, ';', idx, 'm');
}

// RGB colors
static constexpr auto foreground(
    based::bu8 red, based::bu8 green, based::bu8 blue
)
{
  return detail::make(38, ';', 2, ';', red, ';', green, ';', blue, 'm');
}

static constexpr auto background(
    based::bu8 red, based::bu8 green, based::bu8 blue
)
{
  return detail::make(48, ';', 2, ';', red, ';', green, ';', blue, 'm');
}

// Set/reset text decorators
static constexpr auto decor_set(decor::enum_type decor)
{
  return detail::make(decor(), 'm');
}

static constexpr auto decor_reset(decor::enum_type decor)
{
  return detail::make(decor() + 20, 'm');
}

// Set screen modes
static constexpr auto screen_mode_set(unsigned mode)
{
  return detail::make('=', mode, 'h');
}

static constexpr auto screen_mode_reset(unsigned mode)
{
  return detail::make('=', mode, 'l');
}

}  // namespace alec
