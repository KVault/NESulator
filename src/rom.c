#include <stdlib.h>
#include "rom.h"

//Create the struct outside so that when we fill it in and exit the function, we don't lose the reference
//The emulator can run only one ROM at a time, so this also makes sense
struct ROM rom = {};

struct ROM *insertCartridge(char *filePath) {
	FILE *file;
	file = fopen(filePath, "rb");

	fread(&rom.nesTitle, 3, 1, file);
	fread(&rom.fileFormat, 1, 1, file);
	fread(&rom.numPRGPages, 1, 1, file);
	fread(&rom.numCHRPages, 1, 1, file);
	fread(&rom.flags6, 1, 1, file);
	fread(&rom.flags7, 1, 1, file);
	fread(&rom.endOfHeader, 8, 1, file);

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

	for (int i = 0; i < rom.numPRGPages * PRG_PAGE_SIZE; ++i) {
		fread(&rom.prgROM, 1, 1, file);
	}

	for(int i = 0; i < rom.numCHRPages * CHR_PAGE_SIZE; i++){
		fread(&rom.chrROM, 1, 1, file);
	}

	fflush(file);
	fclose(file);
	return &rom;
}

void loadROM(struct ROM *rom){
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
