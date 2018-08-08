#include "CNROM.h"

const unsigned int first_rom_page = 0x8000;
const unsigned int second_rom_page = 0xC000;
const unsigned int prg_ram = 0x6000;

void mapper3(struct ROM *rom) {
	for (int i = 0; i < PRG_PAGE_SIZE; ++i) {
		wmem_b(first_rom_page + i, rom->prgROM[i]);
	}

	unsigned int mirror_starting_point = rom->numPRGPages == 1 ? 0 : PRG_PAGE_SIZE;

	for (int i = 0; i < PRG_PAGE_SIZE; ++i) {
		wmem_b(second_rom_page + i, rom->prgROM[i + mirror_starting_point]);
	}
	// TODO CHR stuff goes here
}
