#include "nes.h"

int isRunning;

/**
 * Simply stops the emulation.
 */
int stop_emulation(SDL_Event e){
	isRunning = 0;
}

int main() {
	//LOGS
	set_console_log_level(ConsoleError);
	set_file_log_level(FileDebug);
	set_clear_log_file();
	set_log_path("../../logs/nesulator.log");

	power_up(0);
	isRunning = 1;
	//If we need to initialize anything, it should go here

	//Read the ROM, that we're going to execute and all that stuff
	struct ROM *rom = insertCartridge("../../rom/nestest.nes");
	loadROM(rom);
	resetPC();

	//SDL Load
	build_window();


	register_events();

	//Main loop. Keeps the emulator running forever more. In the future we'll be able to
	//control this with a debugger, or an UI. But for now, it simply runs forever
	while (isRunning) {
		cpu_cycle();
		ppu_cycle();
		apu_cycle();

		//SDL stuff. Not related with the actual emulator
		gui_cycle();

		//TODO At some point we would need to run the cpu and ppu independently. Different frequencies
	}

	//Cleans up our stuff
	ejectCartridge();
	cleanup_events();

	return 0;
}

void register_events(){
	sevent(SDL_QUIT, 0, &stop_emulation);
	sevent(SDL_QUIT, 0, &on_close_window);
	sevent(SDL_WINDOWEVENT, SDL_WINDOWEVENT_RESIZED, &on_window_resized_event);
}

void cleanup_events(){
	uevent(SDL_QUIT, 0, &stop_emulation);
	uevent(SDL_QUIT, 0, &on_close_window);
	uevent(SDL_WINDOWEVENT, SDL_WINDOWEVENT_RESIZED, &on_window_resized_event);
}