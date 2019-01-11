#ifndef NESULATOR_GUI_H
#define NESULATOR_GUI_H

#include "../utils/Utils.h"
#include "../ppu.h"

#define EXPORT __declspec(dllexport)

typedef struct {
	int size;
	byte *buffer;
} MemoryDumpInfo;

typedef struct {
	int size;
	uint *buffer;
	uint width;
	uint height;

} FrameInfo;

/**
 * @return CPU speed since the last second. In Hertzs
 */
EXPORT int last_cpu_speed();

/**
 * Uses the rom path to start the emulation.
 * This will run in a new thread managed by C, not the calling frontend
 * @param rom_path
 */
EXPORT void gui_start_emulation(char *rom_path);

/**
 * Stop the emulation
 */
EXPORT void gui_stop_emulation();

/**
 * @return The latest back buffer for the NES main window (The game)
 */
EXPORT FrameInfo gui_frame();

/**
 * @return the latest patterntable frame. It only returns a pointer to int. Both the UI and the backend know
 * the dimensions of the patterntable. Another option would be to return a struct or array that holds that info
 */
EXPORT FrameInfo gui_right_patterntable();

/**
 * @return the latest patterntable frame. It only returns a pointer to int. Both the UI and the backend know
 * the dimensions of the patterntable. Another option would be to return a struct or array that holds that info
 */
EXPORT FrameInfo gui_left_patterntable();

/**
 * @return the latest nametable frame for the given index (TL, TR, BL, BR). It returns it in the same manner as the
 * patterntable. That is with a FrameInfo that then the UI can make use of
 */
EXPORT FrameInfo gui_nametable(NametableIndex index);

/**
 * @return an array with the contents of the RAM at that point
 */
EXPORT MemoryDumpInfo gui_ram_dump();

/**
 * Performs an internal callback to set the CPU speed
 */
EXPORT void gui_cpu_speed(int speed_hertz);

/**
 * @return a buffer with the information of the current background palette
 */
EXPORT FrameInfo gui_palette_dump();

#endif //NESULATOR_GUI_H
