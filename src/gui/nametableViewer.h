#ifndef NESULATOR_NAMETABLEVIEWER_H
#define NESULATOR_NAMETABLEVIEWER_H

#include "gui.h"

#define NAMETABLE_WINDOW_WIDTH 512
#define NAMETABLE_WINDOW_HEIGHT 480
#define NAMETABLE_TEX_WIDTH 256
#define NAMETABLE_TEX_HEIGHT 240
#define NAMETABLE_ROWS_MAP 30
#define NAMETABLE_TILES_PER_ROW 32

#define NAMETABLE_WINDOW_TITLE "Nametable Viewer"

struct nametable_viewer_window{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *top_left;
	SDL_Rect top_left_rect;
	SDL_Texture *top_right;
	SDL_Rect top_right_rect;
	SDL_Texture *bottom_left;
	SDL_Rect bottom_left_rect;
	SDL_Texture *bottom_right;
	SDL_Rect bottom_right_rect;
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
 * Renders the nametable map onto the back buffer
 */
void render_nametable_map(NametableIndex nametableIndex);

int cycle_nametable_viewer();

#endif //NESULATOR_NAMETABLEVIEWER_H
