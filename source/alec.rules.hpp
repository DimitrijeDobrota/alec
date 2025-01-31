#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cinttypes>
#include <string>

namespace alec
{

enum Ctrl : std::uint8_t
{
  BELL = 0x07,
  BS = 0x08,
  HT = 0x09,
  LF = 0x0A,
  VT = 0x0B,
  FF = 0x0C,
  CR = 0x0D,
  ESC = 0x1B,
  DEL = 0x7F,
};

enum class Color : std::uint8_t
{
  BLACK = 0,
  RED = 1,
  GREEN = 2,
  YELLOW = 3,
  BLUE = 4,
  MAGENTA = 5,
  CYAN = 6,
  WHITE = 7,
  DEFAULT = 9,
};

enum class Decor : std::uint8_t
{
  RESET = 0,
  BOLD = 1,
  DIM = 2,
  ITALIC = 3,
  UNDERLINE = 4,
  BLINK = 5,
  INVERSE = 7,
  HIDE = 8,
  STRIKE = 9,
};

enum class Motion : std::uint8_t
{
  END = 0,
  BEGIN = 1,
  WHOLE = 2,
};

namespace details
{

template<std::size_t n>
struct string_literal
{
  constexpr string_literal(const char (&str)[n])  // NOLINT
      : m_value(std::to_array(str))
  {
  }  // NOLINT

  constexpr std::size_t size() const { return n; }
  constexpr const char* data() const { return m_value.data(); }

  std::array<char, n> m_value;
};

namespace helper
{
template<std::size_t n>
static constexpr std::size_t size(string_literal<n> /*val*/)
{
  return n;
}
static constexpr std::size_t size(char /*val*/)
{
  return 1;
}
static constexpr std::size_t size(int val)
{
  std::size_t len = 1;
  while ((val /= 10) != 0) {
    len++;
  }
  return len;
}

template<std::size_t n>
static constexpr char* append(char* ptr, string_literal<n> val)
{
  std::copy_n(val.data(), n, ptr);
  return ptr + n;  // NOLINT
}

static constexpr char* append(char* ptr, char val)
{
  *ptr++ = val;  // NOLINT
  return ptr;
}

static constexpr char* append(char* ptr, int val)
{
  char* tmp = ptr += size(val);  // NOLINT
  do {  // NOLINT
    *--tmp = '0' + static_cast<char>(val % 10);  // NOLINT
  } while ((val /= 10) != 0);
  return ptr;
}

static constexpr std::string make(auto... args)
{
  std::string res((helper::size(args) + ... + 2), 0);
  res[0] = Ctrl::ESC, res[1] = '[';
  auto* ptr = res.data() + 2;  // NOLINT
  ((ptr = helper::append(ptr, args)), ...);
  return res;
}

template<auto... args>
struct escape_t
{
  static constexpr const auto value = []()
  {
    std::array<char, (helper::size(args) + ... + 3)> arr = {Ctrl::ESC, '[', 0};
    auto* ptr = arr.data() + 2;
    ((ptr = helper::append(ptr, args)), ...);
    return arr;
  }();
  static constexpr auto data = value.data();
};

}  // namespace helper

template<auto... args>
static constexpr auto escape = alec::details::helper::escape_t<args...>::data;

template<details::string_literal... strs>
static constexpr auto escape_literal = escape<strs...>;

}  // namespace details

// Tamplate parameter constraints

template<int n>
concept limit_256_v = n >= 0 && n < 256;

template<int n>
concept limit_pos_v = n >= 0;

static constexpr bool limit_pos(int n)
{
  return n >= 0;
}
static constexpr bool limit_256(int n)
{
  return n >= 0 && n < 256;
}

/*%%*//*
// NOLINTBEGIN (*cast*)

// Move cursor up/down/frwd/back

    cursor_up
    int cnt
    limit_pos
    cnt, 'A'

    cursor_down
    int cnt
    limit_pos
    cnt, 'B'

    cursor_frwd
    int cnt
    limit_pos
    cnt, 'C'

    cursor_back
    int cnt
    limit_pos
    cnt, 'D'

// Move cursor to the next/prev line

    cursor_line_next
    int cnt
    limit_pos
    cnt, 'E'

    cursor_line_prev
    int cnt
    limit_pos
    cnt, 'F'

// Set cursor to specific column

    cursor_column
    int col
    limit_pos
    col, 'G'

// Erase functions

    erase_display
    Motion mtn
    |
    int(mtn), 'J'

    erase_line
    Motion mtn
    |
    int(mtn), 'K'

// Scroll up/down

    scroll_up
    int cnt
    limit_pos
    cnt, 'S'

    scroll_down
    int cnt
    limit_pos
    cnt, 'T'

// Set cursor to a specific position

    cursor_position
    int row, int col
    limit_pos
    row, ';', col, 'H'

// color

// palet colors

    foreground
    Color color
    |
    int(color) + 30, 'm'

    background
    Color color
    |
    int(color) + 40, 'm'

// 256-color palette

    foreground
    int idx
    limit_256
    38, ';', 5, ';', idx, 'm'

    background
    int idx
    limit_256
    48, ';', 5, ';', idx, 'm'

// RGB colors

    foreground
    int red, int green, int blue
    limit_256
    38, ';', 2, ';', red, ';', green, ';', blue, 'm'

    background
    int red, int green, int blue
    limit_256
    48, ';', 2, ';', red, ';', green, ';', blue, 'm'

// Set/reset text decorators

    decor_set
    Decor decor
    |
    int(decor), 'm'

    decor_reset
    Decor decor
    |
    int(decor) + 20, 'm'

// Save/restore cursor position;

    cursor_save
    |
    |
    's'

    cursor_restore
    |
    |
    'u'

// Set screen modes

    screen_mode_set
    int mode
    limit_pos
    '=', mode, 'h'

    screen_mode_reset
    int mode
    limit_pos
    '=', mode, 'l'

// Private screen modes supported by most terminals

// Save/restore screen

    screen_save
    |
    |
    "?47h"

    screen_restore
    |
    |
    "?47l"

// Show/hide cursor

    cursor_show
    |
    |
    "?25h"

    cursor_hide
    |
    |
    "?25l"

// Enable/disable alternate buffer

    abuf_enable
    |
    |
    "?1049h"

    abuf_disable
    |
    |
    "?1049l"

// Enable/disable bracketed paste mode

    paste_enable
    |
    |
    "?2004h"

    paste_disable
    |
    |
    "?2004l"

// NOLINTEND (*cast*)
*//*%%*/

// Keyboard string TODO

}  // namespace alec
