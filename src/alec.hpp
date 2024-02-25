#ifndef ALEC_ALEC_H
#define ALEC_ALEC_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>

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

enum class MOTION {
    END = 0,
    BEGIN = 1,
    WHOLE = 2,
};

namespace details {

template <std::size_t N> struct string_literal {
    consteval string_literal(const char (&str)[N]) { std::copy_n(str, N, value); }
    consteval std::size_t size() const { return N; }

    char value[N];
};

template <auto... Args> struct escape_t {
    template <typename T> static consteval std::size_t size(T val);
    template <typename T> static constexpr char *append(char *ptr, T val);

    template <std::size_t N> static constexpr std::size_t size(string_literal<N> val) { return val.size(); }
    static constexpr std::size_t size(char val) { return 1; }
    static constexpr std::size_t size(int val) {
        std::size_t len = 1;
        while (val /= 10)
            len++;
        return len;
    }

    template <std::size_t N> static constexpr char *append(char *ptr, string_literal<N> val) {
        std::copy_n(val.value, N, ptr);
        return ptr + N;
    }

    static constexpr char *append(char *ptr, char val) {
        *ptr++ = val;
        return ptr;
    }

    static constexpr char *append(char *ptr, int val) {
        char *tmp = ptr += size(val);
        do {
            *--tmp = '0' + (val % 10);
        } while (val /= 10);
        return ptr;
    }

    static constexpr const auto value = []() {
        constexpr std::size_t len = (size(Args) + ... + 2);
        std::array<char, len + 1> arr{CTRL::ESC, '[', 0};
        auto map = [ptr = arr.data() + 2](auto const &s) mutable { ptr = append(ptr, s); };
        (map(Args), ...);
        arr[len] = 0;
        return arr;
    }();
};

template <auto... Strs> static constexpr auto escape = escape_t<Strs...>::value.data();
template <details::string_literal... Strs> static constexpr auto escape_literal = escape<Strs...>;

} // namespace details

// Tamplate parameter constraints

template <int n>
concept limit_256 = n >= 0 && n < 256;

template <int n>
concept limit_pos = n >= 0;

// Move cursor up/down/frwd/back
template <int n>
    requires limit_pos<n>
static constexpr auto cursor_up_v = details::escape<n, 'A'>;

template <int n>
    requires limit_pos<n>
static constexpr auto cursor_down_v = details::escape<n, 'B'>;

template <int n>
    requires limit_pos<n>
static constexpr auto cursor_frwd_v = details::escape<n, 'C'>;

template <int n>
    requires limit_pos<n>
static constexpr auto cursor_back_v = details::escape<n, 'D'>;

// Move cursor to the next/prev line
template <int n>
    requires limit_pos<n>
static constexpr auto cursor_line_next_v = details::escape<n, 'E'>;

template <int n>
    requires limit_pos<n>
static constexpr auto cursor_line_prev_v = details::escape<n, 'F'>;

// Set cursor to specific column
template <int n>
    requires limit_pos<n>
static constexpr auto cursor_column_v = details::escape<n, 'G'>;

// Erase functions
template <MOTION m> static constexpr auto erase_display_v = details::escape<int(m), 'J'>;
template <MOTION m> static constexpr auto erase_line_v = details::escape<int(m), 'K'>;

// Scroll up/down
template <int n>
    requires limit_pos<n>
static constexpr auto scroll_up_v = details::escape<n, 'S'>;

template <int n>
    requires limit_pos<n>
static constexpr auto scroll_down_v = details::escape<n, 'T'>;

// Set cursor to a specific position
template <int n, int m>
    requires limit_pos<n> && limit_pos<m>
static constexpr auto cursor_position_v = details::escape<n, ';', m, 'H'>;

// color
template <auto... val> static const char *foreground_v;
template <auto... val> static const char *background_v;

// palet colors
template <COLOR color> static constexpr auto foreground_v<color> = details::escape<(int)color + 30, 'm'>;
template <COLOR color> static constexpr auto background_v<color> = details::escape<(int)color + 40, 'm'>;

// 256-color palette
template <int idx>
    requires limit_256<idx>
static constexpr auto foreground_v<idx> = details::escape<int(38), ';', int(5), ';', idx, 'm'>;

template <int idx>
    requires limit_256<idx>
static constexpr auto background_v<idx> = details::escape<int(48), ';', int(5), ';', idx, 'm'>;

// RGB colors
template <int R, int G, int B>
    requires limit_256<R> && limit_256<G> && limit_256<B>
static constexpr auto foreground_v<R, G, B> =
    details::escape<int(38), ';', int(5), ';', R, ';', G, ';', B, 'm'>;

template <int R, int G, int B>
    requires limit_256<R> && limit_256<G> && limit_256<B>
static constexpr auto background_v<R, G, B> =
    details::escape<int(48), ';', int(5), ';', R, ';', G, ';', B, 'm'>;

// Set/reset text decorators
template <DECOR decor> static constexpr auto decor_set_v = details::escape<(int)decor, 'm'>;
template <DECOR decor> static constexpr auto decor_reset_v = details::escape<(int)decor + 20, 'm'>;

// Save/load cursor position;
static constexpr auto cursor_save_v = details::escape<'s'>;
static constexpr auto cursor_load_v = details::escape<'u'>;

// Set screen modes

template <int n>
    requires limit_pos<n>
static constexpr auto screen_mode_set_v = details::escape<'=', n, 'h'>;

template <int n>
    requires limit_pos<n>
static constexpr auto screen_mode_reset_v = details::escape<'=', n, 'l'>;

// Private screen modes supported by most terminals

// Save/load screen
static constexpr auto screen_show_v = details::escape_literal<"?47h">;
static constexpr auto screen_hide_v = details::escape_literal<"?47l">;

// Show/hide cursor
static constexpr auto cursor_show_v = details::escape_literal<"?25h">;
static constexpr auto cursor_hide_v = details::escape_literal<"?25l">;

// Show/hide alternate buffer
static constexpr auto abuf_show_v = details::escape_literal<"?1049h">;
static constexpr auto abuf_hide_v = details::escape_literal<"?1049l">;

// Keyboard string TODO

} // namespace ALEC

#endif
