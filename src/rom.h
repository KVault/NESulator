#ifndef NESULATOR_ROM_H
#define NESULATOR_ROM_H

#include <stdio.h>
#include "nes.h"

/**
 * Pretty prints the value of the header of the given NES2.0 file format
 */
void printHeaderDebugInfo(byte *fileData[]);

/**
 * Loads a ROM from the specified filePath. What it does with it is still to be determined
 */
void loadROM(char* filePath);

#endif //NESULATOR_ROM_H
