#ifndef CATCH_CONFIG_MAIN
#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  NCursesDisplay::Display(system);
}
#endif