#include "rom.h"

struct ROM {

	//Header
	byte nesTitle[3]; //Bytes 0-3 of the ROM
	byte numROMPages;  //Byte4. Number of 16384 byte program ROM pages. Byte4
	byte numCHRPages; //Byte5. Number of 8192 byte character ROM pages (0 indicates CHR RAM).
	/*	NNNN FTBM
		N: Lower 4 bits of the mapper number
		F: Four screen mode. 0 = no, 1 = yes. (When set, the M bit has no effect)
		T: Trainer.  0 = no trainer present, 1 = 512 byte trainer at 7000-71FFh
		B: SRAM at 6000-7FFFh battery backed.  0= no, 1 = yes
		M: Mirroring.  0 = horizontal, 1 = vertical.
	 */
	byte flags6; //byte 6

	/*
		NNNN xxPV

		N: Upper 4 bits of the mapper number
		P: Playchoice 10.  When set, this is a PC-10 game
		V: Vs. Unisystem.  When set, this is a Vs. game
		x: these bits are not used in iNES.
	 */
	byte flags7; //byte 7
	byte endOfHeader[8]; //bytes 8-15. These bytes are not used, and should be 00h.

	//ROM

	//CHR
};

struct ROM rom = {};


struct ROM *loadROM(char *filePath) {
	//Open the file
	//Figure out the total size of the file
	//Allocate memory for a byte[] of that same size
	//Go back to the init of the file
	//byte copy the file to the array

	FILE *file;
	file = fopen(filePath, "r");
	fseek(file, 0L, SEEK_END);//Go to the end of the file
	long fSize = ftell(file);//get the size
	rewind(file);//Get back to the beginning.

	int bytesRead = 0;

	bytesRead = fread(&rom.nesTitle, 3, 1, file);
	bytesRead = fread(&rom.numROMPages, 1, 1, file);
	bytesRead = fread(&rom.numCHRPages, 1, 1, file);
	bytesRead = fread(&rom.flags6, 1, 1, file);
	bytesRead = fread(&rom.flags7, 1, 1, file);
	bytesRead = fread(&rom.endOfHeader, 8, 1, file);

	return &rom;
}


void printHeaderDebugInfo(byte *fileData[]) {

}
