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

void wmem(unsigned short amountBytes, unsigned int initialPosition, byte *content) {
    int i = 0;
    for (int j = initialPosition; i < amountBytes; j++, i++) {
        memoryBank[j] = content[i];
    }

    //TODO mirroring
}

void wmem_const(unsigned short amountBytes, unsigned int initialPosition, byte const_val) {
    int i = 0;
    for (int j = initialPosition; i < amountBytes; j++, i++) {
        memoryBank[j] = const_val;
    }
}

void rmem(unsigned short amountBytes, unsigned int initialPosition, byte *destiny) {
    int i = 0;
    for (int j = initialPosition; i < amountBytes; i++, j++) {
        destiny[i] = memoryBank[j];
    }
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
    b = zeropagex_addr(b);

    rmem(WORD, b, memContent);

    return to_mem_addr(memContent);
}

word indirecty_addr(byte b) {
    byte memContent[2] = {0};

    rmem(WORD, b, memContent);
    word addr = to_mem_addr(memContent);

    return addr + Y;
}

word zeropagex_addr(byte b) {
    return (word) ((X + b) & 0x00FF);
}

word zeropagey_addr(byte b) {
    return (word) ((Y + b) & 0x00FF);
}

word zeropage_addr(word w) {
    return (word) (w & 0x00FF);
}

/**
 * Yes, this one does basically nothing, but we can't simply not use a function to use this mode. It
 * seems rather random and probably confusing in the future
 */
word absolute_addr(byte *b) {
    return to_mem_addr(&b);
}

word absolutex_addr(byte *b) {
    word addr = to_mem_addr(&b);
    addr += X;
    return addr;
}

word absolutey_addr(byte *b) {
    word addr = to_mem_addr(&b);
    addr += Y;
    return addr;
}

word to_mem_addr(byte *content) {
	return (content[1] << 8) + content[0];
}
