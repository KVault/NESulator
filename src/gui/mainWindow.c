#include "mainWindow.h"

struct main_window main_window;

uint back_buffer[MAIN_WINDOW_WIDTH][MAIN_WINDOW_HEIGHT];

/**
 * Cached variable that holds the refresh time in milliseconds. That is 1/refresh_rate
 */
static double refresh_time;

/**
 * Creates an empty SDL window. It'll log any errors that may happen (Log level dependant)
 */
int build_main_window(int speed) {
	main_window.refresh_rate = speed;
	refresh_time = (1.0/speed) * MILLISECOND;

	main_window.window = SDL_CreateWindow(MAIN_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                          MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (main_window.window == NULL) {
		log_error("Main Window could not be created: %s", SDL_GetError());
		return 0;
	}

	main_window.renderer = SDL_CreateRenderer(main_window.window, -1, SDL_RENDERER_ACCELERATED);
	main_window.back_buffer_tex = SDL_CreateTexture(main_window.renderer, SDL_PIXELFORMAT_ARGB8888,
	                                                SDL_TEXTUREACCESS_STATIC, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);

	//Register to the main gui cycle loop
	register_window_cycle(&cycle_main_window);
	sevent(SDL_WINDOWEVENT, SDL_WINDOWEVENT_CLOSE, &on_quit_main_window);
	sevent(SDL_WINDOWEVENT, SDL_WINDOWEVENT_RESIZED, &on_main_window_resized);
	sevent(SDL_KEYUP, SDLK_ESCAPE, &on_quit_main_window);
}

int on_quit_main_window(SDL_Event e) {
	if(e.window.windowID == SDL_GetWindowID(main_window.window) || e.key.keysym.sym == SDLK_ESCAPE) {
		SDL_DestroyTexture(main_window.back_buffer_tex);
		SDL_DestroyRenderer(main_window.renderer);
		SDL_DestroyWindow(main_window.window);

		//TODO call the quit for the rest of the windows. THis is a memory leak

		//Manually raise a QUIT event
		e.type = SDL_QUIT;
		e.quit.type = SDL_QUIT;
		SDL_PushEvent(&e);

		process_event_callbacks(&e);
		SDL_Quit();
	}
}

/**
 * This function WILL scale up or down the image
 */
int on_main_window_resized(SDL_Event e) {
	//TODO resize the canvas (But keep the aspect ratio of the NES's)
	SDL_RenderSetViewport(main_window.renderer, NULL);
}

int cycle_main_window(){
	static double last_check = 0;
	double time_aux = 0;
	if ((time_aux = has_time_elapsed(last_check, refresh_time))) {
		last_check = time_aux;

		gui_fps++;

		//Redraw the image
		for (int i = 0; i < MAIN_WINDOW_WIDTH; ++i) {
			for (int j = 0; j < MAIN_WINDOW_HEIGHT; ++j) {
				back_buffer[i][j] = (unsigned) (i * j);
			}
		}
		SDL_UpdateTexture(main_window.back_buffer_tex, NULL, back_buffer, MAIN_WINDOW_WIDTH * sizeof(uint));
		SDL_RenderClear(main_window.renderer);
		SDL_RenderCopy(main_window.renderer, main_window.back_buffer_tex, NULL, NULL);
		SDL_RenderPresent(main_window.renderer);

		//Update the surfaces
		SDL_UpdateWindowSurface(main_window.window);
	}
}
