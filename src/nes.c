#include "nes.h"

int isRunning;

/**
 * Simply stops the emulation.
 */
void stopEmulation() {
	isRunning = 0;
}

int main() {
	isRunning = 1;
	//If we need to initialize anything, it should go here

	//Read the ROM, that we're going to execute and all that stuff
	loadROM('a');

	//Main loop. Keeps the emulator running forever more. In the future we'll be able to
	//control this with a debugger, or an UI. But for now, it simply runs forever
	while(isRunning){
		cpu_cycle();
		ppu_cycle();
		apu_cycle();

		//TODO At some point we would need to run the cpu and ppu independently. Different frequencies
	}
	return 0;
}