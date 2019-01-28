#include <stdlib.h>
#include <stdio.h>
#include "ppu.h"
#include "cpu.h"
#include "rom.h"
#include "memory.h"

uint ppu_back_buffer[NES_PPU_TEXTURE_WIDTH * NES_PPU_TEXTURE_HEIGHT];
uint current_bg_data[TILE_WIDTH * TILE_HEIGHT];

word BACKGROUND_PALETTES[4] = {0x3F01, 0x3F05, 0x3F09, 0x3F0D};
word UNIVERSAL_BACKGROUND = 0x3F00;

void encode_as_tiles(byte *mem_addr, uint number_tiles, tile *tiles) {
	//Each tile is defined by 16 bytes
	for (int i, tile_count = i = 0; tile_count < number_tiles; i += 16, ++tile_count) {
		for (int j = 0; j < TILE_WIDTH; ++j) { //And each byte defines a row of pixels with a value from 0 to 3
			for (byte k = 0; k < TILE_HEIGHT; ++k) {
				//Get the bit j from the current iteration and 8 positions ahead. Then add those two bits so that
				//you get one of the following: 00, 01, 10, 11.
				byte least_sig_bit = (byte) bit_test(mem_addr[j + i], k);
				byte most_sig_bit = (byte) bit_test(mem_addr[j + i + TILE_WIDTH], k) << 1;
				//use the abs to flip the tile in the Y component. Otherwise it comes out wrong.
				//The +1 is because otherwise the tiles wraparound itself. Feel free to change it and brake it :D
				tiles[tile_count].pattern[j][abs(k - TILE_HEIGHT + 1)] = most_sig_bit + least_sig_bit;
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
			return (word) (get_ROM()->mirroring ? 0x23C0 : 0x2BC0);
		case AT_BOTTOM_RIGHT:
			return (word) (get_ROM()->mirroring ? 0x27C0 : 0x2FC0);
	}
}

byte get_attribute(NametableIndex nametableIndex, int row_id, int column_id) {
	//We can use the same index, they have the same value
	word addr = get_at_start_addr((AttributeTableIndex)nametableIndex);

	// This is only for move in blocks of 32x32, we still need to find the actual attribute within that block
	addr += 8 * (row_id / 4); // move "down" that is move X times 8 bits in the array
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

/**
 * With the attribute, get the correct palette memory address (for the background) and fill in an array
 * with the colours
 */
colour *get_background_palette(byte attribute) {
	static colour palette[4];
	word palette_addr = (word)(BACKGROUND_PALETTES[attribute] -1); //-1 to include the universal background

	for (uint i = 0; i < 4; ++i) {
		palette[i] = COLOUR_PALETTE[rmem_b_vram(palette_addr + i)];
	}
	return palette;
}

void ppu_power_up(int clock_speed) {
	ppu_cycle_per_cpu_cycle = clock_speed;
	wmem_b(PPUCTRL, 0);
	wmem_b(PPUMASK, 0);
	wmem_b(PPUSTATUS, 0xA0);
	wmem_b(OAMADDR, 0);
	wmem_b(PPUSCROLL, 0);
	wmem_b(PPUADDR, 0);
	wmem_b(PPUDATA, 0);
	wmem_b(OAMDATA, 0); // Undefined default value
	wmem_b(OAMDMA, 0); // Undefined default value
}

void ppu_cycle() {
	switch (current_scanline){
		case 0 ... 239:	step(Visible);	break;
		case 240: step(Post); break;
		case 241: step(NMI); break;
		case 261: step(Pre); break;
		default:break;
	}

	// Update dot and scanline counters:
	if (++current_cycle_scanline > 340)
	{
		current_cycle_scanline %= 341;
		if (++current_scanline> 261)
		{
			current_scanline= 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////////REGISTERS/////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void write_PPUADDR(byte value) {
	if (!w) {
		/**
         * t: .FEDCBA ........ = d: ..FEDCBA
	 	 * t: X...... ........ = 0
         * w:                  = 1
	 	*/
	 	word val = (word) ((value & 0b00111111) << 8);// extract FEDCBA
		t_vram = (word) ((t_vram & ~0b011111100000000) | (val & 0b011111100000000));
		//t_vram &= 0b0111111111111111;// for the X
		w = true;
	} else {
		/**
		 * t: ....... HGFEDCBA = d: HGFEDCBA
		 * v                   = t
		 * w:                  = 0
		 */
		t_vram = (word) ((t_vram & ~0b0000000011111111) | (value));
		c_vram = t_vram;
		w = false;
	}
}

void write_PPUDATA(byte value) {
	wmem_b_vram(c_vram, value);
	c_vram += bit_test(rmem_b(PPUCTRL), 2) ? 32 : 1;
}

byte read_PPUDATA() {
	c_vram += bit_test(rmem_b(PPUCTRL), 2) ? 32 : 1;
	return rmem_b_vram(c_vram);
}

void write_PPUCTRL(byte value) {
	nmi_output = bit_test(value, 7);
	/** from here:
	 * https://stackoverflow.com/questions/4439078/how-do-you-set-only-certain-bits-of-a-byte-in-c-without-affecting-the-rest
	 * value = (value & ~mask) | (newvalue & mask);
	 */
	word t_val = (word) (value & 0x0003) << 8;
	t_vram = (word) ((t_val & ~0b0000110000000000) | (t_val & 0b0000110000000000));
}

/**
 * 2005 first write:
 * t: ....... ...HGFED = d: HGFED...
 * x:              CBA = d: .....CBA
 * w:                  = 1
 *
 * 2005 second write:
 *      t: CBA..HG FED..... = d: HGFEDCBA
 *      w:                  = 0
 */
void write_PPUSCROLL(byte d){
	static word val;
	if(!w){
		val = (d >> 3) << 8; // extract HGFED... //TODO is this right?
		t_vram = (word) ((t_vram & ~0x001F) | (val & 0x0001F));
		x = (byte) (d & 0x07);
		w = true;
	}else{
		//We have to do it in two steps. One for CBA and the other one for HGFED
		w = false;
		val = (word) (d & 0x7) << 8;// extract CBA //TODO is this right?
		t_vram = (word) ((t_vram & ~0x7000) | (val & 0x7000));// CBA

		val = d >> 3;// extract HGFED
		t_vram = (word) ((t_vram & 0x001F) | (val & 0x0001F));// HGFED
	}
}

byte read_PPUSTATUS(){
	//Need to access the bank directly here. Otherwise it'll be stuck in a recursive loop
	byte status = ram_bank[PPUSTATUS];
	bit_val(&status, 7, nmi_occurred);
	nmi_occurred = false;
	wmem_b(PPUSTATUS, status);
	w = 0; //Reading from STATUS resets the (shared) latch w
	return status;
}

///////////////////////////////////////////////////////////////////////////
////////////////////////END OF REGISTERS///////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void try_trigger_nmi() {
	if (nmi_occurred == true ){//&& nmi_output == true) {
		//triggering of a NMI can be prevented if bit 7 of PPU Control Register 1 ($2000) is clear.
		if(bit_test(rmem_b(PPUCTRL), 7) == 1){
			nmi();
			nmi_occurred = true;
		}
	}else{
		nmi_occurred = false;
	}
}

void fetch_nt_byte(){
	// Because NT addresses only go 0x2000 - 0x2FFF. So this is a "wraparound"
	uint addr = (uint) (0x2000 | (c_vram  & 0x0FFF));
	nt_byte = rmem_b_vram(addr);
}

void fetch_at_byte(){
	//I don't understand this. Why?
	//nvm. It comes from the wiki so it's probably ok
	uint addr = (uint) (0x23C0 | (c_vram & 0xC00) | ((c_vram & 0x380) >> 4) | ((c_vram & 0x1C) >> 2));
	at_byte = rmem_b_vram(addr);
}

void fetch_bg_tile(bg_byte high_low){
	int patterntable = bit_test(rmem_b(PPUCTRL), PPUCTRL_B); //0x0000 or 0x1000
	uint addr = (uint) ((0x1000 * patterntable) + (nt_byte * 16));// * 16 because each tile is 16 bytes long

	if(high_low == high){
		high_bg_byte = (byte) ((addr >> 8) & 0xff);
	}else{
		low_bg_byte = (byte) (addr & 0xff);
	}
}

void store_tile_data(){
	static tile tile;
	uint bg_addr = (high_bg_byte << 8 ) + low_bg_byte;
	encode_as_tiles(&vram_bank[bg_addr], 1, &tile);
	colour *palette = get_background_palette(at_byte);

	for (int i = 0; i < TILE_WIDTH; ++i) {
		for (int j = 0; j < TILE_HEIGHT; ++j) {
			static colour draw_colour;
			draw_colour = palette[tile.pattern[i][j]];
			current_bg_data[TILE_WIDTH * i + j] = encode_as_RGBA(draw_colour);
		}
	}
}

/**
 * So here's the deal. We've got a array representing the current background data, I know it's 8x8 and it doesn't need
 * to be since it only represents a scanline....  but hey, ship it :D
 *
 * We get the current scanline and the current cycle within the scanline %8 and that gives us the local coordinate
 * to display in the screen which is given by the current scanline and the cycle within the scanline
 */
void render_pixel(){
	static uint colour;
	if(current_scanline < 240) {
		colour = current_bg_data[TILE_WIDTH * (current_scanline % TILE_WIDTH) + current_cycle_scanline % TILE_WIDTH];
		ppu_back_buffer[NES_PPU_TEXTURE_WIDTH * current_scanline + current_cycle_scanline] = colour;
	}
}

void increment_horizontally(){
	//Got to the end of the nametable
	if((c_vram & 0x001F) == 31){
		// coarse X = 0
		c_vram &= 0xFFE0;
		// switch horizontal nametable
		c_vram ^= 0x0400;
	}else{
		++c_vram;
	}
}

void increment_vertically(){
	// increment vert(c_vram)
	// if fine Y < 7
	if(c_vram & 0x7000 != 0x7000) {
		// increment fine Y
		c_vram += 0x1000;
	} else {
		// fine Y = 0
		c_vram &= 0x8FFF;
		// let y = coarse Y
		int y = (c_vram & 0x03E0) >> 5;
		if (y == 29 ){
			// coarse Y = 0
			y = 0;
			// switch vertical nametable
			c_vram ^= 0x0800;
		} else if (y == 31) {
			// coarse Y = 0, nametable not switched
			y = 0;
		} else {
			// increment coarse Y
			y++;
		}
		// put coarse Y back into c_vram
		c_vram = (word) ((c_vram & 0xFC1F) | (y << 5));
	}
}


void step(scanline_type s_type){
	/** For the Background */

	if(s_type == NMI && !in_vblank){
		nmi_occurred = in_vblank = true;
		try_trigger_nmi();
	}

	if(s_type == Visible || s_type == Pre) {
		switch (current_cycle_scanline) {
			case 1:
				if (s_type == Pre && in_vblank)
					nmi_occurred = in_vblank = false;
				break;
			case 2 ... 255:
			case 322 ...337:
				render_pixel();
				//The pattern for the background repeats every 8 cycles. So doing the mod will work just fine :D
				switch (current_cycle_scanline % 8) {
					case 1: // NT byte
						fetch_nt_byte();
						break;
					case 3: // AT byte
						fetch_at_byte();
						break;
					case 5: // Low BG tile byte
						fetch_bg_tile(low);
						break;
					case 7: // High BG tile byte
						fetch_bg_tile(high);
						break;
					case 0: // Store tile data
						store_tile_data();
						break;
					default:
						break;
				}
				break;

			case 256:
				render_pixel();
				increment_vertically();
				//Update vertical
				break;

			case 257:
				render_pixel();
				//Increment horizontal position
				break;
			case 280 ... 304:
				//increment vertical position
				break;

			default:
				break;
		}
	}
		/**
		if(current_cycle_scanline > 280 && current_cycle_scanline <= 304){
			//v: IHGF.ED CBA..... = t: IHGF.ED CBA.....
			word val = (word) (t_vram & 0b111101111100000); // extract IHGF.ED CBA.....
			c_vram = (word) ((c_vram & ~0b111101111100000) | (val & 0b111101111100000));
		}

		//If rendering is enabled, the PPU copies all bits related to horizontal position from t to v:
		//v: ....F.. ...EDCBA = t: ....F.. ...EDCBA
		if(current_cycle_scanline == 257){
			word val = (word) (t_vram & 0b000010000011111); // extract ....F.. ...EDCBA
			c_vram = (word) ((c_vram & ~0x041F) | (val & 0x041F));
		}
		*/
		/** For the Sprites */
}