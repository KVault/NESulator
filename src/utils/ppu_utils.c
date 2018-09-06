#include "ppu_utils.h"
#include "log.h"

struct tile *encode_as_tiles(byte *mem_addr, byte number_tiles) {
	byte tile_count = 0;
	//Allocate the memory needed to fit in the list of tiles.
	struct tile *tiles = malloc(number_tiles * sizeof(struct tile));

	for (int i = 0; tile_count < number_tiles; i+=16, ++tile_count) { //Each tile is defined by 16 bytes

		log_debug("Tile: $%02i: \n", i/16);
		for (int j = 0; j <  8; ++j) { //And each byte defines a row of pixels with a value from 0 to 3
			for (byte k = 0; k < 8; ++k) {
				//Get the bit j from the current iteration and 8 positions ahead. Then add those two bits so that
				//you get one of the following: 00, 01, 10, 11.
				byte most_sig_bit = (byte)bit_test(mem_addr[j+ i], k) << 1;
				byte least_sig_bit = (byte)bit_test(mem_addr[j+ i + 8], k);
				tiles[tile_count].pattern[j][k] = most_sig_bit + least_sig_bit;

				log_debug("%i",most_sig_bit + least_sig_bit);
			}
			log_debug("\n");
		}
	}

	return tiles;
}
