#include "stdlib.h"
#include "ppu.h"

uint ppu_back_buffer[NES_PPU_TEXTURE_WIDTH * NES_PPU_TEXTURE_HEIGHT];

/**
 * Executes internal "housekeeping" operations such as frame increase, cycle increase and nmi trigger
 */
void tick();

/**
 * Executes the actual PPU logic for the current cycle.
 */
void step();

bool visible_scanline(){
	return current_scanline < PPU_VISIBLE_SCANLINES;
}

bool visible_cycle(){
	return current_cycle_scanline >= 1 && current_cycle_scanline <= 256;
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
	tick(); // PPU internal counters and frame update
	step(); // PPU actual logic and execution
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////////REGISTERS/////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void write_PPUADDR(byte value) {
	if (w == 0) {
		t = value << 8;
		w = 1;
	} else {
		t += value;
		v = t;
		w = 0;
	}
}

void write_PPUDATA(byte value) {
	wmem_b_vram(v, value);
	v += bit_test(rmem_b(PPUCTRL), 2) ? 32 : 1;
}

byte read_PPUDATA() {
	v += bit_test(rmem_b(PPUCTRL), 2) ? 32 : 1;
	return rmem_b_vram(v);
}

void write_PPUCTRL(byte value) {
	nmi_output = bit_test(value, 7);
}

byte read_PPUSTATUS(){
	//Need to access the bank directly here. Otherwise it'll be stuck in a recursive loop
	byte status = ram_bank[PPUSTATUS];
	bit_val(&status, 7, nmi_occurred);
	nmi_occurred = FALSE;
	wmem_b(PPUSTATUS, status);
	return status;
}

///////////////////////////////////////////////////////////////////////////
////////////////////////END OF REGISTERS///////////////////////////////////
///////////////////////////////////////////////////////////////////////////


void start_vblank() {
	in_vblank = TRUE;
	nmi_occurred = TRUE;
}

void finish_vblank() {
	in_vblank = FALSE;
	nmi_occurred = FALSE;
	current_scanline = 0;
}

void try_trigger_nmi() {
	if (nmi_occurred == TRUE && nmi_output == TRUE) {
		//triggering of a NMI can be prevented if bit 7 of PPU Control Register 1 ($2000) is clear.
		if(bit_test(rmem_b(PPUCTRL), 7) == 1){
			nmi();
		}

		nmi_occurred = FALSE;
	}
}

void render_pixel(){
	static uint colour;
	colour = 0xff000000 + rand() % (0xfffffffa+1 - 0xff000000 );
	ppu_back_buffer[NES_PPU_TEXTURE_HEIGHT * current_scanline + current_cycle_scanline] = colour;
}


void tick(){
	//h-blank
	//TODO how long does it take to reset to 0? Take that time into account
	if (current_cycle_scanline > PPU_POINT_PER_SCANLINE) {
		current_cycle_scanline = 0;
		++current_scanline;
	}

	//v-blank
	if (!visible_scanline() && !in_vblank) {
		start_vblank();
	}

	//Finish the v-blank!
	if (current_scanline > PPU_SCANLINES) {
		finish_vblank();
	}

	try_trigger_nmi();

	++current_cycle_scanline;
	++ppu_cycles_this_sec;
}

void step(){
	if(render_enabled || TRUE){
		if(visible_scanline() && visible_cycle()){
			//TODO Render the pixel
			render_pixel();
		}


	}

}