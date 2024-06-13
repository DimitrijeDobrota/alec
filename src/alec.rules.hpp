#ifndef ALEC_ALEC_H
#define ALEC_ALEC_H

#include <algorithm>
#include <array>
#include <assert.h>
#include <cstdint>
#include <string>
#include <type_traits>


namespace alec {

enum Ctrl {
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

enum class Color {
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

enum class Decor {
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

enum class Motion {
    END = 0,
    BEGIN = 1,
    WHOLE = 2,
};

namespace details {

template <std::size_t N> struct string_literal {
    constexpr string_literal(const char (&str)[N]) { std::copy_n(str, N, value); }
    constexpr std::size_t size() const { return N; }

    char value[N];
};

namespace helper {
    template <std::size_t N> static constexpr std::size_t size(string_literal<N> val) { return N; }
    static constexpr std::size_t size(char val) { return 1; }
    static constexpr std::size_t size(int val) {
        std::size_t len = 1;
        while (val /= 10) len++;
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

    static const std::string make(auto... args) {
        std::string res((helper::size(args) + ... + 2), 0);
        res[0] = Ctrl::ESC, res[1] = '[';
        auto ptr = res.data() + 2;
        ((ptr = helper::append(ptr, args)), ...);
        return res;
    }


    template <auto... Args> struct escape_t {
        static constexpr const auto value = []() {
            std::array<char, (helper::size(Args) + ... + 3)> arr = {Ctrl::ESC, '[', 0};
            auto ptr = arr.data() + 2;
            ((ptr = helper::append(ptr, Args)), ...);
            return arr;
        }();
        static constexpr auto data = value.data();
    };
};

template <auto... Args> static constexpr auto escape = helper::escape_t<Args...>().data;
template <details::string_literal... Strs> static constexpr auto escape_literal = escape<Strs...>;

} // namespace details

// Tamplate parameter constraints

template <int n>
concept limit_256_v = n >= 0 && n < 256;

template <int n>
concept limit_pos_v = n >= 0;

static inline bool limit_pos(int n) { return n >= 0; };
static inline bool limit_256(int n) { return n >= 0 && n < 256; };

%%

// Move cursor up/down/frwd/back

    cursor_up
    int n
    limit_pos
    n, 'A'

    cursor_down
    int n
    limit_pos
    n, 'B'

    cursor_frwd
    int n
    limit_pos
    n, 'C'

    cursor_back
    int n
    limit_pos
    n, 'D'

// Move cursor to the next/prev line

    cursor_line_next
    int n
    limit_pos
    n, 'E'

    cursor_line_prev
    int n
    limit_pos
    n, 'F'

// Set cursor to specific column

    cursor_column
    int n
    limit_pos
    n, 'G'

// Erase functions

    erase_display
    Motion m
    |
    (int)m, 'J'

    erase_line
    Motion m
    |
    (int)m, 'K'

// Scroll up/down

    scroll_up
    int n
    limit_pos
    n, 'S'

    scroll_down
    int n
    limit_pos
    n, 'T'

// Set cursor to a specific position

    cursor_position
    int n, int m
    limit_pos
    n, ';', m, 'H'

// color

// palet colors

    foreground
    Color color
    |
    (int)color + 30, 'm'

    background
    Color color
    |
    (int)color + 40, 'm'

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
    int R, int G, int B
    limit_256
    38, ';', 2, ';', R, ';', G, ';', B, 'm'

    background
    int R, int G, int B
    limit_256
    48, ';', 2, ';', R, ';', G, ';', B, 'm'

// Set/reset text decorators

    decor_set
    Decor decor
    |
    (int)decor, 'm'

    decor_reset
    Decor decor
    |
    (int)decor + 20, 'm'

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
    int n
    limit_pos
    '=', n, 'h'

    screen_mode_reset
    int n
    limit_pos
    '=', n, 'l'

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

%%

// Keyboard string TODO

} // namespace ALEC


#endif
