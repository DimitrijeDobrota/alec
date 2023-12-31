#include "alec.hpp"
#include <iostream>

int main(void) {

    std::cout << ALEC::ABUF_SHOW << ALEC::CURSOR_HIDE;

    std::cout << ALEC::CURSOR_POSITION<1, 1> << ALEC::FOREGROUND<91> << "HELLO!\n";

    std::cout << ALEC::CURSOR_DOWN<3>;
    std::cout << ALEC::FOREGROUND<30> << ALEC::BACKGROUND<196> << "WORLD!\n";

    std::cout << ALEC::BACKGROUND<ALEC::DEFAULT> << "testing 1...\n" << ALEC::FOREGROUND<ALEC::DEFAULT>;
    std::cout << ALEC::DECOR_SET<ALEC::INVERSE> << "testing 2...\n" << ALEC::DECOR_RESET<ALEC::INVERSE>;

    std::cout << ALEC::CURSOR_UP<5> << "Hello there!" << ALEC::CURSOR_SAVE;
    std::cout << ALEC::CURSOR_DOWN<10> << "General Kenobi!";
    std::cout << ALEC::CURSOR_POSITION<10, 40> << "no pain no gain" << ALEC::CURSOR_LOAD;

    getchar();

    std::cout << ALEC::CURSOR_SHOW << ALEC::ABUF_HIDE;

    return 0;
}
