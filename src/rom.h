#ifndef NESULATOR_ROM_H
#define NESULATOR_ROM_H

#include "Utils.h"
#include <stdio.h>

#define PRG_PAGE_SIZE 16384
#define CHR_PAGE_SIZE 8192
#define TRAINER_SIZE 512

struct ROM {
	//Header
	byte nesTitle[3]; //Bytes 0-3 of the ROM
	byte fileFormat;// for iNES format it should always be 0xA1 (26)
	uint numPRGPages;  //Byte4. Number of 16384 byte program ROM pages. Byte4
	uint numCHRPages; //Byte5. Number of 8192 byte character ROM pages (0 indicates CHR RAM).

	/*NNNN FTBM
	N: Lower 4 bits of the mapper number
	F: Four screen mode. 0 = no, 1 = yes. (When set, the M bit has no effect)
	T: Trainer.  0 = no trainer present, 1 = 512 byte trainer at 7000-71FFh
	B: SRAM at 6000-7FFFh battery backed.  0= no, 1 = yes
	M: Mirroring.  0 = horizontal, 1 = vertical.*/
	byte flags6; //byte 6

	/* NNNN xxPV
	N: Upper 4 bits of the mapper number
	P: Playchoice 10.  When set, this is a PC-10 game
	V: Vs. Unisystem.  When set, this is a Vs. game
	x: these bits are not used in iNES. */
	byte flags7; //byte 7
	byte endOfHeader[8]; //bytes 8-15. These bytes are not used, and should be 00h.

	//MAPPER
	int mapper;

	byte *trainer;

	//ROM
	byte *prgROM;

	//CHR
	byte *chrROM;
};

/**
 * Loads a ROM from the specified filePath. What it does with it is still to be determined
 */
struct ROM *insertCartridge(char *filePath);

/**
 * Cleans up the memory from the ROM. Basically it releases the mallocs
 */
void ejectCartridge();


#endif //NESULATOR_ROM_H
