#ifndef NESULATOR_ROM_H
#define NESULATOR_ROM_H

#include "types.h"
#include <stdio.h>

/**
 * Loads a ROM from the specified filePath. What it does with it is still to be determined
 */
struct ROM *loadROM(char *filePath);

#endif //NESULATOR_ROM_H
