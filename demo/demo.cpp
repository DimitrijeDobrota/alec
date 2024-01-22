#include "alec.hpp"
#include <iostream>

using namespace ALEC;
using enum COLOR;
using enum DECOR;

int main(void) {
    std::cout << ABUF_SHOW << CURSOR_HIDE;

    std::cout << CURSOR_POSITION<1, 1> << FOREGROUND<91> << "HELLO!\n";

    std::cout << CURSOR_DOWN<3>;
    std::cout << FOREGROUND<30> << BACKGROUND<196, 53, 64> << "WORLD!\n";

    std::cout << BACKGROUND<DEFAULT> << "testing 1...\n" << FOREGROUND<DEFAULT>;
    std::cout << DECOR_SET<INVERSE> << "testing 2...\n" << DECOR_RESET<INVERSE>;

    std::cout << CURSOR_UP<5> << "Hello there!" << CURSOR_SAVE;
    std::cout << CURSOR_DOWN<10> << "General Kenobi!";
    std::cout << CURSOR_POSITION<10, 40> << "no pain no gain" << CURSOR_LOAD << CURSOR_SHOW;

    getchar();

    std::cout << ABUF_HIDE;

    return 0;
}
