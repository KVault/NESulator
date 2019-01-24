#ifndef NESULATOR_TYPES_H
#define NESULATOR_TYPES_H

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;
typedef enum {false, true} bool;
typedef struct{
	byte R;
	byte G;
	byte B;
} colour;

#define BYTE 1
#define WORD 2

#define MILLISECOND 1000
#define MICROSECOND 1000000
#define NANOSECOND  1000000000

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

/**
 * Applies the given mask to change the bits set to 1 in that mask to the value passed in
 * @param source Initial value to be changed
 * @param mask Mask with all the bits to be changed set to 1
 * @param value New value to be applied to the mask and therefore to the old value
 * @return
 */
byte byte_mask(byte source, byte mask, byte value);

/**
 * Applies the given mask to change the bits set to 1 in that mask to the value passed in
 * @param source Initial value to be changed
 * @param mask Mask with all the bits to be changed set to 1
 * @param value New value to be applied to the mask and therefore to the old value
 * @return
 */
word word_mask(word source, word mask, word value);

/**
 * The Colour palette for the NES. The interpretation of the colors may change, but not its positions on it
 *
 * This might seem a bit random but it can be found here (https://wiki.nesdev.com/w/images/5/59/Savtool-swatches.png)
 */
extern const colour COLOUR_PALETTE[];

uint encode_as_RGBA(colour colour);


#endif //NESULATOR_TYPES_H
