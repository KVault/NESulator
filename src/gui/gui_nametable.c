#include <stdlib.h>
#include "gui_nametable.h"
#include "../memory.h"
#include "../ppu.h"

/**
 * Holds a pointer to each nametable back buffer
 */
static uint *nametables[4];

void render_tile(NametableIndex nametableIndex, tile *tile, uint row_id, uint column_id) {
	for (int i = 0; i < TILE_WIDTH; ++i) {
		for (int j = 0; j < TILE_HEIGHT; ++j) {
			int row = row_id * TILE_WIDTH + i;
			int col = column_id * TILE_HEIGHT + j;
			byte attribute = get_attribute(nametableIndex, row_id, column_id);
			colour *palette = get_background_palette(attribute);
			colour draw_colour = palette[tile->pattern[i][j]];
			nametables[nametableIndex][NAMETABLE_TEXTURE_WIDTH * row + col] = encode_as_RGBA(draw_colour);
		}
	}
}

void nametable_map(NametableIndex nametableIndex) {
	word start_addr = get_nt_start_addr(nametableIndex);
	for (uint i = 0; i < NAMETABLE_ROWS_MAP; ++i) {
		for (uint j = 0; j < NAMETABLE_TILES_PER_ROW; ++j) {
			byte tile_id = rmem_b_vram(start_addr + (i * NAMETABLE_TILES_PER_ROW + j));
			tile tile = nametable_tile(tile_id);
			render_tile(nametableIndex, &tile, i, j);
		}
	}
}

/**
 * Generic function that does all the processing
 */
FrameInfo nametable_frame(NametableIndex index){
	static tile *tiles;
	static FrameInfo frameInfo = {};
	static int nametable_buffer_size = NAMETABLE_TEXTURE_WIDTH * NAMETABLE_TEXTURE_HEIGHT;

	//First run through. Also configure the nametables array to point where it should
	if(tiles == NULL){
		tiles = calloc(TILES_PER_TABLE + 1, sizeof(tile));
		for(int i = 0; i < 4; ++i){
			nametables[i] = calloc(NAMETABLE_TEXTURE_WIDTH * NAMETABLE_TEXTURE_HEIGHT, sizeof(uint));
		}
	}

	nametable_map(index);

	frameInfo.buffer = nametables[index];
	frameInfo.size = nametable_buffer_size;
	frameInfo.width = NAMETABLE_TEXTURE_WIDTH;
	frameInfo.height = NAMETABLE_TEXTURE_HEIGHT;

	return frameInfo;
}

FrameInfo nametable(NametableIndex index){
	return nametable_frame(index);
}

