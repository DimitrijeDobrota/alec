#ifndef ALEC_ALEC_H
#define ALEC_ALEC_H

#include <array>
#include <cstdint>
#include <iostream>
#include <string_view>

namespace ALEC {

enum CTRL {
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

enum class COLOR {
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

enum class DECOR {
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

using enum COLOR;
using enum DECOR;

template <std::string_view const &...Strs> struct join {
    static constexpr auto arr = []() {
        constexpr std::size_t len = (Strs.size() + ... + 0);
        std::array<char, len + 1> arr{};
        auto append = [i = 0, &arr](auto const &s) mutable {
            for (auto c : s)
                arr[i++] = c;
        };
        (append(Strs), ...);
        arr[len] = 0;
        return arr;
    }();
    static constexpr std::string_view value{arr.data(), arr.size() - 1};
};

template <std::intmax_t N> class to_string_t {
  public:
    constexpr operator const auto &() const { return value; }

  private:
    static constexpr auto buf = []() {
        constexpr const auto len = []() {
            std::size_t len = 1;
            for (auto n = N; n; len++, n /= 10)
                ;
            return len;
        }();

        std::array<char, len> arr{};

        auto ptr = arr.data() + len;
        *--ptr = '\0';

        arr[0] = 0;
        for (auto n = N; n; n /= 10)
            *--ptr = '0' + n % 10;

        return arr;
    }();
    static constexpr std::string_view value{buf.data(), buf.size() - 1};
};

template <char C> class to_string_c {
  public:
    constexpr operator const auto &() const { return value; }

  private:
    static constexpr auto c = C;
    static constexpr std::string_view value{&c, 1};
};

static constexpr const std::string_view buffer = "?1049";
static constexpr const std::string_view cursor = "?25";
static constexpr const std::string_view esc = "\033[";
static constexpr const std::string_view sep = ";";

template <std::string_view const &...Strs> static constexpr auto concatenate = join<Strs...>::value;
template <std::string_view const &...Strs> static constexpr auto escape = join<esc, Strs...>::value;

template <std::intmax_t N> constexpr to_string_t<N> to_string;
template <char C> constexpr to_string_c<C> to_stringc;
template <std::intmax_t N> static constexpr auto to_stringp = join<to_string<N>, sep>::value;

// Move cursor up/down/frwd/back
template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_UP(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<n>, to_stringc<'A'>>;
}

template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_DOWN(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<n>, to_stringc<'B'>>;
}

template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_FRWD(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<n>, to_stringc<'C'>>;
}

template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_BACK(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<n>, to_stringc<'D'>>;
}

// Move cursor to the next/prev line
template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_LINE_NEXT(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<n>, to_stringc<'E'>>;
}

template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_LINE_PREV(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<n>, to_stringc<'F'>>;
}

// Set cursor to specific column
template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_COLUMN(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<n>, to_stringc<'G'>>;
}

// Erase functions
template <int n, class CharT, class Traits>
static constexpr auto &ERASE_DISPLAY(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<n>, to_stringc<'J'>>;
}

template <int n, class CharT, class Traits>
static constexpr auto &ERASE_LINE(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<n>, to_stringc<'K'>>;
}

// Scroll up/down
template <int n, class CharT, class Traits>
static constexpr auto &SCROLL_UP(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<n>, to_stringc<'S'>>;
}

template <int n, class CharT, class Traits>
static constexpr auto &SCROLL_DOWN(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<n>, to_stringc<'T'>>;
}

// Set cursor to a specific position
template <int n, int m, class CharT, class Traits>
static constexpr auto &CURSOR_POSITION(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_stringp<n>, to_string<m>, to_stringc<'H'>>;
}

// palet colors
template <COLOR color, class CharT, class Traits>
static constexpr auto &FOREGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<static_cast<uint>(color) + 30>, to_stringc<'m'>>;
}

template <COLOR color, class CharT, class Traits>
static constexpr auto &BACKGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<static_cast<uint>(color) + 40>, to_stringc<'m'>>;
}

// 256-color palette
template <int idx, class CharT, class Traits>
static constexpr auto &FOREGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_stringp<38>, to_stringp<5>, to_string<idx>, to_stringc<'m'>>;
}

template <int idx, class CharT, class Traits>
static constexpr auto &BACKGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_stringp<48>, to_stringp<5>, to_string<idx>, to_stringc<'m'>>;
}

// RGB colors
template <int R, int G, int B, class CharT, class Traits>
static constexpr auto &FOREGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_stringp<38>, to_stringp<5>, to_stringp<R>, to_stringp<G>, to_string<B>,
                         to_stringc<'m'>>;
}

template <int R, int G, int B, class CharT, class Traits>
static constexpr auto &BACKGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_stringp<48>, to_stringp<5>, to_stringp<R>, to_stringp<G>, to_string<B>,
                         to_stringc<'m'>>;
}

// Set/reset text decorators
template <DECOR decor, class CharT, class Traits>
static constexpr auto &DECOR_SET(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<static_cast<uint>(decor)>, to_stringc<'m'>>;
}

template <DECOR decor, class CharT, class Traits>
static constexpr auto &DECOR_RESET(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_string<static_cast<uint>(decor) + 20>, to_stringc<'m'>>;
}

// Savle/load cursor position
template <class CharT, class Traits>
static constexpr auto &CURSOR_SAVE(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_stringc<'s'>>;
}

template <class CharT, class Traits>
static constexpr auto &CURSOR_LOAD(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<to_stringc<'u'>>;
}

// Show/hide cursor
template <class CharT, class Traits>
static constexpr auto &CURSOR_SHOW(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<cursor, to_stringc<'h'>>;
}

template <class CharT, class Traits>
static constexpr auto &CURSOR_HIDE(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<cursor, to_stringc<'l'>>;
}

// Show/hide alternate buffer
template <class CharT, class Traits>
static constexpr auto &ABUF_SHOW(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<buffer, to_stringc<'h'>>;
}

template <class CharT, class Traits>
static constexpr auto &ABUF_HIDE(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<buffer, to_stringc<'l'>>;
}

} // namespace ALEC

#endif
