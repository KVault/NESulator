#ifndef NESULATOR_PPU_UTILS_H
#define NESULATOR_PPU_UTILS_H

#include "colour_palette.h"
#include <malloc.h>

struct tile{
	/**
	 * matrix of numbers from 0 to 3. Defines the shape of the tile
	 */
	byte pattern[8][8];
};

/**
 * Starting at "start_pos" encodes the next "number_tiles" as a pattern table.
 *
 * That is in blocks of 16 bytes and performing the left + right sum to obtain numbers from 0 to 3.
 */
struct tile *encode_as_tiles(byte *mem_addr, byte number_tiles) ;

#endif //NESULATOR_PPU_UTILS_H
