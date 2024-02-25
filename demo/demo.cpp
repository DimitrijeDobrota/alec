#include "alec.hpp"
#include <iostream>

using namespace ALEC;
using enum COLOR;
using enum DECOR;

int main(void) {
    std::cout << abuf_show_v << cursor_hide_v;

    std::cout << cursor_position_v<1, 1> << foreground_v<91> << "HELLO!\n";

    std::cout << cursor_down_v<3>;
    std::cout << foreground_v<30> << background_v<196, 53, 64> << "WORLD!\n";

    std::cout << background_v<DEFAULT> << "testing 1...\n" << foreground_v<DEFAULT>;
    std::cout << decor_set_v<INVERSE> << "testing 2...\n" << decor_reset_v<INVERSE>;

    std::cout << cursor_up_v<5> << "Hello there!" << cursor_save_v;
    std::cout << cursor_down_v<10> << "General Kenobi!";
    std::cout << cursor_position_v<10, 40> << "no pain no gain" << cursor_load_v << cursor_show_v;

    getchar();

    std::cout << abuf_hide_v;

    return 0;
}
