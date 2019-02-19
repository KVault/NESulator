#ifndef NESULATOR_PPU_UTILS_H
#define NESULATOR_PPU_UTILS_H
#include "Utils.h"

typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} NametableIndex;
extern uint ppu_back_buffer[];

extern word c_vram;
extern word t_vram;
extern bool w;
extern bool nmi_output;
extern bool nmi_occurred;
extern int fine_x;

#define NES_SCREEN_WIDTH 256
#define NES_SCREEN_HEIGHT 240

#define PPUCTRL   0x2000
#define PPUSTATUS 0x2002
#define PPUADDR   0x2006
#define PPUDATA   0x2007
#define PPUSCROLL 0x2005

void write_PPUCTRL(byte value);
void write_PPUSCROLL(byte value);
void write_PPUADDR(byte value);
void write_PPUDATA(byte value);

byte read_PPUDATA();
byte read_PPUSTATUS();
#endif //NESULATOR_PPU_UTILS_H
