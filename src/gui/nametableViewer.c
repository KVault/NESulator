#include "nametableViewer.h"

static struct nametable_viewer_window window;
uint back_buffer[NAMETABLE_WINDOW_WIDTH * NAMETABLE_WINDOW_HEIGHT];

int build_nametable_viewer(int refresh_rate) {
	window.refresh_time = (1.0/ refresh_rate) * MILLISECOND;
	window.window = SDL_CreateWindow(NAMETABLE_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, NAMETABLE_WINDOW_WIDTH, NAMETABLE_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

	if(window.window == NULL){
		log_error("Nametable Window could not be created: %s", SDL_GetError());
		return 0;
	}

	window.renderer = SDL_CreateRenderer(window.window, -1, SDL_RENDERER_ACCELERATED);

	window.back_buffer_tex = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGB888,
			SDL_TEXTUREACCESS_STATIC, NAMETABLE_WINDOW_WIDTH, NAMETABLE_WINDOW_HEIGHT);

	register_window_cycle(&cycle_nametable_viewer);
	sevent(SDL_WINDOWEVENT, SDL_WINDOWEVENT_CLOSE, &on_quit_nametable_viewer_window);
	return 0;
}

int on_quit_nametable_viewer_window(SDL_Event e) {
	if(e.window.windowID == SDL_GetWindowID(window.window)){
		SDL_HideWindow(window.window);
		unregister_window_cycle(&cycle_nametable_viewer);
		return 1;
	}
	return 0;
}

void render_nametable(){

}

int cycle_nametable_viewer() {
	static double last_check = 0;
	double time_aux = 0;
	if((time_aux = has_time_elapsed(last_check, window.refresh_time))){
		last_check = time_aux;

		SDL_UpdateTexture(window.back_buffer_tex, NULL, back_buffer,NAMETABLE_WINDOW_WIDTH);
		SDL_RenderClear(window.renderer);
		SDL_RenderCopy(window.renderer, window.back_buffer_tex, NULL, NULL);
		SDL_RenderPresent(window.renderer);

		//Update the surfaces
		SDL_UpdateWindowSurface(window.window);
	}
	return 0;
}
