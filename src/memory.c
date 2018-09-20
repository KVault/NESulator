#include "nes.h"
#include "memory.h"

////// This functions are defined here so that when other file includes the .h, they won't be able to see this ones

/**
 * Generic function that deals with the actual writing to a memory bank. It'll be used to write both to RAM and VRAM
 * by suplying a pointer to the bank.
 */
void wmem(byte *memory_bank, unsigned short amountBytes, uint initialPosition, byte *content);

/**
 * Generic function that deals with the action reading from a memory bank. It'll be used to read from RAM and VRAM
 */
void rmem(byte *memoryBank, unsigned short amountBytes, uint initialPosition, byte *destiny);

/**
 * Helper function used to deal with the mirroring. When a write to RAM happens, this function will also
 * write to any mirrored address whenever necessary
 */
void ram_mirroring(unsigned short amountBytes, uint initialPosition, byte *content);

/**
 * Helper function used to deal with the mirroring. When a write to VRAM happens, this function will also
 * write to any mirrored address whenever necessary
 */
void vram_mirroring(unsigned short amountBytes, uint initialPosition, byte *content);

/**
 * RAM function
 * Write from content in a specific memory location
 * It also deals with RAM mirroring
 */
void wmem_ram(unsigned short amountBytes, uint initialPosition, byte *content);

/**
 * RAM function
 * Read content from a specific memory address and write to destiny
 */
void rmem_ram(unsigned short amountBytes, uint initialPosition, byte *destiny);

/**
 * VRAM function
 * Write from content to a specific memory location
 * It also deals with the VRAM mirroring
 */
void wmem_vram(unsigned short amountBytes, uint initialPosition, byte *content);

/**
 * Handle the different operations with the PPU
 */
void ppu_reg_write(uint initialPosition, byte content);

void ppu_reg_read(uint initialPosition);

/**
 * VRAM function
 * Reads from VRAM from a specific memory address. Writes it to destiny
 */
void rmem_vram(unsigned short amountBytes, uint initialPosition, byte *destiny);

void ram_mirroring(unsigned short amountBytes, uint initialPosition, byte *content) {
	int i = 0;
	if (initialPosition >= 0x0000 && initialPosition <= 0x07FF) {
		for (int k = 0; k < 4; k++) {
			uint mirrored_position = initialPosition + (0x0800 * k);
			for (int j = mirrored_position; i < amountBytes; j++, i++) {
				ram_bank[j] = content[i];
			}
		}
	}
	//TODO mirroring $2008-$3FFF
}

void vram_mirroring(unsigned short amountBytes, uint initialPosition, byte *content) {
	//TODO the actual mirroring. Nothing here for now
}

void rmem(byte *memoryBank, unsigned short amountBytes, uint initialPosition, byte *destiny) {
	int i = 0;
	for (int j = initialPosition; i < amountBytes; i++, j++) {
		destiny[i] = memoryBank[j];
	}
}

void wmem(byte *memoryBank, unsigned short amountBytes, uint initialPosition, byte *content) {
	int i = 0;
	for (int j = initialPosition; i < amountBytes; j++, i++) {
		memoryBank[j] = content[i];
	}
}

/* RAM MEMORY MAP
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

void wmem_ram(unsigned short amountBytes, uint initialPosition, byte *content) {
	ppu_reg_write(initialPosition, content[0]);
	wmem(ram_bank, amountBytes, initialPosition, content);
	ram_mirroring(amountBytes, initialPosition, content);
}

void rmem_ram(unsigned short amountBytes, uint initialPosition, byte *destiny) {
	ppu_reg_read(initialPosition);
	rmem(ram_bank, amountBytes, initialPosition, destiny);
}

void ppu_reg_write(uint initialPosition, byte content) {
	// TODO Check the mirroring
	switch (initialPosition) {
		case PPUADDR:
			write_PPUADDR(content);
			break;
		case PPUDATA:
			write_PPUDATA(content);
			break;
		default:break;
	}
}

void ppu_reg_read(uint initialPosition) {
	// TODO Check the mirroring
	switch (initialPosition) {
		case PPUDATA:
			read_PPUDATA();
			break;
		default:break;
	}
}

/**
 *  Due  to  the  difference  between  physical  and  logical  address  spaces,
 *  any  address  above  $3FFF  is  wrapped  around,  making  the  logical
 *  memory locations $4000-$FFFF effectively a mirror of locations $0000-$3FFF.
 */
void wmem_vram(unsigned short amountBytes, uint initialPosition, byte *content) {
	wmem(vram_bank, amountBytes, initialPosition, content);
	vram_mirroring(amountBytes, initialPosition, content);
}

void rmem_vram(unsigned short amountBytes, uint initialPosition, byte *destiny) {
	rmem(vram_bank, amountBytes, initialPosition, destiny);
}

byte rmem_b(uint address) {
	byte destiny = 0;
	rmem_ram(BYTE, address, &destiny);
	return destiny;
}

/**
 * Reads two bytes from the starting position and returns it as a memory address.
 * It will do the calculation for you, that's how nice this bad boy is
 */
word rmem_w(uint address) {
	byte destiny[2] = {0};
	rmem_ram(WORD, address, destiny);
	return to_mem_addr(destiny);
}

void wmem_b(uint address, byte content) {
	wmem_ram(BYTE, address, &content);
}

void wmem_w(uint address, word content) {
	byte wordVal[2] = {0};
	to_mem_bytes(content, wordVal);
	wmem_ram(WORD, address, wordVal);
}

byte rmem_b_vram(uint address) {
	byte destiny = 0;
	rmem_vram(BYTE, address, &destiny);
	return destiny;
}

word rmem_w_vram(uint start_address) {
	byte destiny[2] = {0};
	rmem_vram(WORD, start_address, destiny);
	return to_mem_addr(destiny);
}

void wmem_b_vram(uint address, byte content) {
	wmem_vram(BYTE, address, &content);
}

void wmem_w_vram(uint start_address, word content) {
	byte wordVal[2] = {0};
	to_mem_bytes(content, wordVal);
	wmem_vram(WORD, start_address, wordVal);
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
void zero_ram() {
	for (uint i = 0; i < RAM_MEM_SIZE; i++) {
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
		rmem_ram(WORD, b, memContent);
	}

	return to_mem_addr(memContent);
}

word indirecty_addr(byte b) {
	byte memContent[2] = {0};

	if (b == 0xFF) {
		memContent[0] = rmem_b(b);
		memContent[1] = rmem_b(0x00);
	} else {
		rmem_ram(WORD, b, memContent);
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