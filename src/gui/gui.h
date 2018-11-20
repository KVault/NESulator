#ifndef NESULATOR_GUI_H
#define NESULATOR_GUI_H

#define EXPORT __declspec(dllexport)

#include "../cpu.h"
#include "../rom.h"
#include "gui_patterntable.h"

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
 *
 * TODO Maybe change that? I just want to get this working right now
 */
EXPORT int *gui_patterntable_frame();

#endif //NESULATOR_GUI_H
