#include "ppu.h"

//TODO this has some dupplicated code. Can we fix this?
void *ppu_run(void *arg){
	ppu_running = 1;
	static struct timespec this_second;
	static struct timespec this_cycle;
	long nano_per_cycle = (long)((1.0 / (cpu_speed * ppu_points_per_cpu_cycle)) * NANOSECOND);

	long should_have_elapsed, have_elapsed, elapsed_delta = 0;
	clock_gettime(CLOCK_REALTIME, &this_second);

	while(cpu_running){
		//Check if enough time has passed, and sleep the process otherwise
		clock_gettime(CLOCK_REALTIME, &this_cycle);

		//New second. Reset stuff
		if(this_cycle.tv_sec > this_second.tv_sec){
			this_second = this_cycle;
			log_info("PPU cycles last second (%i): %d\n", this_cycle.tv_sec, ppu_cycles_this_sec);
			ppu_cycles_this_sec = 0;
		}

		should_have_elapsed = nano_per_cycle * cpu_cyclesThisSec;
		have_elapsed = this_cycle.tv_nsec - this_second.tv_nsec;
		elapsed_delta = should_have_elapsed - have_elapsed;
		if(elapsed_delta > 0){
			this_cycle.tv_nsec = elapsed_delta;
			this_cycle.tv_sec = 0;
			nanosleep(&this_cycle, NULL);
		}

		ppu_cycle();
	}
}

void ppu_cycle() {

	//h-blank
	if(current_cycle_scanline > PPU_POINT_PER_SCANLINE){
		current_cycle_scanline = 0;
		++current_scanline;
	}

	//v-blank
	if(current_scanline > PPU_SCANLINES){
		start_vblank();
	}

	++current_cycle_scanline;
	++ppu_cycles_this_sec;
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
		v = t;
		w = 0;
	}
}

void write_PPUDATA(byte value) {
	wmem_b_vram(v, value);
	v += bit_test(rmem_b(PPUCTRL), 2) ? 32 : 1;
}

byte read_PPUDATA(){
	v += bit_test(rmem_b(PPUCTRL), 2) ? 32 : 1;
	return rmem_b_vram(v);
}

void start_vblank(){
	current_scanline = 0;
	nmi();
}