#include "gui_nametable.h"

/**
 * Holds a pointer to each nametable back buffer
 */
uint *nametables[4];

uint top_left[NAMETABLE_TEXTURE_WIDTH * NAMETABLE_TEXTURE_HEIGHT];
uint top_right[NAMETABLE_TEXTURE_WIDTH * NAMETABLE_TEXTURE_HEIGHT];
uint bottom_left[NAMETABLE_TEXTURE_WIDTH * NAMETABLE_TEXTURE_HEIGHT];
uint bottom_right[NAMETABLE_TEXTURE_WIDTH * NAMETABLE_TEXTURE_HEIGHT];


void render_tile(NametableIndex nametableIndex, tile *tile, uint *back_buffer, uint row_id, uint column_id) {
	for (int i = 0; i < TILE_ROW_SIZE; ++i) {
		for (int j = 0; j < TILE_COLUMN_SIZE; ++j) {
			int row = row_id * TILE_ROW_SIZE + i;
			int col = column_id * TILE_COLUMN_SIZE + j;
			uint  draw_color = tile->pattern[i][j] != 0 ? 0xffffffff : 0xff000000;
			//byte attribute = get_attribute(nametableIndex, row_id, column_id);
			//colour *palette = get_background_palette(attribute);
			//draw_color = set_pixel(window.window, palette[tile->pattern[i][j]]);
			//width * row + col
			back_buffer[NAMETABLE_TEXTURE_WIDTH * row + col] = draw_color;
		}
	}
}

void nametable_map(NametableIndex nametableIndex) {
	word start_addr = get_nt_start_addr(nametableIndex);
	for (uint i = 0; i < NAMETABLE_ROWS_MAP; ++i) {
		for (uint j = 0; j < NAMETABLE_TILES_PER_ROW; ++j) {
			byte tile_id = rmem_b_vram(start_addr + (i * NAMETABLE_TILES_PER_ROW + j));
			tile tile = nametable_tile(tile_id);
			render_tile(nametableIndex, &tile, nametables[nametableIndex], i, j);
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
		nametables[0] = &top_left[0];
		nametables[1] = &top_right[0];
		nametables[2] = &bottom_left[0];
		nametables[3] = &bottom_right[0];
	}

	nametable_map(index);

	frameInfo.buffer = nametables[index];
	frameInfo.size = nametable_buffer_size;

	return frameInfo;
}

FrameInfo nametable(NametableIndex index){
	return nametable_frame(index);
}

