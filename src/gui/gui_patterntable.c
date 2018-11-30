#include "gui_patterntable.h"
#include "gui.h"

unsigned int back_buffer[PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT];


void render_tiles(tile *tiles, unsigned int *back_buffer) {
	for (int i = 0; i < TILES_TOTAL; ++i) {
		for (int j = 0; j < TILE_ROW_SIZE; ++j) {
			for (int k = 0; k < TILE_COLUMN_SIZE; ++k) {
				//The 3 is used because every pixel has 3 bytes in it.
				//A[x,y] would then be mapped to DIM1[x + y * Xn]
				static int stride;
				stride = TILE_ROW_SIZE * TILES_PER_ROW;
				int finalPos = (j + k * stride);

				//TODO remove that magic number. For debug only
				unsigned int colour = i%2 == 0 ? 0xff000000:0xff000000;//COLOUR_PALETTE[tiles[i].pattern[j][k] + 15].R;
				back_buffer[finalPos] = colour;//(color << 24) + (color << 16) + (color << 8) + color;
		 	}
		}
	}
}

FrameInfo patterntable_frame(){
	static tile *tiles;
	static FrameInfo frameInfo = {};

	if (tiles == NULL) {
		tiles = calloc(TILES_TOTAL + 1, sizeof(tile));
	}

	encode_as_tiles(&PPU_PATTERN_LEFT, TILES_TOTAL, tiles);
	render_tiles(tiles, back_buffer);

	frameInfo.buffer = back_buffer;
	frameInfo.size = PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT;

	return frameInfo;
}