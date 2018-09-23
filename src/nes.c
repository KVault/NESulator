#include "nes.h"

int is_running;
pthread_t cpu_thread;
pthread_t ppu_tread;

/**
 * Simply stops the emulation.
 */
int stop_emulation(SDL_Event e) {
	is_running = 0;
	cpu_running = 0;
	ppu_running = 0;
	//pthread_exit(&cpu_thread);
	//pthread_exit(&ppu_tread);
	ejectCartridge();
}

int main() {
	//LOGS
	set_console_log_level(ConsoleError);
	set_file_log_level(FileDebug);
	set_clear_log_file();
	set_log_path("../../logs/donkey_kong.log");

	cpu_power_up(1789773);
	ppu_power_up(3);//TODO this powerup has to wait about 20k cycles. PPU stuff
	is_running = 1;

	//Read the ROM, that we're going to execute and all that stuff
	ROM *rom = insertCartridge("../../rom/donkey_kong.nes");
	load_ROM(rom);
	reset_pc();

	//Start the gui, SDL and all the stuff
	gui_init();

	//Subscribe to all the events that we care about.
	register_events();

	//pthread_create(&cpu_thread, NULL, cpu_run, (void *)&cpu_thread);
	//pthread_create(&ppu_tread, NULL, ppu_run, (void *)&ppu_tread);

	//Main loop. Keeps the emulator running forever more. In the future we'll be able to
	//control this with a debugger, or an UI. But for now, it simply runs forever
	while (is_running) {
		//SDL stuff. Not related with the actual emulator
		cpu_cycle();
		gui_cycle();
		ppu_cycle();
	}


	return 0;
}

void register_events() {
	sevent(SDL_QUIT, SDL_QUIT, &stop_emulation);
}