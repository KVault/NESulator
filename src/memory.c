#include "nes.h"
#include "memory.h"

/*
 * Address range  Size  Device
 * $0000-$07FF    $0800  2KB internal RAM
 * $0800-$0FFF    $0800  -----------------------
 * $1000-$17FF    $0800  Mirrors of $0000-$07FF
 * $1800-$1FFF    $0800  -----------------------
 * $2000-$2007    $0008  NES PPU registers
 * $2008-$3FFF    $1FF8  Mirrors of $2000-2007 (repeats every 8 bytes)
 * $4000-$4017    $0018  NES APU and I/O registers
 * $4018-$401F    $0008  APU and I/O functionality that is normally disabled. See CPU Test Mode.
 * $4020-$FFFF    $BFE0  Cartridge space: PRG ROM, PRG RAM, and mapper registers
 */

void wmem(unsigned short amountBytes, unsigned short initialPosition, byte *content) {
    int i = 0;
    for (int j = initialPosition; i < amountBytes; j++, i++) {
        memoryBank[j] = content[i];
    }

    //TODO mirroring
}

void wmem_const(unsigned short amountBytes, unsigned short initialPosition, byte const_val) {
    int i = 0;
    for (int j = initialPosition; i < amountBytes; j++, i++) {
        memoryBank[j] = const_val;
    }
}

void rmem(unsigned short amountBytes, unsigned short initialPosition, byte *destiny) {
    int i = 0;
    for (int j = initialPosition; i < amountBytes; i++, j++) {
        destiny[i] = memoryBank[j];
    }
    //TODO mirroring
}

/**
 * Zeroes the memory, pum, bam, gone, stiff, cold, dead.
 */
void zeroMemory() {
    wmem_const(MEM_SIZE, 0, 0);
}

/**
 * Basically the way it works, it gets the value of the parameter, adds the register X to it. That is an
 * address from which we get one byte, shift it left by 8 bits, read the next position and add t hat to the
 * shifted value. THAT is the indirectX address of it
 */
word indirectx_addr(byte b) {
    byte memContent[2] = {0};
    b += X;

    rmem(WORD, b, &memContent);

    return (memContent[1] << 8)+ memContent[0];
}
