#include <cstdio>
#include <iostream>

#include "alec/alec.hpp"

using namespace alec;  // NOLINT

int main()
{
  std::cout << abuf_enable_v << cursor_hide_v;

  std::cout << cursor_position(1, 1) << foreground(91) << "HELLO!\n";

  std::cout << cursor_down(3);
  std::cout << foreground(30) << background(96, 53, 64) << "WORLD!\n";

  std::cout << background(color::def) << "testing 1...\n"
            << foreground(color::def);

  std::cout << decor_set(decor::inverse) << "testing 2...\n"
            << decor_reset(decor::inverse);

  std::cout << cursor_up(5) << "Hello there!" << cursor_save_v;
  std::cout << cursor_down(10) << "General Kenobi!";
  std::cout << cursor_position(10, 40) << "no pain no gain" << cursor_restore_v
            << cursor_show_v;

  (void)std::getchar();

  std::cout << abuf_disable_v;

  return 0;
}
