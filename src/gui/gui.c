#include "gui.h"
#include "gui_patterntable.h"
#include "gui_nametable.h"

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

/**
 * Iterates through the VRAM, where the colour palette information is stored and fills in a FrameInfo struct
 * with the RGBA colour of each position. The UI will take care of creating the images and fill them in with the colour.
 */
EXPORT FrameInfo gui_palette_dump(){
	FrameInfo info = {};
	static uint palette_buffer[32];
	info.size = 32;// Two rows of 16. Basically from 0x3F01 to 0x3F1F
	info.buffer = palette_buffer;
	info.width = 16;
	info.height = 16;

	for(uint i = 0; i < info.size; ++i){
		int colour_index = rmem_b_vram(i + UNIVERSAL_BACKGROUND);
		palette_buffer[i] = encode_as_RGBA(COLOUR_PALETTE[colour_index]);
	}

	return info;
}