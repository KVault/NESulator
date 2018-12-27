#include "gui_patterntable.h"
#include "gui.h"

unsigned int left_buffer[PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT];
unsigned int right_buffer[PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT];


void render_tiles(tile *tiles, uint *back_buffer) {
	for (byte i = 0; i < TILES_PER_TABLE; ++i) {
		for (int j = 0; j < TILE_ROW_SIZE; ++j) {
			for (int k = 0; k < TILE_COLUMN_SIZE; ++k) {
				int columnIndex = ((i & 0x0F) * TILE_COLUMN_SIZE) + k;
				int rowIndex = ((i >> 4) * TILE_ROW_SIZE) + j;

				int colour_index  = rmem_b_vram(UNIVERSAL_BACKGROUND + tiles[i].pattern[j][k]);
				uint colour = encode_as_RGBA(COLOUR_PALETTE[colour_index]);
				back_buffer[PATTERNTABLE_TEXTURE_HEIGHT * rowIndex + columnIndex] = colour;
			}
		}
	}
}


/**
 * Generic function to be called. It will encode the patterntable, fill in the colours, struct and return it
 */
FrameInfo patterntable_frame(byte *mem_addr, unsigned int *buffer){
	static tile *tiles;
	static FrameInfo frameInfo = {};

	if (tiles == NULL) {
		tiles = calloc(TILES_PER_TABLE + 1, sizeof(tile));
	}

	encode_as_tiles(mem_addr, TILES_PER_TABLE, tiles);
	render_tiles(tiles, buffer);

	frameInfo.buffer = buffer;
	frameInfo.size = PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT;

	return frameInfo;
}

FrameInfo left_patterntable(){
	return patterntable_frame(&PPU_PATTERN_LEFT, left_buffer);
}

FrameInfo right_patterntable(){
	return patterntable_frame(&PPU_PATTERN_RIGHT, right_buffer);
}

