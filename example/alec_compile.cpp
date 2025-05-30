#include <cstdio>
#include <iostream>

#include "alec/alec.hpp"

using namespace alec;  // NOLINT

int main()
{
  std::cout << abuf_enable_v << cursor_hide_v;

  std::cout << cursor_position_v<1, 1> << foreground_v<91> << "HELLO!\n";

  std::cout << cursor_down_v<3>;
  std::cout << foreground_v<30> << background_v<196, 53, 64> << "WORLD!\n";

  std::cout << background_v<color::def> << "testing 1...\n"
            << foreground_v<color::def>;

  std::cout << decor_set_v<decor::inverse> << "testing 2...\n"
            << decor_reset_v<decor::inverse>;

  std::cout << cursor_up_v<5> << "Hello there!" << cursor_save_v;
  std::cout << cursor_down_v<10> << "General Kenobi!";
  std::cout << cursor_position_v<10, 40> << "no pain no gain"
            << cursor_restore_v << cursor_show_v;

  (void)std::getchar();

  std::cout << abuf_disable_v;

  return 0;
}
