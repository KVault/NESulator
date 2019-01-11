#include "rom.h"
#include "mappers/NROM.h"
#include "mappers/CNROM.h"
#include <stdio.h>
#include <stdlib.h>

//Create the struct outside so that when we fill it in and exit the function, we don't_vram lose the reference
//The emulator can run only one ROM at a time, so this also makes sense (I think)

ROM rom = {};

void insertCartridge(char *filePath) {
	FILE *file;
	file = fopen(filePath, "rb");

	fread(&rom.nesTitle, 3, 1, file);
	fread(&rom.fileFormat, 1, 1, file);
	fread(&rom.numPRGPages, 1, 1, file);
	fread(&rom.numCHRPages, 1, 1, file);
	fread(&rom.flags6, 1, 1, file);
	fread(&rom.flags7, 1, 1, file);
	fread(&rom.endOfHeader, 8, 1, file);

	rom.mirroring = (uint)bit_test(rom.flags6, 0);
	rom.four_screen = (uint)bit_test(rom.flags6, 3);

	//Figure out the mapper number
	int lowerBits = (rom.flags6 & 0b11110000) >> 4; // extract upper 4 bits of the flag6 and use them as lower bits
	int upperBits = (rom.flags7 & 0b00001111); // extract lower 4 bits of the flag7 and use them as upper bits

	rom.mapper = lowerBits + upperBits;

	//Check the third bit to check if the ROM has a trainer
	if (bit_test(rom.flags6, 3)) { //if the trainer is there, then it's 512 bytes long. Always.
		rom.trainer = malloc(TRAINER_SIZE);
		fread(&rom.trainer, TRAINER_SIZE, 1, file);
	}

	rom.prgROM = malloc(rom.numPRGPages * PRG_PAGE_SIZE);
	rom.chrROM = malloc(rom.numCHRPages * CHR_PAGE_SIZE);

	fread(*&rom.prgROM, rom.numPRGPages * PRG_PAGE_SIZE, 1, file);
	fread(*&rom.chrROM, rom.numCHRPages * CHR_PAGE_SIZE, 1, file);

	fflush(file);
	fclose(file);
}

void load_ROM(ROM *rom){
	//Apply the mapper
	switch (rom->mapper){
		case 0:
			mapper0(rom);
		break;
		case 3:
			mapper3(rom);
		break;
		default:break;
	}
}

void ejectCartridge() {
	free(rom.trainer);
	free(rom.prgROM);
	free(rom.chrROM);
}

ROM *get_ROM(){
	return &rom;
}
