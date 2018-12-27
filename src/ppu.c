#include "ppu.h"


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
	//h-blank
	//TODO how long does it take to reset to 0? Take that time into account
	if (current_cycle_scanline > PPU_POINT_PER_SCANLINE) {
		current_cycle_scanline = 0;
		++current_scanline;
	}

	//v-blank
	if (current_scanline > PPU_VISIBLE_SCANLINES && !in_vblank) {
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
	//TODO Docs also say that nmi_output == TRUE? but if I do that, nothing comes up
	if (nmi_occurred == TRUE) {
		//triggering of a NMI can be prevented if bit 7 of PPU Control Register 1 ($2000) is clear.
		if(bit_test(rmem_b(PPUCTRL), 7) == 1){
			nmi();
		}

		nmi_occurred = FALSE;
	}
}