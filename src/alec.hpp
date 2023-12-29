#ifndef ALEC_ALEC_H
#define ALEC_ALEC_H

#include <format>
#include <iostream>

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

enum COLOR {
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

enum DECOR {
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

template <typename... Args> class CMD {
  public:
    constexpr CMD(const char c) : c(c) {}
    constexpr std::string operator()(Args... args) const {
        return std::format(fmt, (char)CTRL::ESC, args..., c);
    }

  private:
    const char c;

    static constexpr auto arr = []() {
        constexpr std::size_t len = sizeof...(Args) * 3 + 4;
        std::array<char, len + 1> arr = {'{', '}', '['};
        for (std::size_t i = 3; i < len - 1; i += 3) {
            arr[i] = '{', arr[i + 1] = '}', arr[i + 2] = ';';
        }
        arr[len - 2] = '{', arr[len - 1] = '}', arr[len] = 0;
        return arr;
    }();
    static constexpr auto fmt = std::string_view(arr.data(), arr.size() - 1);
};

static constexpr const CMD<int> CURSOR_UP('A');
static constexpr const CMD<int> CURSOR_DOWN('B');
static constexpr const CMD<int> CURSOR_FRWD('C');
static constexpr const CMD<int> CURSOR_BACK('D');
static constexpr const CMD<int> CURSOR_LINE_NEXT('E');
static constexpr const CMD<int> CURSOR_LINE_PREV('F');
static constexpr const CMD<int> CURSOR_COLUMN('G');
static constexpr const CMD<int> ERASE_DISPLAY('J');
static constexpr const CMD<int> ERASE_LINE('K');
static constexpr const CMD<int> SCROLL_UP('S');
static constexpr const CMD<int> SCROLL_DOWN('T');

static constexpr const CMD<int, int> CURSOR_POSITION('H');

// palet colors
static constexpr auto FOREGROUND(COLOR color) {
    static constexpr const CMD<int> bootstrap('m');
    return bootstrap(30 + color);
}

static constexpr auto BACKGROUND(COLOR color) {
    static constexpr const CMD<int> bootstrap('m');
    return bootstrap(40 + color);
}

// 256-color palette
static constexpr auto FOREGROUND(int idx) {
    static constexpr const CMD<int, int, int> bootstrap('m');
    return bootstrap(38, 5, idx);
}

static constexpr auto BACKGROUND(int idx) {
    static constexpr const CMD<int, int, int> bootstrap('m');
    return bootstrap(48, 5, idx);
}

// RGB colors
static constexpr auto FOREGROUND(int r, int g, int b) {
    static constexpr const CMD<int, int, int, int, int> bootstrap('m');
    return bootstrap(38, 5, r, g, b);
}

static constexpr auto BACKGROUND(int r, int g, int b) {
    static constexpr const CMD<int, int, int, int, int> bootstrap('m');
    return bootstrap(48, 5, r, g, b);
}

// Set/Reset text decorators
static constexpr auto DECOR_SET(DECOR decor) {
    static constexpr const CMD<int> bootstrap('m');
    return bootstrap(static_cast<int>(decor));
}

static constexpr auto DECOR_RESET(DECOR decor) {
    static constexpr const CMD<int> bootstrap('m');
    return bootstrap(static_cast<int>(decor) + 20);
}

template <class CharT, class Traits>
static constexpr std::basic_ostream<CharT, Traits> &CURSOR_SAVE(std::basic_ostream<CharT, Traits> &bos) {
    static auto sequence = CMD<>('s')();
    return bos << sequence;
}

template <class CharT, class Traits>
static constexpr std::basic_ostream<CharT, Traits> &CURSOR_LOAD(std::basic_ostream<CharT, Traits> &bos) {
    static auto sequence = CMD<>('u')();
    return bos << sequence;
}

template <class CharT, class Traits>
static constexpr std::basic_ostream<CharT, Traits> &CURSOR_SHOW(std::basic_ostream<CharT, Traits> &bos) {
    static auto sequence = CMD<const char *>('h')("?25");
    return bos << sequence;
}

template <class CharT, class Traits>
static constexpr std::basic_ostream<CharT, Traits> &CURSOR_HIDE(std::basic_ostream<CharT, Traits> &bos) {
    static auto sequence = CMD<const char *>('l')("?25");
    return bos << sequence;
}

template <class CharT, class Traits>
static constexpr std::basic_ostream<CharT, Traits> &ABUF_SHOW(std::basic_ostream<CharT, Traits> &bos) {
    static auto sequence = CMD<const char *>('h')("?1049");
    return bos << sequence;
}

template <class CharT, class Traits>
static constexpr std::basic_ostream<CharT, Traits> &ABUF_HIDE(std::basic_ostream<CharT, Traits> &bos) {
    static auto sequence = CMD<const char *>('l')("?1049");
    return bos << sequence;
}

} // namespace ALEC

#endif
