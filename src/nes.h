#ifndef NESULATOR_NES_H
#define NESULATOR_NES_H

#include "cpu.h"
#include "rom.h"

/**
 * Of course, only one ROM can be in the console at the same time. This is the one
 */
extern ROM rom;

int stop_emulation();

int start_emulation();

/**
 * Prepares the log levels, log directories and the CPU/PPU/APU for the console to run
 */
void configure();

void every_second();

#endif //NESULATOR_NES_H
