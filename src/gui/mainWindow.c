#include "mainWindow.h"

struct main_window main_window;

static int refresh_rate;//Normally the GUI refresh rate is measured in FPS, not Hz but we'll assume it's the same
uint back_buffer[SCREEN_WIDTH][SCREEN_HEIGHT];


/**
 * Creates an empty SDL window. It'll log any errors that may happen (Log level dependant)
 */
int build_main_window(int speed) {
	refresh_rate = speed;

	main_window.window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (main_window.window == NULL) {
		log_error("Window could not be created: %s", SDL_GetError());
		return 0;
	}

	main_window.renderer = SDL_CreateRenderer(main_window.window, -1, SDL_RENDERER_ACCELERATED);
	main_window.back_buffer_tex = SDL_CreateTexture(main_window.renderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);

	register_window_cycle(&cycle_main_window);
}



/**
 * Determines whether enough time has elapsed since the last call to this function so that the gui should refresh
 *
 * @return 0 if it shouldn't, something else otherwise
 */
int should_refresh() {
	static double last_refresh = 0;
	static double current_time = 0;
	static double frame_time = 0;
	struct timespec ts;

	//This comes in seconds, multiply by 1000 to convert it it to milliseconds
	frame_time = (1.0 / refresh_rate) * 1000;
	clock_gettime(CLOCK_REALTIME, &ts);
	current_time = ts.tv_nsec / 2.0e6;//ts.tv_nsec comes in nanoseconds. This will convert it to milliseconds as well.

	//Now we're all on the same page, we can compare it
	if (fabs(current_time - last_refresh) > frame_time) {
		last_refresh = current_time;
		return 1;
	}
	return 0;
}


int on_close_window(SDL_Event e) {
	SDL_DestroyTexture(main_window.back_buffer_tex);
	SDL_DestroyRenderer(main_window.renderer);
	SDL_DestroyWindow(main_window.window);
	SDL_Quit();
}

/**
 * This function WILL scale up or down the image
 */
int on_window_resized_event(SDL_Event e) {
	//TODO resize the canvas (But keep the aspect ratio of the NES's)
	SDL_RenderSetViewport(main_window.renderer, NULL);
}

int cycle_main_window(){
	if (!should_refresh()) return 0;

	gui_fps++;


	//Redraw the image
	for (int i = 0; i < SCREEN_WIDTH; ++i) {
		for (int j = 0; j < SCREEN_HEIGHT; ++j) {
			back_buffer[i][j] = (unsigned)(i * j);
		}
	}
	SDL_UpdateTexture(main_window.back_buffer_tex, NULL, back_buffer, SCREEN_WIDTH* sizeof(uint));
	SDL_RenderClear(main_window.renderer);
	SDL_RenderCopy(main_window.renderer, main_window.back_buffer_tex, NULL, NULL);
	SDL_RenderPresent(main_window.renderer);

	//Update the surfaces
	SDL_UpdateWindowSurface(main_window.window);
}
