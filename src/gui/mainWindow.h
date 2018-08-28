#ifndef NESULATOR_MAINWINDOW_H
#define NESULATOR_MAINWINDOW_H

#include "../../lib/SDL2/include/SDL.h"
#include "../log.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240
#define WINDOW_TITLE "NESulator"

typedef int (*sdl_event_func)(SDL_Event event);

/**
 * Builds and creates the SDL window what holds the emulator
 * @return 1 if succeeded, something else if didn't
 */
int build_window();

/**
 * Close and cleanup of all the bits and pieces being used by SDL
 * @return 1 if succeeded, something else if didn't
 */
int close_window();

/**
 * Handles any event, input or draw call from the gui. This "cycle" isn't dependant on the emulator
 * and it only shares its name for clarity.
 */
void gui_cycle();

/**
 * Subscribes to the given event.
 * @return 1 if succeeded, something else if didn't
 */
int sevent(SDL_EventType event, sdl_event_func func);

/**
 * Unsubscribes from the given event
 * @return 1 if succeeded, something else if didn't
 */
void uevent(SDL_EventType event, sdl_event_func func);

#endif //NESULATOR_MAINWINDOW_H
