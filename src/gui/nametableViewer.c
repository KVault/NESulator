#include "nametableViewer.h"
#include "patterntableViewer.h"


struct nametable_viewer_window nametable_window;

/**
 * Cached variable that holds the refresh time in milliseconds. That is 1/refresh_rate
 */
static double refresh_time;

int build_nametable_viewer(int refresh_rate) {
	refresh_time = (1.0/ refresh_rate) * MILLISECOND;
	nametable_window.refresh_rate = refresh_rate;
	nametable_window.window = SDL_CreateWindow(NAMETABLE_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, NAMETABLE_WINDOW_WIDTH, NAMETABLE_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

	if(nametable_window.window == NULL){
		log_error("Nametable Window could not be created: %s", SDL_GetError());
		return 0;
	}

	nametable_window.renderer = SDL_CreateRenderer(nametable_window.window, -1, SDL_RENDERER_ACCELERATED);

	nametable_window.back_buffer_tex = SDL_CreateTexture(nametable_window.renderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STATIC, NAMETABLE_WINDOW_WIDTH, NAMETABLE_WINDOW_HEIGHT);

	register_window_cycle(&cycle_nametable_viewer);
	sevent(SDL_WINDOWEVENT, SDL_WINDOWEVENT_CLOSE, &on_quit_nametable_viewer_window);
	return 0;
}

int on_quit_nametable_viewer_window(SDL_Event e) {
	if(e.window.windowID == SDL_GetWindowID(nametable_window.window)){
		SDL_HideWindow(nametable_window.window);
		return 1;
	}
	return 0;
}


int cycle_nametable_viewer() {
	static double last_check = 0;
	double time_aux = 0;
	if((time_aux = has_time_elapsed(last_check, refresh_time))){
		last_check = time_aux;

		SDL_UpdateTexture(nametable_window.back_buffer_tex, NULL, &vram_bank[0], NAMETABLE_WINDOW_WIDTH);
		SDL_RenderClear(nametable_window.renderer);
		SDL_RenderCopy(nametable_window.renderer, nametable_window.back_buffer_tex, NULL, NULL);
		SDL_RenderPresent(nametable_window.renderer);

		//Update the surfaces
		SDL_UpdateWindowSurface(nametable_window.window);
	}
	return 0;
}

