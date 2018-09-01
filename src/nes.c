#include <time.h>
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
	set_file_log_level(FileDebug);
	set_clear_log_file();
	set_log_path("../../logs/nesulator.log");

	cpu_power_up(0);
	is_running = 1;
	//If we need to initialize anything, it should go here

	//Read the ROM, that we're going to execute and all that stuff
	struct ROM *rom = insertCartridge("../../rom/donkey_kong.nes");
	load_ROM(rom);
	reset_pc();

	//SDL Load
	build_window(60);

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

		//TODO change this to accept every how many miliseconds to run. For now, this will do
		every_second();

		//TODO At some point we would need to run the cpu and ppu independently. Different frequencies
	}

	//Cleans up our stuff.
	ejectCartridge();

	return 0;
}

void every_second() {
	static long last_second = 0;
	static long ctime = 0;
	ctime = time(NULL);

	//More than one second elapsed
	if (ctime - last_second > 1) {
		last_second = time(NULL);
		log_info("Processor speed: %iHz\n", cyclesThisSec);
		cyclesThisSec = 0;
		log_info("GUI FPS: %i\n", gui_fps);
		gui_fps = 0;
	}


}

void register_events() {
	sevent(SDL_QUIT, SDL_QUIT, &stop_emulation);
	sevent(SDL_QUIT, SDL_QUIT, &on_close_window);
	sevent(SDL_WINDOWEVENT, SDL_WINDOWEVENT_RESIZED, &on_window_resized_event);
}