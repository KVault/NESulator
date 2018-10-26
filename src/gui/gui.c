#include "gui.h"

patterntable *fill_patterntable(){
	static tile *left_tiles;
	static tile *right_tiles;
	static patterntable table;

	//TODO Could we do this allocating only the patterntable?
	//Only allocate it the first time
	if (left_tiles == NULL) {
		left_tiles = calloc(TILES_PER_TABLE + 1, sizeof(tile));
	}
	if (right_tiles == NULL) {
		right_tiles = calloc(TILES_PER_TABLE + 1, sizeof(tile));
	}
	if(table.buffer == NULL){
		table.size = (PATTERNTABLE_WIDTH * PATTERNTABLE_HEIGHT) * 3;//The 3 is the amount of bytes per colour. (RGB)
		table.buffer = calloc(table.size, sizeof(byte));
	}

	encode_as_tiles(&PPU_PATTERN_LEFT, TILES_PER_TABLE, left_tiles);
	encode_as_tiles(&PPU_PATTERN_RIGHT, TILES_PER_TABLE, right_tiles);
	render_tiles(left_tiles, &table, LeftPatterntable);
	render_tiles(right_tiles, &table, RightPatterntable);

	return &table;
}

void render_tiles(tile *tiles, patterntable *patterntable, enum PatterntableSide side) {
	/** Right ok, so because we're now filling in one bid buffer for the entite patterntable, we need a way of
	  	knowhing where the patterntable actually starts. So this variable will take the value 0 if the rendering the
	  	left one or TILE_COLUMN_SIZE (256) if rendering the right one. */
	int start_col_possition = side == LeftPatterntable ? 0 : TILE_COLUMN_SIZE;

	/** And for this one we need to know where to stop. So stop at 256 if rendering the left one or 512 if doing
	 	the right one. */
	int finish_col_possition = side == LeftPatterntable ? TILE_COLUMN_SIZE : TILE_COLUMN_SIZE * 2;

	for (byte i = 0; i < TILES_PER_TABLE; ++i) {
		for (int j = 0; j < TILE_ROW_SIZE; ++j) {
			for (int k = start_col_possition; k < finish_col_possition; ++k) {
				int columnIndex = ((i & 0x0F) * TILE_COLUMN_SIZE) + k;
				int rowIndex = ((i >> 4) * TILE_ROW_SIZE) + j;
				if (tiles[i].pattern[j][k] != 0) {
					//TODO remove that magic number. For debug only
					colour target_colour = COLOUR_PALETTE[tiles[i].pattern[j][k] + 15];
					int pos = PATTERNTABLE_HEIGHT * rowIndex + columnIndex;
					patterntable->buffer[pos] = target_colour.R;
					patterntable->buffer[pos + 1] = target_colour.G;
					patterntable->buffer[pos + 2] = target_colour.B;
				}
			}
		}
	}
}
