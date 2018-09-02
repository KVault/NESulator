#ifndef NESULATOR_MAINWINDOW_H
#define NESULATOR_MAINWINDOW_H

#include "gui.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240
#define WINDOW_TITLE "NESulator"

struct main_window{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *back_buffer_tex;
};

/**
 * Used to keep track of the FPS
 */
int gui_fps;

/**
 * Builds and creates the SDL window what holds the emulator
 * @return 1 if succeeded, something else if didn't
 */
int build_main_window(int speed);

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
 * Called every gui cycle
 */
int cycle_main_window();

#endif //NESULATOR_MAINWINDOW_H
