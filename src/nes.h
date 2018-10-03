#ifndef NESULATOR_NES_H
#define NESULATOR_NES_H

#include "cpu.h"
#include "rom.h"
#include "gui/python_gui_wrapper.h"

int stop_emulation();

//So that SDL stops complaining
#undef main

int main();

void every_second();

#endif //NESULATOR_NES_H
