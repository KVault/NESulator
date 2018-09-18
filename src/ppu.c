#include "ppu.h"

void ppu_cycle() {

}

void ppu_power_up(int clock_speed) {
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

void write_PPUADDR(byte value) {
	if (w == 0) {
		t = value << 8;
		w = 1;
	} else {
		t += value;
		w = 0;
	}
}

void write_PPUDATA(byte value) {
	wmem_b_vram(t, value);
	t += bit_test(rmem_b(PPUCTRL), 3) ? 32 : 1;
}