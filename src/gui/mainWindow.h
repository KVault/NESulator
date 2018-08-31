#ifndef NESULATOR_MAINWINDOW_H
#define NESULATOR_MAINWINDOW_H

#include <time.h>
#include "../../lib/SDL2/include/SDL.h"
#include "../log.h"
#include "../Utils.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240
#define WINDOW_TITLE "NESulator"

// TODO DELETE ME
int debug_speed;

/**
 * Function header for the callbacks. Every function has to match this header to be able to register itself
 * as a callback.
 */
typedef int (*sdl_event_func)(SDL_Event event);

/**
 * Builds and creates the SDL window what holds the emulator
 * @return 1 if succeeded, something else if didn't
 */
int build_window(int speed);

/**
 * Called when the user clicks on the X, red button or whatever depending on your OS. Basically que user
 * wants to quit the app, so it does it.
 */
int on_close_window(SDL_Event);

/**
 * Resize the background canvas to fill in the desired size of the window. It will also keep the NES's frame with
 * the desired aspect ratio and centered.
 *
 * This function WILL scale up or down the image
 */
int on_window_resized_event(SDL_Event);

/**
 * Handles any event, input or draw call from the gui. This "cycle" isn't dependant on the emulator
 * and it only shares its name for clarity.
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

#endif //NESULATOR_MAINWINDOW_H
