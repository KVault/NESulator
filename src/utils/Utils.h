#ifndef NESULATOR_TYPES_H
#define NESULATOR_TYPES_H

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;
typedef enum {false, true} bool;

#define UNIVERSAL_BACKGROUND_ADDR 0x3F00
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
 * Check if a two memory addresses are one page apart.
 * @param addr1
 * @param addr2
 * @return
 */
bool page_crossed(word addr1, word addr2);

void log_instruction(int num_params, const char *mnemonic, ...);

/** from here:
 * https://stackoverflow.com/questions/4439078/how-do-you-set-only-certain-bits-of-a-byte-in-c-without-affecting-the-rest
 * value = (value & ~mask) | (newvalue & mask);
 */
byte mask_byte(byte original_value, byte mask, byte mask_value);

/** from here:
 * https://stackoverflow.com/questions/4439078/how-do-you-set-only-certain-bits-of-a-byte-in-c-without-affecting-the-rest
 * value = (value & ~mask) | (newvalue & mask);
 */
word mask_word(word original_value, word mask, word mask_value);

/**
 * The Colour palette for the NES. The interpretation of the colors may change, but not its positions on it
 *
 * This might seem a bit random but it can be found here (https://wiki.nesdev.com/w/images/5/59/Savtool-swatches.png)
 */
extern const colour COLOUR_PALETTE[];

uint encode_as_RGBA(colour colour);


#endif //NESULATOR_TYPES_H
