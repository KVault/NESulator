#include "nes.h"

int is_running;

static PyObject *run(PyObject *self, PyObject *args) {
	//LOGS
	set_console_log_level(ConsoleInfo);
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

	return 0;
}

void every_second() {
	static long last_second = 0;
	static long ctime = 0;
	ctime = time(NULL);

	//More than one second elapsed
	if (ctime - last_second > 1) {
		last_second = time(NULL);
		log_info("Processor speed: %iHz\n", cpu_cyclesThisSec);
		cpu_cyclesThisSec = 0;
	}


}
