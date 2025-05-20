#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>

#include <based/enum/enum.hpp>
#include <based/enum/enum_flag.hpp>
#include <based/types/types.hpp>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace alec
{

class runtime_error : public std::runtime_error
{
public:
  explicit runtime_error(const std::string& err)
      : std::runtime_error(err)
  {
  }
};

#define ENUM_ERROR_CODE                                                        \
  fdnterm, termiosrd, termioswr, buffull, charrd, ioctl, screensz
BASED_DECLARE_ENUM(error_code_t, based::bi8, 0, ENUM_ERROR_CODE)
BASED_DEFINE_ENUM(error_code_t, based::bi8, 0, ENUM_ERROR_CODE)
#undef ENUM_ERROR_CODE

template<error_code_t::enum_type e>
class error : public runtime_error
{
public:
  explicit error()
      : runtime_error(error_get_message(e))
  {
  }

private:
  static std::string error_get_message(error_code_t::enum_type error)
  {
    switch (error()) {
      case error_code_t::fdnterm():
        return "File descriptor is not associated with a terminal";
      case error_code_t::termiosrd():
        return "Can't read termios";
      case error_code_t::termioswr():
        return "Can't write termios";
      case error_code_t::buffull():
        return "Buffer is full";
      case error_code_t::charrd():
        return "Can't read character";
      case error_code_t::ioctl():
        return "ioctl error";
      case error_code_t::screensz():
        return "Can't determine the screen size";
      default:
        return "alec error, should not happen...";
    }
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
      throw error<error_code_t::fdnterm>();
    }

    if (tcgetattr(m_fd, &m_orig_termios) == -1) {
      throw error<error_code_t::termiosrd>();
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
      throw error<error_code_t::termioswr>();
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
      throw error<error_code_t::buffull>();
    }

    if (m_start <= m_end) {
      scnt = ::read(m_fd, m_buffer.data() + m_end, m_buffer.size() - m_end);
    } else {
      scnt = ::read(m_fd, m_buffer.data() + m_end, m_start - m_end);
    }

    if (scnt == -1) {
      throw error<error_code_t::charrd>();
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
    throw error<error_code_t::ioctl>();
  }
  return {tts.ws_col, tts.ws_row};
#endif /* TIOCGSIZE */

  throw error<error_code_t::screensz>();
}

#define ENUM_EVENT_TYPE none, key, resize, mouse
#define ENUM_EVENT_MOD none, alt, ctrl, shift, motion

class event
{
public:
  BASED_DECLARE_ENUM(type, based::bu8, 0, ENUM_EVENT_TYPE)
  BASED_DECLARE_ENUM_FLAG(mod, based::bu8, ENUM_EVENT_MOD)

  event(type::enum_type type, uint8_t mod_mask, uint8_t key)  // NOLINT
      : m_type(type)
      , m_mod_mask(mod_mask)
      , m_key(key)
  {
  }

  [[nodiscard]] const auto& type() const { return m_type; }
  [[nodiscard]] auto& type() { return m_type; }

  [[nodiscard]] const auto& key() const { return m_key; }
  [[nodiscard]] auto& key() { return m_key; }

  [[nodiscard]] const auto& mod_mask() const { return m_mod_mask; }
  [[nodiscard]] auto& mod_mask() { return m_mod_mask; }

  [[nodiscard]] bool is_set(uint8_t mask) const
  {
    return mask == (m_mod_mask & mask);
  }

private:
  type::enum_type m_type = type::none;
  uint8_t m_mod_mask = 0;
  uint8_t m_key = 0;
};

BASED_DEFINE_ENUM_FLAG_CLASS(event, mod, based::bu8, ENUM_EVENT_MOD)
#undef ENUM_EVENT_MOD

BASED_DEFINE_ENUM_CLASS(event, type, based::bu8, 0, ENUM_EVENT_TYPE)
#undef ENUM_EVENT_TYPE

inline event get_event()
{
  const auto chr = get_buffer().value().read();
  return {chr != 0 ? event::type::key : event::type::none, 0, chr};
}

}  // namespace alec
