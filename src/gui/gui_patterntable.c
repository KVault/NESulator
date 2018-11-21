#include "gui_patterntable.h"
#include "gui.h"

byte back_buffer[PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT * 3];//The 3 is the RGB


void render_tiles(tile *tiles, byte *back_buffer) {
	for (byte i = 0; i < TILES_PER_TABLE; ++i) {
		for (int j = 0; j < TILE_ROW_SIZE; ++j) {
			for (int k = 0; k < TILE_COLUMN_SIZE; ++k) {
				int columnIndex = ((i & 0x0F) * TILE_COLUMN_SIZE) + k;
				int rowIndex = ((i >> 4) * TILE_ROW_SIZE) + j;
				if (tiles[i].pattern[j][k] != 0) {
					int finalPos = PATTERNTABLE_TEXTURE_HEIGHT * rowIndex + columnIndex;

					//TODO remove that magic number. For debug only
					//And now 3 assignments. One for each color of RGB
					back_buffer[finalPos] = COLOUR_PALETTE[tiles[i].pattern[j][k] + 15].R;
					back_buffer[finalPos + 1] = COLOUR_PALETTE[tiles[i].pattern[j][k] + 15].G;
					back_buffer[finalPos +2] = COLOUR_PALETTE[tiles[i].pattern[j][k] + 15].B;
				}
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
	frameInfo.size = PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT * 3;

	return frameInfo;
}  