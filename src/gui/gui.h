#ifndef NESULATOR_GUI_H
#define NESULATOR_GUI_H

#define EXPORT __declspec(dllexport)

#include "../cpu.h"
#include "../rom.h"

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

#endif //NESULATOR_GUI_H
