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

EXPORT FrameInfo gui_nametable(NametableIndex index){
	return nametable(index);
}

EXPORT MemoryDumpInfo gui_ram_dump(){
	MemoryDumpInfo info = {};
	info.size = RAM_MEM_SIZE;
	info.buffer = ram_bank;

	return info;
}

EXPORT void gui_cpu_speed(int speed_hertz){
	cpu_set_speed(speed_hertz);
}