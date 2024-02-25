#include "alec.hpp"
#include <iostream>

using namespace ALEC;
using enum COLOR;
using enum DECOR;

int main(void) {
    std::cout << abuf_show() << cursor_hide();

    std::cout << cursor_position(1, 1) << foreground(91) << "HELLO!\n";

    std::cout << cursor_down(3);
    std::cout << foreground(30) << background(196, 53, 64) << "WORLD!\n";

    std::cout << background(DEFAULT) << "testing 1...\n" << foreground(DEFAULT);
    std::cout << decor_set(INVERSE) << "testing 2...\n" << decor_reset(INVERSE);

    std::cout << cursor_up(5) << "Hello there!" << cursor_save();
    std::cout << cursor_down(10) << "General Kenobi!";
    std::cout << cursor_position(10, 40) << "no pain no gain" << cursor_load() << cursor_show();

    getchar();

    std::cout << abuf_hide();

    return 0;
}
