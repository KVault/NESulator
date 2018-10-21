#include "nes.h"

// So that MSVC is happy
#define HAVE_STRUCT_TIMESPEC
#include "pthread.h"

int is_running;
pthread_t nes;

void *run(void *args);

/**
 * Simply stops the emulation.
 */
int stop_emulation() {
	is_running = 0;
	ejectCartridge();
	return 0;
}

int start_emulation(){
	if(rom.prgROM == NULL){ // No ROM has been loaded. Error and stop the process
		log_error("No ROM has been loaded. Stopping the emulation. Abort, abort, abort!!");
		return 1;//Means error?
	}

	is_running = 1;
	reset_pc();
	pthread_create(&nes, NULL, &run, NULL);
	return 0;
}

void configure(){
	//LOGS
	set_console_log_level(ConsoleInfo);
	set_file_log_level(FileDebug);
	set_clear_log_file();
	set_log_path("../../logs/NESulator.log");

	cpu_power_up(1789773);
	ppu_power_up(3);//TODO this powerup has to wait about 20k cycles. PPU stuff
}

void *run(void *args) {
	is_running = 1;

	//Main loop. Keeps the emulator running forever more. In the future we'll be able to
	//control this with a debugger, or an UI. But for now, it simply runs forever
	byte num_cycle;
	while (is_running) {
		num_cycle = cpu_instruction();
		for (int i = 0; i < ppu_cycle_per_cpu_cycle * num_cycle; ++i) {
			ppu_cycle();
		}
		every_second();
	}

	pthread_join(nes, NULL);
	return 0;
}

void main(){
	printf("HEEE");
}

/**
 * Ticks every second. It's used to keep track of the clock speeds and whatnot
 */
void every_second() {
	static long last_second = 0;
	static long ctime = 0;
	ctime = time(NULL);

	//More than one second elapsed
	if (ctime - last_second > 1) {
		last_second = time(NULL);
		cpu_cyclesLastSec = cpu_cyclesThisSec;
		cpu_cyclesThisSec = 0;
	}
}
