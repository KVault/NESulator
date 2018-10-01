#ifndef NESULATOR_NES_H
#define NESULATOR_NES_H

#include "utils/log.h"
#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "apu.h"
#include "stdlib.h"
#include "stdio.h"

int stop_emulation();

//So that SDL stops complaining
#undef main

int main();

void every_second();

#endif //NESULATOR_NES_H
