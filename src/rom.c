#include <stdlib.h>
#include "rom.h"

struct ROM {
    //Header
    byte nesTitle[3]; //Bytes 0-3 of the ROM
    byte fileFormat;// for iNES format it should always be 0xA1 (26)
    byte numPRGPages;  //Byte4. Number of 16384 byte program ROM pages. Byte4
    byte numCHRPages; //Byte5. Number of 8192 byte character ROM pages (0 indicates CHR RAM).

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

    //TRAINER 0 or 512 bytes
    int trainerSize;//We need to keep track of the size for when we free up the memory
    byte *trainer;

    //ROM
    byte *prgROM;

    //CHR
    byte *chrROM;
};

//Create the struct outside so that when we fill it in and exit the function, we don't lose the reference
//The emulator can run only one ROM at a time, so this also makes sense
struct ROM rom = {};


struct ROM *loadROM(char *filePath) {
    FILE *file;
    file = fopen(filePath, "r");
    fseek(file, 0L, SEEK_END);//Go to the end of the file
    long fSize = ftell(file);//get the size
    rewind(file);//Get back to the beginning.

    //Used to track count of the bytes read throughout
    int bytesRead = 0;

    bytesRead += fread(&rom.nesTitle, 3, 1, file);
    bytesRead += fread(&rom.fileFormat, 1, 1, file);
    bytesRead += fread(&rom.numPRGPages, 1, 1, file);
    bytesRead += fread(&rom.numCHRPages, 1, 1, file);
    bytesRead += fread(&rom.flags6, 1, 1, file);
    bytesRead += fread(&rom.flags7, 1, 1, file);
    bytesRead += fread(&rom.endOfHeader, 8, 1, file);

    //Figure out the mapper number
    int lowerBits = (rom.flags6 & 0b11110000) >> 4; // extract upper 4 bits of the flag6 and use them as lower bits
    int upperBits = (rom.flags7 & 0b00001111); // extract lower 4 bits of the flag7 and use them as upper bits

    rom.mapper = lowerBits + upperBits;

    //Check the third bit to check if the ROM has a trainer
    int hasTrainer = rom.flags6 & 0b00000100;
    if (hasTrainer) { //if the trainer is there, then it's 512 bytes long. Always.
        rom.trainerSize = 512;
        rom.trainer = (byte *) malloc(rom.trainerSize);
        for (int i = 0; i < rom.trainerSize; i++) {
            fread(&rom.trainer[i], 1, 1, file);
        }
    }

    fflush(file);
    fclose(file);
    return &rom;
}

void cleanupROM() {
    free(rom.trainer);
    free(rom.prgROM);
    free(rom.chrROM);
}
