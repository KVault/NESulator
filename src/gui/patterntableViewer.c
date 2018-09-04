#include "patterntableViewer.h"

struct patterntable_viewer_window patterntable_window;

double refresh_time;


int build_patterntable_viewer(int refresh_rate) {
	refresh_time = (1.0 / refresh_rate) * MILLISECOND;
	patterntable_window.refresh_rate = refresh_rate;
	patterntable_window.window = SDL_CreateWindow(PATTERNTABLE_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
	                                              SDL_WINDOWPOS_UNDEFINED, PATTERNTABLE_WINDOW_WIDTH,
	                                              PATTERNTABLE_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

	if (patterntable_window.window == NULL) {
		log_error("Patterntable window could not be created: %s\n", SDL_GetError());
	}

	patterntable_window.renderer = SDL_CreateRenderer(patterntable_window.window, -1, SDL_RENDERER_ACCELERATED);

	patterntable_window.back_buffer_text = SDL_CreateTexture(patterntable_window.renderer, SDL_PIXELFORMAT_ABGR8888,
	                                                         SDL_TEXTUREACCESS_STATIC, PATTERNTABLE_WINDOW_WIDTH,
	                                                         PATTERNTABLE_WINDOW_HEIGHT);

	register_window_cycle(&cycle_patterntable_viewer);

	sevent(SDL_WINDOWEVENT, SDL_WINDOWEVENT_CLOSE, &on_quit_patterntable_viewer_window);
}

int on_quit_patterntable_viewer_window(SDL_Event event) {
	if (event.window.windowID == SDL_GetWindowID(patterntable_window.window)) {
		SDL_HideWindow(patterntable_window.window);
		return 1;
	}
	return 0;
}

int cycle_patterntable_viewer() {
	static double last_check;
	double time_aux = 0;
	if((time_aux = has_time_elapsed(last_check, refresh_time))) {
		last_check = time_aux;
		SDL_RenderClear(patterntable_window.renderer);
		SDL_RenderPresent(patterntable_window.renderer);
		SDL_UpdateWindowSurface(patterntable_window.window);
	}
	return 0;
}



