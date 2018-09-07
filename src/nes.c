#include "nes.h"

int is_running;

/**
 * Simply stops the emulation.
 */
int stop_emulation(SDL_Event e) {
	is_running = 0;
}

int main() {
	//LOGS
	set_console_log_level(ConsoleInfo);
	set_file_log_level(FileDisabled);
	set_clear_log_file();
	set_log_path("../../logs/donkey_kong.log");

	cpu_power_up(0);
	ppu_power_up(0);//TODO this powerup has to wait about 20k cycles. PPU stuff
	is_running = 1;
	//If we need to initialize anything, it should go here

	//Read the ROM, that we're going to execute and all that stuff
	struct ROM *rom = insertCartridge("../../rom/donkey_kong.nes");
	load_ROM(rom);
	reset_pc();

	//Start the gui, SDL and all the stuff
	gui_init();

	//Subscribe to all the events that we care about.
	register_events();

	//Main loop. Keeps the emulator running forever more. In the future we'll be able to
	//control this with a debugger, or an UI. But for now, it simply runs forever
	while (is_running) {
		cpu_cycle();
		ppu_cycle();
		apu_cycle();

		//SDL stuff. Not related with the actual emulator
		gui_cycle();

		//TODO At some point we would need to run the cpu and ppu independently. Different frequencies
	}

	//Cleans up our stuff.
	ejectCartridge();

	return 0;
}

void register_events() {
	sevent(SDL_QUIT, SDL_QUIT, &stop_emulation);
}