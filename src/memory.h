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
 * Reads one byte from the specified memory position
 */
byte rmem_b(unsigned int address);

/**
 * Reads a words starting at the specified memory position
 */
word rmem_w(unsigned int startAddress);

/**
 * Writes one byte at the specified memory position
 */
void wmem_b(unsigned int address, byte content);

/**
 * Writes two bytes starting at the specified memory position
 */
void wmem_w(unsigned int address, word content);

/**
 * Pops a byte from the stack
 */
byte pop_b();

/**
 * Pops a word from the stack
 */
word pop_w();

/**
 * Peeks a byte from stack, it doesn't modify the SP
 */
byte peek_b();

/**
 * Peeks a word from stack, it doesn't modify the SP
 */
word peek_w();

/**
 * Pushes a byte to the stack
 */
void push_b(byte content);

/**
 * Pushes a word to the stack
 */
void push_w(word content);


/**
 * returns the byte data as a memory address value. Basically it joins the bytes as it should to give back
 * a valid word.
 * This operation is the inverse as to_mem_addr
 */
word to_mem_addr(byte *content);

/**
 * Returns the memory address stored in the word as a byte array that can be written in the memory bank
 * This operation is the inverse as to_mem_addr
 */
void to_mem_bytes(word content, byte *result);

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
word zpagex_addr(byte b);

/**
 * Like zeropagex_addr but with the Y instead of X
 */
word zpagey_addr(byte b);

/**
 *  Like zeropagex_addr and zeropagey_addr but without using registers
 */
word zpage_addr(word w);

/**
 * Instructions using absolute addressing contain a full 16 bit address to identify the target location.
 */
word absolute_addr(word w);

/**
 * The address to be accessed by an instruction using X register indexed absolute addressing
 * is computed by taking the 16 bit address from the instruction and added the contents of the X register.
 */
word absolutex_addr(word w);

/**Y
 * The address to be accessed by an instruction using Y register indexed absolute addressing
 * is computed by taking the 16 bit address from the instruction and added the contents of the Y register.
 */
word absolutey_addr(word w);


#endif //NESULATOR_MEMORY_H

