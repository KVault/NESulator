#ifndef NESULATOR_PPU_UTILS_H
#define NESULATOR_PPU_UTILS_H

#include "colour_palette.h"
#include <stdlib.h>
#include "../memory.h"

#define TILE_ROW_SIZE 8
#define TILE_COLUMN_SIZE 8
#define TILES_PER_TABLE 0xFF
#define PPU_PATTERN_LEFT vram_bank[0x0000]
#define PPU_PATTERN_RIGHT vram_bank[0x1000]


struct tile {
	/**
	 * matrix of numbers from 0 to 3. Defines the shape of the tile
	 */
	byte pattern[TILE_ROW_SIZE][TILE_COLUMN_SIZE];
};

/**
 * Starting at "start_pos" encodes the next "number_tiles" as a pattern table.
 *
 * That is in blocks of 16 bytes and performing the left + right sum to obtain numbers from 0 to 3.
 */
void encode_as_tiles(byte *mem_addr, byte number_tiles, struct tile *tiles);

/**
 * Gets a tile ID and returns a pointer to the first byte of that tile.
 * Note: To determine whether the tile comes from the left or the right PPUCTRL flag is used
 * @param tile_id
 * @return a pointer to the first byte
 */
byte *tile_addr(byte tile_id);

#endif //NESULATOR_PPU_UTILS_H
