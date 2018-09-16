#ifndef NESULATOR_NAMETABLEVIEWER_H
#define NESULATOR_NAMETABLEVIEWER_H

#include "gui.h"
#include "../memory.h"
#include "../utils/ppu_utils.h"

#define NAMETABLE_WINDOW_WIDTH 256
#define NAMETABLE_WINDOW_HEIGHT 240
#define NAMETABLE_TEX_WIDTH 256
#define NAMETABLE_TEX_HEIGHT 240
#define NAMETABLE_ROWS_MAP 30
#define NAMETABLE_TILES_PER_ROW 32

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
void render_nametable_map(word start_addr);

void render_tile(struct tile *tile, uint row_id, uint column_id);

int cycle_nametable_viewer();

#endif //NESULATOR_NAMETABLEVIEWER_H
