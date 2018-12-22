#include "nes.h"

int is_running;

void run();

void match_cpu_speed();

unsigned long get_epoch_millisecond();

/**
 * Simply stops the emulation.
 */
int stop_emulation() {
	is_running = 0;
	ejectCartridge();
	return 0;
}

int start_emulation(){
	configure();
	if(rom.prgROM == NULL){ // No ROM has been loaded. Error and stop the process
		log_error("No ROM has been loaded. Stopping the emulation. Abort, abort, abort!!");
		return 1;//Means error?
	}

	is_running = 1;
	reset_pc();
	run();
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

	//If a ROM was already loaded, do it again, powering up the CPU zeroes the memory
	if(get_ROM() != NULL){
		load_ROM(get_ROM());
	}
}

void run() {
	is_running = 1;

	//Main loop. Keeps the emulator running forever more. In the future we'll be able to
	//control this with a debugger, or an UI. But for now, it simply runs forever
	byte num_cycle;
	while (is_running) {
		num_cycle =cpu_instruction();
		match_cpu_speed();
		for (int i = 0; i < ppu_cycle_per_cpu_cycle * num_cycle; ++i) {
			ppu_cycle();
		}
	}
}

/**
 * Ran every tick. for the current cycle this second it looks how much the original NES should have spent and
 * how much the host CPU has. Once the host CPU is a bit ahead of the original CPU it'll sleep the CPU until
 * both match.
 */
void match_cpu_speed(){
	//Long value so that the first run through enters the "every second" condition
	static unsigned long ctime, last_second = 4294967295;
	static long should_elapsed, has_elapsed;

	ctime = get_epoch_millisecond();

	//More than one second has passed, so update the CPU speed. if less than 0 means a timer hasn't been initialized.
	//This will do it
	if(ctime - last_second > MILLISECOND){
		cpu_cyclesLastSec = cpu_cyclesThisSec;
		cpu_cyclesThisSec = 0;
		last_second = get_epoch_millisecond();
	}

	//TODO still working on this feature
	/**
	should_elapsed = (cpu_cyclesThisSec * nanoseconds_cpu_cycle) / 1000; //convert to microseconds
	has_elapsed = ctime - last_second;

	if(should_elapsed > has_elapsed){
		usleep((useconds_t) 1);
	}
	 **/
}

unsigned long get_epoch_millisecond(){
	static struct timeval tp;
	mingw_gettimeofday(&tp, NULL);
	return (unsigned long) (tp.tv_sec * MILLISECOND + tp.tv_usec / MILLISECOND);
}
