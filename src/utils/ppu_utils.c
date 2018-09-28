#include "ppu_utils.h"

void log_tile(tile *tile) {
	for (int i = 0; i < TILE_ROW_SIZE; ++i) {
		for (int j = 0; j < TILE_ROW_SIZE; ++j) {
			tile->pattern[i][j] ? log_info("%i", tile->pattern[i][j]) : log_info(".");
		}
		log_info("\n");
	}
}

void encode_as_tiles(byte *mem_addr, byte number_tiles, tile *tiles) {
	for (int i, tile_count = i = 0;
	     tile_count < number_tiles; i += 16, ++tile_count) { //Each tile is defined by 16 bytes
		for (int j = 0; j < TILE_ROW_SIZE; ++j) { //And each byte defines a row of pixels with a value from 0 to 3
			for (byte k = 0; k < TILE_COLUMN_SIZE; ++k) {
				//Get the bit j from the current iteration and 8 positions ahead. Then add those two bits so that
				//you get one of the following: 00, 01, 10, 11.
				byte most_sig_bit = (byte) bit_test(mem_addr[j + i], k) << 1;
				byte least_sig_bit = (byte) bit_test(mem_addr[j + i + TILE_ROW_SIZE], k);

				//use the abs to flip the tile in the Y component. Otherwise it comes out wrong.
				tiles[tile_count].pattern[j][abs(k - TILE_COLUMN_SIZE)] = most_sig_bit + least_sig_bit;
			}
		}
	}
}

tile nametable_tile(byte tile_id) {
	uint base_tile_addr = bit_test(rmem_b(PPUCTRL), PPUCTRL_S) ? 0x0000 : 0x1000;
	base_tile_addr += tile_id * 16;

	tile tile;
	encode_as_tiles(&vram_bank[base_tile_addr], 1, &tile);
	return tile;
}

/**
 * This "Magic numbers" are actually the memory addresses. They have to be somewhere and this place
 * looks as good as any other. We don't want #defines or global variables since we don't want to be
 * touching them from outside
 */
word get_nt_start_addr(NametableIndex nametableIndex) {
	switch (nametableIndex) {
		case NT_TOP_LEFT:
			return 0x2000;
		case NT_TOP_RIGHT:
			return (word) (get_ROM()->mirroring ? 0x2400 : 0x2000);
		case NT_BOTTOM_LEFT:
			return (word) (get_ROM()->mirroring ? 0x2000 : 0x2800);
		case NT_BOTTOM_RIGHT:
			return (word) (get_ROM()->mirroring ? 0x2400 : 0x2800);
	}
}

word get_at_start_addr(AttributeTableIndex attributeTableIndex) {
	switch (attributeTableIndex) {
		case AT_TOP_LEFT:
			return 0x23C0;
		case AT_TOP_RIGHT:
			return (word) (get_ROM()->mirroring ? 0x27C0 : 0x23C0);
		case AT_BOTTOM_LEFT:
			return (word) (get_ROM()->mirroring ? 0x23C00 : 0x2BC0);
		case AT_BOTTOM_RIGHT:
			return (word) (get_ROM()->mirroring ? 0x27C0 : 0x2FC0);
	}
}

byte get_attribute(int row_id, int column_id) {
	//Use this two booleans to create a number that will match the AttributeTableIndex enums
	int is_bottom = row_id / 240;//240 is the vertical half of the nametable
	int is_right = column_id / 256;//256  is the horizontal half of the nametable

	AttributeTableIndex index = (is_bottom << 1) + is_right;
	word addr = get_at_start_addr(index);

	// This is only for move in blocks of 32x32, we still need to find the actual attribute within that block
	addr += 8 * (row_id / 4); // move down
	addr += column_id / 4; // move right

	// We've reach the block.
	byte meta_tile = rmem_b_vram(addr);

	//In here, again look within the block if the tile is in the upper cells or right/left. Use the same principle
	//as above and once we know the cell we're in simply get two bits and generate the attribute
	if ((row_id % 2 == 0) && (column_id % 2 == 0)) { // top_left
		return (byte) ((bit_test(meta_tile, 1) << 1) + (bit_test(meta_tile, 0)));
	} else if ((row_id % 2 == 0) && (column_id % 2 == 1)) { // top_right
		return (byte) ((bit_test(meta_tile, 3) << 1) + (bit_test(meta_tile, 2)));
	} else if ((row_id % 2 == 1) && (column_id % 2 == 0)) { // bottom_left
		return (byte) ((bit_test(meta_tile, 5) << 1) + (bit_test(meta_tile, 4)));
	} else { // bottom_right
		return (byte) ((bit_test(meta_tile, 7) << 1) + (bit_test(meta_tile, 6)));
	}
}

colour *fill_palette_colours(word palette_addr){
	static colour palette[3];
	for (uint i = 0; i < 3; ++i) {
		palette[i] = COLOUR_PALETTE[rmem_b_vram(palette_addr + i)];
	}

	return palette;
}

colour *get_background_palette(byte attribute) {
	switch (attribute) {
		case 0:
			return fill_palette_colours(BACKGROUND_PALETTE_0);
		case 1:
			return fill_palette_colours(BACKGROUND_PALETTE_1);
		case 2:
			return fill_palette_colours(BACKGROUND_PALETTE_2);
		case 3:
			return fill_palette_colours(BACKGROUND_PALETTE_3);
		default:
			break;
	}
}
