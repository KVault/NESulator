#ifndef NESULATOR_GUI_H
#define NESULATOR_GUI_H

#include <time.h>
#include <math.h>
#include "../../lib/SDL2/include/SDL.h"
#include "../utils/log.h"
#include "../utils/Utils.h"
#include "mainWindow.h"
#include "nametableViewer.h"
#include "patterntableViewer.h"

#define MILLISECOND 1000.0

/**
 * Function header for the callbacks. Every function has to match this header to be able to register itself
 * as a callback.
 */
typedef int (*sdl_event_func)(SDL_Event event);

/**
 * Function header for each window's cycle. This will be used on the main gui_cycle to call each window.
 * Each window should know what needs to be done on itself and moar stuff.
 */
typedef int (*gui_window_cycle)();

/**
 * This is a bit messy function, but it needs to happen somewhere. Basically search in the event_callbacks array
 * for the one (if any) that matches the raised event. if found then call the function within it.
 */
void process_event_callbacks(SDL_Event *pEvent);

/**
 * Handles any event, input or draw call from the gui. This "cycle" isn't dependant on the emulator
 * and it only shares its name for clarity.
 *
 * It also calls window_cycle on every window. they'll be the ones actually doing the processing for each window
 */
void gui_cycle();

/**
 * Subscribes to the given event with a function to be called when the event is raised
 * @return 1 if succeeded, something else if didn't
 */
int sevent(SDL_EventType event, uint event_id, sdl_event_func func);

/**
 * Unsubscribes from the given event
 * @return 1 if succeeded, something else if didn't
 */
void uevent(SDL_EventType event, uint event_id, sdl_event_func func);

/**
 * Subscribes a window to the main gui_cycle loop.
 * @return
 */
int register_window_cycle(gui_window_cycle cycle_func);

/**
 * Determines whether enough time has elapsed since the last call to this function so that the gui should refresh
 *
 * @return 0 if it hasn't, the timestamp (in milliseconds) if it has
 */
double has_time_elapsed(double last_check, double time);

/**
 * Init SDL and every window. It doesn't mean they'll be visible or active or doing something. That's down to
 * each individual window to decide.
 * @return SDL error code
 */
int gui_init();

Uint32 set_pixel(SDL_Window *window, byte *colour_palette_component);

#endif //NESULATOR_GUI_H
