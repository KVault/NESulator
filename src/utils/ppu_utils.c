#include "ppu_utils.h"
#include "log.h"
#include "math.h"


void encode_as_tiles(byte *mem_addr, byte number_tiles, struct tile *tiles) {
	for (int i, tile_count = i = 0; tile_count < number_tiles; i+=16, ++tile_count) { //Each tile is defined by 16 bytes
		for (int j = 0; j < 8; ++j) { //And each byte defines a row of pixels with a value from 0 to 3
			for (byte k = 0; k < 8; ++k) {
				//Get the bit j from the current iteration and 8 positions ahead. Then add those two bits so that
				//you get one of the following: 00, 01, 10, 11.
				byte most_sig_bit = (byte)bit_test(mem_addr[j+ i], k) << 1;
				byte least_sig_bit = (byte)bit_test(mem_addr[j+ i + 8], k);
				tiles[tile_count].pattern[abs(j - 8)][abs(k - 8)] = most_sig_bit + least_sig_bit;
			}
		}
	}
}
