#include "gui.h"

EXPORT int last_cpu_speed() {
	return cpu_cyclesLastSec;
}

EXPORT void gui_start_emulation(char *rom_path) {
	insertCartridge(rom_path);
	load_ROM(get_ROM());
	start_emulation();
}

EXPORT void gui_stop_emulation() {
	stop_emulation();
}

EXPORT FrameInfo gui_left_patterntable(){
	return left_patterntable();
}

EXPORT FrameInfo gui_right_patterntable(){
	return right_patterntable();
}