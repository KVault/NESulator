#ifndef NESULATOR_PPU_H
#define NESULATOR_PPU_H

#include "memory.h"

#define PPUCTRL 0x2000
#define PPUMASK 0x2001
#define PPUSTATUS 0x2002
#define OAMADDR 0x2003
#define OAMDATA 0x2004
#define PPUSCROLL 0x2005
#define PPUADDR 0x2006
#define PPUDATA 0x2007
#define OAMDMA 0x4014

static enum {
	PPUCTRL_N = 0, PPUCTRL_I = 2, PPUCTRL_S = 3, PPUCTRL_B = 4, PPUCTRL_H = 5, PPUCTRL_P = 6, PPUCTRL_V = 7
} PPUCTRL_FLAGS;

//int speed = 0; // the speed of the PPU
//int scanline_ptr = 0; // in which scanline the PPU is currently. From 0 to 261
//int cycle_ptr = 0; // the cycle within the scanline. From 0 to 340

/**
 * Executes an instruction on the PPU.
 * Still to be defined what it means and what it needs to be happen on each cycle
 */
void ppu_cycle();

void ppu_power_up(int clock_speed);

#endif //NESULATOR_PPU_H
