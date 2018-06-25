#ifndef NESULATOR_MEMORY_H
#define NESULATOR_MEMORY_H

#include "nes.h"

/**
 * Write from content in a specific memory address
 * It also deals with memory mirroring
 * @param amountBytes
 * @param initialPosition
 * @param content
 */
void wmem(unsigned short amountBytes, unsigned short initialPosition, byte *content[]);

/**
 * Read content from a specific memory address and write to destiny
 * @param amountBytes
 * @param initialPosition
 * @param destiny
 */
void rmem(unsigned short amountBytes, unsigned short initialPosition, byte *destiny[]);

#endif //NESULATOR_MEMORY_H

