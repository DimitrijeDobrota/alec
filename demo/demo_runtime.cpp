#include "alec.hpp"
#include <iostream>

using namespace alec;
using enum Color;
using enum Decor;

int main(void) {
    std::cout << abuf_enable() << cursor_hide();

    std::cout << cursor_position(1, 1) << foreground(91) << "HELLO!\n";

    std::cout << cursor_down(3);
    std::cout << foreground(30) << background(96, 53, 64) << "WORLD!\n";

    std::cout << background(DEFAULT) << "testing 1...\n" << foreground(DEFAULT);
    std::cout << decor_set(INVERSE) << "testing 2...\n" << decor_reset(INVERSE);

    std::cout << cursor_up(5) << "Hello there!" << cursor_save();
    std::cout << cursor_down(10) << "General Kenobi!";
    std::cout << cursor_position(10, 40) << "no pain no gain" << cursor_restore() << cursor_show();

    getchar();

    std::cout << abuf_disable();

    return 0;
}
