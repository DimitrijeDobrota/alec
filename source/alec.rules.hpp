#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cinttypes>
#include <optional>
#include <stdexcept>
#include <string>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

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

class runtime_error : public std::runtime_error
{
public:
  explicit runtime_error(const std::string& err)
      : std::runtime_error(err)
  {
  }
};

enum error_code_t  // NOLINT
{
  FDNTERM,
  TERMIOSRD,
  TERMIOSWR,
  BUFFULL,
  CHARRD,
  IOCTL,
  SCREENSZ
};

template<error_code_t e>
class error : public runtime_error
{
public:
  explicit error()
      : runtime_error(error_get_message(e))
  {
  }

private:
  static std::string error_get_message(error_code_t error)
  {
    switch (error) {
      case error_code_t::FDNTERM:
        return "File descriptor is not associated with a terminal";
      case error_code_t::TERMIOSRD:
        return "Can't read termios";
      case error_code_t::TERMIOSWR:
        return "Can't write termios";
      case error_code_t::BUFFULL:
        return "Buffer is full";
      case error_code_t::CHARRD:
        return "Can't read character";
      case error_code_t::IOCTL:
        return "ioctl error";
      case error_code_t::SCREENSZ:
        return "Can't determine the screen size";
    }

    return "alec error, should not happen...";
  }
};

class buffer
{
public:
  explicit buffer(int fdsc)
      : m_fd(fdsc)
      , m_orig_termios()
  {
    if (isatty(m_fd) == 0) {
      throw error<error_code_t::FDNTERM>();
    }

    if (tcgetattr(m_fd, &m_orig_termios) == -1) {
      throw error<error_code_t::TERMIOSRD>();
    }

    struct termios raw = m_orig_termios;

    // NOLINTBEGIN
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);  // | ISIG
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    // NOLINTEND

    /* put terminal in raw mode after flushing */
    if (tcsetattr(m_fd, TCSAFLUSH, &raw) < 0) {
      throw error<error_code_t::TERMIOSWR>();
    }
  }

  buffer(const buffer&) = delete;
  buffer& operator=(const buffer&) = delete;

  buffer(buffer&&) = default;
  buffer& operator=(buffer&&) = default;

  ~buffer() { tcsetattr(m_fd, TCSAFLUSH, &m_orig_termios); }

  uint8_t read()
  {
    if (m_start == m_end && get() == 0) {
      return 0;
    }

    uint8_t chr = m_buffer[m_start];  // NOLINT
    m_start = (m_start + 1) % m_buffer.size();
    return chr;
  }

  uint8_t read_blocking()
  {
    while (m_start == m_end) {
      get();
    }

    uint8_t chr = m_buffer[m_start];  // NOLINT
    m_start = (m_start + 1) % m_buffer.size();
    return chr;
  }

  void flush()
  {
    while (get() != 0) {
    }
  }

private:
  size_t get()
  {
    ssize_t scnt = -1;
    if ((m_end + 1) % m_buffer.size() == m_start) {
      throw error<error_code_t::BUFFULL>();
    }

    if (m_start <= m_end) {
      scnt = ::read(m_fd, m_buffer.data() + m_end, m_buffer.size() - m_end);
    } else {
      scnt = ::read(m_fd, m_buffer.data() + m_end, m_start - m_end);
    }

    if (scnt == -1) {
      throw error<error_code_t::CHARRD>();
    }

    const auto cnt = static_cast<size_t>(scnt);
    m_end = (m_end + cnt) % m_buffer.size();
    return cnt;
  }

  std::array<uint8_t, 1024> m_buffer = {0};
  int m_fd = 0;

  uint64_t m_start = 0;
  uint64_t m_end = 0;

  struct termios m_orig_termios;
};

inline auto& get_buffer()
{
  static std::optional<buffer> ibuf;
  return ibuf;
}

inline void init_buffer(int fdsc)
{
  get_buffer().emplace(fdsc);
}

inline void dest_buffer()
{
  get_buffer().reset();
}

inline std::pair<std::uint16_t, std::uint16_t> get_screen_size()
{
#ifdef TIOCGSIZE
  struct ttysize tts = {};
  if (ioctl(STDIN_FILENO, TIOCGSIZE, &tts) == -1) {  // NOLINT
    throw error<error_code_t::IOCTL>();
  }
  return {tts.ts_cols, tts.ts_lines};
#elif defined(TIOCGWINSZ)
  struct winsize tts = {};
  if (ioctl(STDIN_FILENO, TIOCGWINSZ, &tts) == -1) {  // NOLINT
    throw error<error_code_t::IOCTL>();
  }
  return {tts.ws_col, tts.ws_row};
#endif /* TIOCGSIZE */

  throw error<error_code_t::SCREENSZ>();
}

class event
{
public:
  enum class Type : std::uint8_t
  {
    NONE = 0,
    KEY = 1,
    RESIZE = 2,
    MOUSE = 3,
  };

  enum class Mod : std::uint8_t
  {
    ALT = 1,
    CTRL = 2,
    SHIFT = 4,
    MOTION = 8,
  };

  event(Type type, uint8_t mod_mask, uint8_t key)
      : m_type(type)
      , m_mod_mask(mod_mask)
      , m_key(key)
  {
  }

  auto type() const { return m_type; }
  auto key() const { return m_key; }
  auto mod_mask() const { return m_mod_mask; }

  bool is_set(uint8_t mask) const { return mask == (m_mod_mask & mask); }

private:
  Type m_type = Type::NONE;
  uint8_t m_mod_mask = 0;
  uint8_t m_key = 0;
};

inline event get_event()
{
  const auto chr = get_buffer().value().read();
  return {chr != 0 ? event::Type::KEY : event::Type::NONE, 0, chr};
}

}  // namespace alec
