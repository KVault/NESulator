#include "ppu_utils.h"
#include "log.h"
#include "math.h"


void encode_as_tiles(byte *mem_addr, byte number_tiles, struct tile *tiles) {
	for (int i, tile_count = i = 0; tile_count < number_tiles; i+=16, ++tile_count) { //Each tile is defined by 16 bytes
		log_debug("Tile: $%02i: \n", i/16);
		for (int j = 0; j < TILE_ROW_SIZE; ++j) { //And each byte defines a row of pixels with a value from 0 to 3
			for (byte k = 0; k < TILE_COLUMN_SIZE; ++k) {
				//Get the bit j from the current iteration and 8 positions ahead. Then add those two bits so that
				//you get one of the following: 00, 01, 10, 11.
				byte most_sig_bit = (byte)bit_test(mem_addr[j+ i], k) << 1;
				byte least_sig_bit = (byte)bit_test(mem_addr[j+ i + TILE_ROW_SIZE], k);

				//use the abs to flip the tile in the Y component. Otherwise it comes out wrong.
				tiles[tile_count].pattern[j][abs(k - TILE_COLUMN_SIZE)] = most_sig_bit + least_sig_bit;
				log_debug("%i",most_sig_bit + least_sig_bit);
			}
			log_debug("\n");
		}
	}
}
