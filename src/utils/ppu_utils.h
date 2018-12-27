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
#define PPU_PATTERN_LEFT vram_bank[0x0000]
#define PPU_PATTERN_RIGHT vram_bank[0x1000]

typedef enum {
	NT_TOP_LEFT, NT_TOP_RIGHT, NT_BOTTOM_LEFT, NT_BOTTOM_RIGHT
} NametableIndex;

typedef enum {
	AT_TOP_LEFT, AT_TOP_RIGHT, AT_BOTTOM_LEFT, AT_BOTTOM_RIGHT
} AttributeTableIndex;

/**
 * Memory addresses for the Background palettes. These are the entry points and each palette holds 3 colours
 */
extern word BACKGROUND_PALETTES[4];
extern word UNIVERSAL_BACKGROUND;

typedef struct{
	/**
	 * matrix of numbers from 0 to 3. Defines the shape of the tile
	 */
	byte pattern[TILE_ROW_SIZE][TILE_COLUMN_SIZE];
} tile;

/**
 * Starting at "start_pos" encodes the next "number_tiles" as a pattern table.
 *
 * That is in blocks of 16 bytes and performing the left + right sum to obtain numbers from 0 to 3.
 */
void encode_as_tiles(byte *mem_addr, uint number_tiles, tile *tiles);

tile nametable_tile(byte tile_id);

/**
 * Uses the current mirroring to return the start address of the nametable index
 * i.e. NT_TOP_RIGHT would be 0x2000 with horizontal mirroring or 0x2400 with vertical mirroring.
 */
word get_nt_start_addr(NametableIndex nametableIndex);

word get_at_start_addr(AttributeTableIndex attributeTableIndex);

void log_tile(tile *tile);

byte get_attribute(NametableIndex nametableIndex, int row_id, int column_id);

colour *get_background_palette(byte attribute);

#endif //NESULATOR_PPU_UTILS_H
