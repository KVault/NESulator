#ifndef NESULATOR_NES_H
#define NESULATOR_NES_H

#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "apu.h"

typedef unsigned char byte;
typedef unsigned short nibble;
// TODO typedef unsigned ??? WORD. Should be 3 bytes.

#define BYTE 1;
#define NIBBLE 2;

void stopEmulation();

int main();

#endif //NESULATOR_NES_H
