#ifndef ALEC_ALEC_H
#define ALEC_ALEC_H

#include <array>
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
    static constexpr auto impl() noexcept {
        constexpr std::size_t len = (Strs.size() + ... + 0);
        std::array<char, len + 1> arr{};
        auto append = [i = 0, &arr](auto const &s) mutable {
            for (auto c : s)
                arr[i++] = c;
        };
        (append(Strs), ...);
        arr[len] = 0;
        return arr;
    }
    static constexpr auto arr = impl();
    static constexpr std::string_view value{arr.data(), arr.size() - 1};
};

static constexpr const std::string_view OP_ESC = "\033[";

template <std::string_view const &...Strs> static constexpr auto concatenate = join<Strs...>::value;
template <std::string_view const &...Strs> static constexpr auto escape = join<OP_ESC, Strs...>::value;

static constexpr const std::array<std::string_view, 256> l = {
    "0",   "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8",   "9",   "10",  "11",  "12",  "13",  "14",
    "15",  "16",  "17",  "18",  "19",  "20",  "21",  "22",  "23",  "24",  "25",  "26",  "27",  "28",  "29",
    "30",  "31",  "32",  "33",  "34",  "35",  "36",  "37",  "38",  "39",  "40",  "41",  "42",  "43",  "44",
    "45",  "46",  "47",  "48",  "49",  "50",  "51",  "52",  "53",  "54",  "55",  "56",  "57",  "58",  "59",
    "60",  "61",  "62",  "63",  "64",  "65",  "66",  "67",  "68",  "69",  "70",  "71",  "72",  "73",  "74",
    "75",  "76",  "77",  "78",  "79",  "80",  "81",  "82",  "83",  "84",  "85",  "86",  "87",  "88",  "89",
    "90",  "91",  "92",  "93",  "94",  "95",  "96",  "97",  "98",  "99",  "100", "101", "102", "103", "104",
    "105", "106", "107", "108", "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119",
    "120", "121", "122", "123", "124", "125", "126", "127", "128", "129", "130", "131", "132", "133", "134",
    "135", "136", "137", "138", "139", "140", "141", "142", "143", "144", "145", "146", "147", "148", "149",
    "150", "151", "152", "153", "154", "155", "156", "157", "158", "159", "160", "161", "162", "163", "164",
    "165", "166", "167", "168", "169", "170", "171", "172", "173", "174", "175", "176", "177", "178", "179",
    "180", "181", "182", "183", "184", "185", "186", "187", "188", "189", "190", "191", "192", "193", "194",
    "195", "196", "197", "198", "199", "200", "201", "202", "203", "204", "205", "206", "207", "208", "209",
    "210", "211", "212", "213", "214", "215", "216", "217", "218", "219", "220", "221", "222", "223", "224",
    "225", "226", "227", "228", "229", "230", "231", "232", "233", "234", "235", "236", "237", "238", "239",
    "240", "241", "242", "243", "244", "245", "246", "247", "248", "249", "250", "251", "252", "253", "254",
    "255",
};

static constexpr const std::array<std::string_view, 256> lp = {
    "0;",   "1;",   "2;",   "3;",   "4;",   "5;",   "6;",   "7;",   "8;",   "9;",   "10;",  "11;",  "12;",
    "13;",  "14;",  "15;",  "16;",  "17;",  "18;",  "19;",  "20;",  "21;",  "22;",  "23;",  "24;",  "25;",
    "26;",  "27;",  "28;",  "29;",  "30;",  "31;",  "32;",  "33;",  "34;",  "35;",  "36;",  "37;",  "38;",
    "39;",  "40;",  "41;",  "42;",  "43;",  "44;",  "45;",  "46;",  "47;",  "48;",  "49;",  "50;",  "51;",
    "52;",  "53;",  "54;",  "55;",  "56;",  "57;",  "58;",  "59;",  "60;",  "61;",  "62;",  "63;",  "64;",
    "65;",  "66;",  "67;",  "68;",  "69;",  "70;",  "71;",  "72;",  "73;",  "74;",  "75;",  "76;",  "77;",
    "78;",  "79;",  "80;",  "81;",  "82;",  "83;",  "84;",  "85;",  "86;",  "87;",  "88;",  "89;",  "90;",
    "91;",  "92;",  "93;",  "94;",  "95;",  "96;",  "97;",  "98;",  "99;",  "100;", "101;", "102;", "103;",
    "104;", "105;", "106;", "107;", "108;", "109;", "110;", "111;", "112;", "113;", "114;", "115;", "116;",
    "117;", "118;", "119;", "120;", "121;", "122;", "123;", "124;", "125;", "126;", "127;", "128;", "129;",
    "130;", "131;", "132;", "133;", "134;", "135;", "136;", "137;", "138;", "139;", "140;", "141;", "142;",
    "143;", "144;", "145;", "146;", "147;", "148;", "149;", "150;", "151;", "152;", "153;", "154;", "155;",
    "156;", "157;", "158;", "159;", "160;", "161;", "162;", "163;", "164;", "165;", "166;", "167;", "168;",
    "169;", "170;", "171;", "172;", "173;", "174;", "175;", "176;", "177;", "178;", "179;", "180;", "181;",
    "182;", "183;", "184;", "185;", "186;", "187;", "188;", "189;", "190;", "191;", "192;", "193;", "194;",
    "195;", "196;", "197;", "198;", "199;", "200;", "201;", "202;", "203;", "204;", "205;", "206;", "207;",
    "208;", "209;", "210;", "211;", "212;", "213;", "214;", "215;", "216;", "217;", "218;", "219;", "220;",
    "221;", "222;", "223;", "224;", "225;", "226;", "227;", "228;", "229;", "230;", "231;", "232;", "233;",
    "234;", "235;", "236;", "237;", "238;", "239;", "240;", "241;", "242;", "243;", "244;", "245;", "246;",
    "247;", "248;", "249;", "250;", "251;", "252;", "253;", "254;", "255;",
};

static constexpr const std::string_view OP_A = "A";
static constexpr const std::string_view OP_B = "B";
static constexpr const std::string_view OP_C = "C";
static constexpr const std::string_view OP_D = "D";
static constexpr const std::string_view OP_E = "E";
static constexpr const std::string_view OP_F = "F";
static constexpr const std::string_view OP_G = "G";
static constexpr const std::string_view OP_H = "H";
static constexpr const std::string_view OP_J = "J";
static constexpr const std::string_view OP_K = "K";
static constexpr const std::string_view OP_S = "S";
static constexpr const std::string_view OP_T = "T";
static constexpr const std::string_view OP_h = "h";
static constexpr const std::string_view OP_l = "l";
static constexpr const std::string_view OP_m = "m";
static constexpr const std::string_view OP_s = "s";
static constexpr const std::string_view OP_u = "u";
static constexpr const std::string_view buffer = "?1049";
static constexpr const std::string_view cursor = "?25";

// Move cursor up/down/frwd/back
template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_UP(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[n], OP_A>;
}

template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_DOWN(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[n], OP_B>;
}

template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_FRWD(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[n], OP_C>;
}

template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_BACK(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[n], OP_D>;
}

// Move cursor to the next/prev line
template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_LINE_NEXT(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[n], OP_E>;
}

template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_LINE_PREV(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[n], OP_F>;
}

// Set cursor to specific column
template <int n, class CharT, class Traits>
static constexpr auto &CURSOR_COLUMN(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[n], OP_G>;
}

// Erase functions
template <int n, class CharT, class Traits>
static constexpr auto &ERASE_DISPLAY(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[n], OP_J>;
}

template <int n, class CharT, class Traits>
static constexpr auto &ERASE_LINE(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[n], OP_K>;
}

// Scroll up/down
template <int n, class CharT, class Traits>
static constexpr auto &SCROLL_UP(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[n], OP_S>;
}

template <int n, class CharT, class Traits>
static constexpr auto &SCROLL_DOWN(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[n], OP_T>;
}

// Set cursor to a specific position
template <int n, int m, class CharT, class Traits>
static constexpr auto &CURSOR_POSITION(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<lp[n], l[m], OP_H>;
}

// palet colors
template <COLOR color, class CharT, class Traits>
static constexpr auto &FOREGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[static_cast<uint>(color) + 30], OP_m>;
}

template <COLOR color, class CharT, class Traits>
static constexpr auto &BACKGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[static_cast<uint>(color) + 40], OP_m>;
}

// 256-color palette
template <int idx, class CharT, class Traits>
static constexpr auto &FOREGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<lp[38], lp[5], l[idx], OP_m>;
}

template <int idx, class CharT, class Traits>
static constexpr auto &BACKGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<lp[48], lp[5], l[idx], OP_m>;
}

// RGB colors
template <int R, int G, int B, class CharT, class Traits>
static constexpr auto &FOREGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<lp[38], lp[5], lp[R], lp[G], l[B], OP_m>;
}

template <int R, int G, int B, class CharT, class Traits>
static constexpr auto &BACKGROUND(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<lp[48], lp[5], lp[R], lp[G], l[B], OP_m>;
}

// Set/reset text decorators
template <DECOR decor, class CharT, class Traits>
static constexpr auto &DECOR_SET(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[static_cast<uint>(decor)], OP_m>;
}

template <DECOR decor, class CharT, class Traits>
static constexpr auto &DECOR_RESET(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<l[static_cast<uint>(decor) + 20], OP_m>;
}

// Savle/load cursor position
template <class CharT, class Traits>
static constexpr auto &CURSOR_SAVE(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<OP_s>;
}

template <class CharT, class Traits>
static constexpr auto &CURSOR_LOAD(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<OP_u>;
}

// Show/hide cursor
template <class CharT, class Traits>
static constexpr auto &CURSOR_SHOW(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<cursor, OP_h>;
}

template <class CharT, class Traits>
static constexpr auto &CURSOR_HIDE(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<cursor, OP_l>;
}

// Show/hide alternate buffer
template <class CharT, class Traits>
static constexpr auto &ABUF_SHOW(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<buffer, OP_h>;
}

template <class CharT, class Traits>
static constexpr auto &ABUF_HIDE(std::basic_ostream<CharT, Traits> &bos) {
    return bos << escape<buffer, OP_l>;
}

} // namespace ALEC

#endif
