#ifndef NESULATOR_NES_H
#define NESULATOR_NES_H

#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "apu.h"

typedef unsigned char BYTE;
typedef unsigned short NIBBLE;
// TODO typedef unsigned ??? WORD. Should be 3 bytes.

void stopEmulation();

int main();

#endif //NESULATOR_NES_H
