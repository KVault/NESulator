#ifndef NESULATOR_NAMETABLEVIEWER_H
#define NESULATOR_NAMETABLEVIEWER_H

#include "gui.h"
#include "../memory.h"

#define NAMETABLE_WINDOW_WIDTH 511
#define NAMETABLE_WINDOW_HEIGHT 479
#define NAMETABLE_WINDOW_TITLE "Nametable Viewer"

struct nametable_viewer_window{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *back_buffer_tex;
	double refresh_time;//In ms
};

/**
 * Does what it says it does :P
 */
int build_nametable_viewer(int refresh_rate);

/**
 * Hides the window, doesn't quit it (at least just yet)
 */
int on_quit_nametable_viewer_window(SDL_Event);

/**
 * Renders the nametable sprites onto the back buffer
 */
void render_nametable();

int cycle_nametable_viewer();

#endif //NESULATOR_NAMETABLEVIEWER_H
