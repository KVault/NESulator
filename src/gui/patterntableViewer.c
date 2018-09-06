#include "patterntableViewer.h"

struct patterntable_viewer_window patterntable_window;
struct tile *patterns;
double refresh_time;
uint back_buffer[PATTERNTABLE_TEXTURE_WIDTH][PATTERNTABLE_TEXTURE_HEIGHT];

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

	patterntable_window.back_buffer_tex = SDL_CreateTexture(patterntable_window.renderer, SDL_PIXELFORMAT_ARGB8888,
	                                                        SDL_TEXTUREACCESS_STATIC, PATTERNTABLE_TEXTURE_WIDTH,
	                                                         PATTERNTABLE_TEXTURE_HEIGHT);

	memset(back_buffer, 0 , PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT * sizeof(uint));
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

		free(patterns);
		patterns = encode_as_tiles(vram_bank, 0xFF);

		//IS THIS MAGIC? WTF is this shit
		for (byte i = 0; i < 0xFF; ++i) {
			for (int j = 0; j < 8; ++j) {
				for (int k = 0; k < 8; ++k) {
					byte columnIndex = (byte)(i & 0x0F);
					byte rowIndex = i >> 4;
					if(patterns[i].pattern[j][k] != 0){
						back_buffer[(columnIndex * 8) + j][(rowIndex * 8)+ k] = 255; //COLOUR_PALETTE[patterns[i].pattern[j][k] + 0x05];
					}
				}
			}
		}
		

		SDL_UpdateTexture(patterntable_window.back_buffer_tex, NULL, back_buffer, PATTERNTABLE_TEXTURE_WIDTH * sizeof(uint));

		SDL_RenderClear(patterntable_window.renderer);
		SDL_RenderCopy(patterntable_window.renderer, patterntable_window.back_buffer_tex, NULL, NULL);
		SDL_RenderPresent(patterntable_window.renderer);
	}
	return 0;
}




