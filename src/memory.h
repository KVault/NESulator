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
void wmem(unsigned short amountBytes, unsigned int initialPosition, byte *content);

/**
 * Writes the same `repeatedValue` starting at `initialPosition` during `amountBytes` bytes
 */
void wmem_const(unsigned short amountBytes, unsigned int initialPosition, byte const_val);

/**
 * Read content from a specific memory address and write to destiny
 */
void rmem(unsigned short amountBytes, unsigned int initialPosition, byte *destiny);

/**
 * returns the byte data as a memory address value. Basically it joins the bytes as it should to give back
 * a valid word
 */
word to_mem_addr(byte *content);

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

/**
 * This works just like absolute indexed, but the target address is limited to the first 0xFF bytes.
 */
word zeropagex_addr(byte b);

/**
 * Like zeropagex_addr but with the Y instead of X
 */
word zeropagey_addr(byte b);

/**
 *  Like zeropagex_addr and zeropagey_addr but without using registers
 */
word zeropage_addr(word w);

/**
 * Instructions using absolute addressing contain a full 16 bit address to identify the target location.
 */
word absolute_addr(byte *b);

/**
 * The address to be accessed by an instruction using X register indexed absolute addressing
 * is computed by taking the 16 bit address from the instruction and added the contents of the X register.
 */
word absolutex_addr(byte *b);

/**Y
 * The address to be accessed by an instruction using Y register indexed absolute addressing
 * is computed by taking the 16 bit address from the instruction and added the contents of the Y register.
 */
word absolutey_addr(byte *b);


#endif //NESULATOR_MEMORY_H

