#ifndef NESULATOR_GUI_H
#define NESULATOR_GUI_H

#define EXPORT __declspec(dllexport)

#include "../cpu.h"
#include "../rom.h"
#include "gui_patterntable.h"
#include "gui_nametable.h"

typedef struct {
	int size;
	byte *buffer;
} MemoryDumpInfo;

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
 * Returns the latest patterntable frame. It only returns a pointer to int. Both the UI and the backend know
 * the dimensions of the patterntable. Another option would be to return a struct or array that holds that info
 */
EXPORT FrameInfo gui_right_patterntable();

/**
 * Returns the latest patterntable frame. It only returns a pointer to int. Both the UI and the backend know
 * the dimensions of the patterntable. Another option would be to return a struct or array that holds that info
 */
EXPORT FrameInfo gui_left_patterntable();

/**
 * Returns the latest nametable frame for the given index (TL, TR, BL, BR). It returns it in the same manner as the
 * patterntable. That is with a FrameInfo that then the UI can make use of
 */
EXPORT FrameInfo gui_nametable(NametableIndex index);

/**
 * @return an array with the contents of the RAM at that point
 */
EXPORT MemoryDumpInfo gui_ram_dump();

#endif //NESULATOR_GUI_H
