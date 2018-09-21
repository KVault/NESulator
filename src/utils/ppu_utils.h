#ifndef NESULATOR_PPU_UTILS_H
#define NESULATOR_PPU_UTILS_H

#include "colour_palette.h"
#include <stdlib.h>
#include "../memory.h"
#include "log.h"
#include "math.h"
#include "../rom.h"

#define TILE_ROW_SIZE 8
#define TILE_COLUMN_SIZE 8
#define TILES_PER_TABLE 0xFF
#define TILE_SIZE 64
#define PPU_PATTERN_LEFT vram_bank[0x0000]
#define PPU_PATTERN_RIGHT vram_bank[0x1000]

typedef enum {NT_TOP_LEFT, NT_TOP_RIGHT, NT_BOTTOM_LEFT, NT_BOTTOM_RIGHT} NametableIndex;

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

struct tile nametable_tile(uint tile_id);

/**
 * Uses the current mirroring to return the start address of the nametable index
 * i.e. NT_TOP_RIGHT would be 0x2000 with horizontal mirroring or 0x2400 with vertical mirroring.
 */
word get_nt_start_addr(NametableIndex nametableIndex);

void log_tile(struct tile *tile);

#endif //NESULATOR_PPU_UTILS_H
