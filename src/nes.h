#ifndef NESULATOR_NES_H
#define NESULATOR_NES_H

#include "cpu.h"
#include "rom.h"

int stop_emulation();

//So that SDL stops complaining
#undef main

int run();

void every_second();

#endif //NESULATOR_NES_H
