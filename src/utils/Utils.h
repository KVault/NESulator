#ifndef NESULATOR_TYPES_H
#define NESULATOR_TYPES_H

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;

#define BYTE 1
#define WORD 2

/**
 * Return the bitPos'th value of b
 */
int bit_test(byte b, byte bitPos);

/**
 * Set the bitPos'th bit of b
 */
void bit_set(byte *b, byte bitPos);

/**
 * Clear the bitPos'th bit of b
 */
void bit_clear(byte *b, byte bitPos);

/**
 * Set or clear (based on value) the bitPos'th bit of b
 */
void bit_val(byte *b, byte bitPos, int value);

#endif //NESULATOR_TYPES_H
