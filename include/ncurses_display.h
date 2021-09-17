#ifndef NCURSES_DISPLAY_H
#define NCURSES_DISPLAY_H

#include <curses.h>

#include "process.h"
#include "system.h"

namespace NCursesDisplay {
constexpr int MAX_PROCESSES_DISPLAY = 18;
void Display(System &system, const int &n = MAX_PROCESSES_DISPLAY);
void DisplaySystem(System &system, WINDOW *window);
void DisplayProcesses(std::vector<Process> &processes, WINDOW *window, int n);
std::string ProgressBar(float percent);
}; // namespace NCursesDisplay

#endif