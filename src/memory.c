#include "nes.h"
#include "memory.h"

////// This functions are defined here so that when other file includes the .h, they won't be able to see this ones

/**
 * Write from content in a specific memory address
 * It also deals with memory mirroring
 */
void wmem(unsigned short amountBytes, uint initialPosition, byte *content);

/**
 * Read content from a specific memory address and write to destiny
 */
void rmem(unsigned short amountBytes, uint initialPosition, byte *destiny);

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

void wmem(unsigned short amountBytes, uint initialPosition, byte *content) {
	int i = 0;
	for (int j = initialPosition; i < amountBytes; j++, i++) {
		memoryBank[j] = content[i];
	}

	if (initialPosition >= 0x0000 && initialPosition <= 0x07FF) {
		for (int k = 0; k < 4; k++) {
			uint mirrored_position = initialPosition + (0x0800 * k);
			for (int j = mirrored_position; i < amountBytes; j++, i++) {
				memoryBank[j] = content[i];
			}
		}
	}
	//TODO mirroring $2008-$3FFF
}

void rmem(unsigned short amountBytes, uint initialPosition, byte *destiny) {
	int i = 0;
	for (int j = initialPosition; i < amountBytes; i++, j++) {
		destiny[i] = memoryBank[j];
	}
}

byte rmem_b(uint address) {
	byte destiny = 0;
	rmem(BYTE, address, &destiny);
	return destiny;
}

/**
 * Reads two bytes from the starting position and returns it as a memory address.
 * It will do the calculation for you, that's how nice this bad boy is
 */
word rmem_w(uint address) {
	byte destiny[2] = {0};
	rmem(WORD, address, destiny);
	return to_mem_addr(destiny);
}

void wmem_b(uint address, byte content) {
	wmem(BYTE, address, &content);
}

void wmem_w(uint address, word content) {
	byte wordVal[2] = {0};
	to_mem_bytes(content, wordVal);
	wmem(WORD, address, wordVal);
}

byte pop_b() {
	uint bankPointer = SP + 1 + 0x100; // The stack is between 0x100 and 0x1FF
	SP++;
	return rmem_b(bankPointer);
}

word pop_w() {
	uint bankPointer = SP + 1 + 0x100; // The stack is between 0x100 and 0x1FF
	SP += 2;
	return rmem_w(bankPointer);
}

byte peek_b() {
	byte cachedSP = SP; // The stack is between 0x100 and 0x1FF
	byte value = pop_b();
	SP = cachedSP;
	return value;
}

word peek_w() {
	byte cachedSP = SP; // The stack is between 0x100 and 0x1FF
	word value = pop_w();
	SP = cachedSP;
	return value;
}

void push_b(byte content) {
	uint bankPointer = SP + 0x100; // The stack is between 0x100 and 0x1FF
	wmem_b(bankPointer, content);
	SP -= 1;
}

void push_w(word content) {
	//TODO. this is wrong, but atm I just want to make it compile
	uint bankPointer = SP + 0xFF; // 100 - 1 but in hex -> 99 is 0xFF
	wmem_w(bankPointer, content);
	SP -= 2;
}

/**
 * Zeroes the memory, pum, bam, gone, stiff, cold, dead.
 */
void zeroMemory() {
	for (uint i = 0; i < MEM_SIZE; i++) {
		wmem_b(i, 0);
	}
}

/**
 * Basically the way it works, it gets the value of the parameter, adds the register X to it. That is an
 * address from which we get one byte, shift it left by 8 bits, read the next position and add t hat to the
 * shifted value. THAT is the indirectX address of it
 */
word indirectx_addr(byte b) {
	byte memContent[2] = {0};
	b = (byte) zpagex_addr(b);
	if (b == 0xFF) {
		memContent[0] = rmem_b(b);
		memContent[1] = rmem_b(0x00);
	} else {
		rmem(WORD, b, memContent);
	}

	return to_mem_addr(memContent);
}

word indirecty_addr(byte b) {
	byte memContent[2] = {0};

	if (b == 0xFF) {
		memContent[0] = rmem_b(b);
		memContent[1] = rmem_b(0x00);
	} else {
		rmem(WORD, b, memContent);
	}

	word addr = to_mem_addr(memContent);

	return addr + Y;
}

word zpagex_addr(byte b) {
	return (word) ((X + b) & 0x00FF);
}

word zpagey_addr(byte b) {
	return (word) ((Y + b) & 0x00FF);
}

word zpage_addr(word w) { // should it be a byte?
	return (word) (w & 0x00FF);
}

/**
 * Yes, this one does basically nothing, but we can't simply not use a function to use this mode. It
 * seems rather random and probably confusing in the future
 */
word absolute_addr(word w) {
	return w;
}

word absolutex_addr(word w) {
	w += X;
	return w;
}

word absolutey_addr(word w) {
	w += Y;
	return w;
}

word to_mem_addr(byte *content) {
	return (content[1] << 8) + content[0];
}

void to_mem_bytes(word content, byte *result) {
	byte mostSigByte = (byte) ((content & 0xFF00) >> 8);
	byte leastSigByte = (byte) (content & 0x00FF);
	result[0] = leastSigByte;
	result[1] = mostSigByte;
}
