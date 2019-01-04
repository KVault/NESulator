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
#define PPU_SCANLINES 261
#define PPU_VISIBLE_SCANLINES 240
#define PPU_POINT_PER_SCANLINE 340
#define NES_PPU_TEXTURE_HEIGHT 240
#define NES_PPU_TEXTURE_WIDTH 256

static enum {
	PPUCTRL_N = 0, PPUCTRL_I = 2, PPUCTRL_S = 3, PPUCTRL_B = 4, PPUCTRL_H = 5, PPUCTRL_P = 6, PPUCTRL_V = 7
} PPUCTRL_FLAGS;

word v; /** Current vram address */
word t; /** Temporary vram address aka address of the top left onscreen tile */
byte x; /** Find X scroll, 3 bits */
byte w; /** Latch, 1 bit */
uint ppu_running;
int ppu_cycle_per_cpu_cycle;  //Speed of the PPU in Hz. Used to slow down the emulation to match the NES's clock speed
static int ppu_cycles_this_sec;
static int current_scanline;
static int current_cycle_scanline;
extern uint ppu_back_buffer[];

//PPU latches and flags
static bool render_enabled;
static bool in_vblank;
static bool nmi_occurred;
static bool nmi_output;
static bool showBackground = TRUE;
static bool showSprites = TRUE;




//int speed = 0; // the speed of the PPU
//int scanline_ptr = 0; // in which scanline the PPU is currently. From 0 to 261
//int cycle_ptr = 0; // the cycle within the scanline. From 0 to 340

/**
 * Main function for the PPU. It runs on a thread and it takes care of keeping the right speed
 */
void *ppu_run(void *arg);

/**
 * Executes an instruction on the PPU.
 * Still to be defined what it means and what it needs to be happen on each cycle
 */
void ppu_cycle();

void ppu_power_up(int clock_speed);

void write_PPUADDR(byte value);

void write_PPUDATA(byte value);

void write_PPUCTRL(byte value);

byte read_PPUDATA();

byte read_PPUSTATUS();

void start_vblank();

void finish_vblank();

/**
 * Checks for the correct conditions to trigger an nmi. If they are met, then it triggers it
 */
void try_trigger_nmi();

/**
 * @return whether the current scanline is visible (Projected to the screen) or if it's a v-blank scanline
 */
bool visible_scanline();

/**
 * @return whether the current cycle is visible (Only the first 256 are.)
 */
bool visible_cycle();

#endif //NESULATOR_PPU_H
