#include "nes.h"

int isRunning;

/**
 * Simply stops the emulation.
 */
void stopEmulation() {
	isRunning = 0;
}

int main() {
	set_log_level(Debug);
	set_clear_log_file();
	set_log_path("../../log.txt");
	power_up(0);
	isRunning = 1;
	//If we need to initialize anything, it should go here

	//Read the ROM, that we're going to execute and all that stuff
	struct ROM *rom = insertCartridge("../../rom/cpu_dummy_reads/cpu_dummy_reads.nes");
	loadROM(rom);
	resetPC();

	//Main loop. Keeps the emulator running forever more. In the future we'll be able to
	//control this with a debugger, or an UI. But for now, it simply runs forever
	while (isRunning) {
		cpu_cycle();
		ppu_cycle();
		apu_cycle();

		//TODO At some point we would need to run the cpu and ppu independently. Different frequencies
	}

	//Cleans up our stuff
	ejectCartridge();

	return 0;
}