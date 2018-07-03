#ifndef NESULATOR_MEMORY_H
#define NESULATOR_MEMORY_H

#include "nes.h"

#define MEM_SIZE 65535

byte memoryBank[MEM_SIZE];

/**
 * Fills in the memory with zeroes. Just so we know what an empty memory looks like instead of garbage
 */
void zeroMemory();

/**
 * Write from content in a specific memory address
 * It also deals with memory mirroring
 */
void wmem(unsigned short amountBytes, unsigned short initialPosition, byte *content);

/**
 * Writes the same `repeatedValue` starting at `initialPosition` during `amountBytes` bytes
 */
void wmem_const(unsigned short amountBytes, unsigned short initialPosition, byte const_val);

/**
 * Read content from a specific memory address and write to destiny
 */
void rmem(unsigned short amountBytes, unsigned short initialPosition, byte *destiny);

/**
 * Indexed indirect addressing takes a single byte as an operand and adds the value of the X register to it to give the
 * address of the least significant byte of the target address.
 */
word indirectx_addr(byte b);

/**
 * This mode is only used with the Y register. It differs in the order that
 * Y is applied to the indirectly fetched address.
 *  To calculate the target address, the CPU will first fetch the address stored at zero page location $XX.
 *  That address will be added to register Y to get the final target address
 */
word indirecty_addr(byte b);

#endif //NESULATOR_MEMORY_H

