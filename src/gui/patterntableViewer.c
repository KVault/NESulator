#include "patterntableViewer.h"

struct patterntable_viewer_window patterntable_window;
double refresh_time;
uint back_buffer_left[PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT];
uint back_buffer_right[PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT];

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
	
	patterntable_window.left = SDL_CreateTexture(patterntable_window.renderer, SDL_PIXELFORMAT_RGB888,
	                                             SDL_TEXTUREACCESS_STATIC, PATTERNTABLE_TEXTURE_WIDTH,
	                                             PATTERNTABLE_TEXTURE_HEIGHT);

	patterntable_window.right = SDL_CreateTexture(patterntable_window.renderer, SDL_PIXELFORMAT_RGB888,
	                                              SDL_TEXTUREACCESS_STATIC, PATTERNTABLE_TEXTURE_WIDTH,
	                                              PATTERNTABLE_TEXTURE_HEIGHT);

	patterntable_window.left_rect.x = 0;
	patterntable_window.left_rect.y = 0;
	patterntable_window.left_rect.h = PATTERNTABLE_WINDOW_HEIGHT;
	patterntable_window.left_rect.w = PATTERNTABLE_WINDOW_WIDTH / 2;

	patterntable_window.right_rect.x = PATTERNTABLE_WINDOW_WIDTH / 2;
	patterntable_window.right_rect.y = 0;
	patterntable_window.right_rect.h = PATTERNTABLE_WINDOW_HEIGHT;
	patterntable_window.right_rect.w = PATTERNTABLE_WINDOW_WIDTH / 2;


	memset(back_buffer_left, 0, PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT * sizeof(uint));
	memset(back_buffer_right, 0, PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT * sizeof(uint));
	register_window_cycle(&cycle_patterntable_viewer);

	sevent(SDL_WINDOWEVENT, SDL_WINDOWEVENT_CLOSE, &on_quit_patterntable_viewer_window);
}

int on_quit_patterntable_viewer_window(SDL_Event e) {
	if (e.window.windowID == SDL_GetWindowID(patterntable_window.window)) {
		SDL_DestroyTexture(patterntable_window.left);
		SDL_DestroyTexture(patterntable_window.right);
		SDL_DestroyRenderer(patterntable_window.renderer);
		SDL_DestroyWindow(patterntable_window.window);

		//Manually raise a QUIT event
		e.type = SDL_QUIT;
		e.quit.type = SDL_QUIT;
		SDL_PushEvent(&e);

		process_event_callbacks(&e);
		SDL_Quit();
	}
	return 0;
}

void render_tiles(struct tile *tiles, uint *back_buffer) {
	for (byte i = 0; i < TILES_PER_TABLE; ++i) {
		for (int j = 0; j < TILE_ROW_SIZE; ++j) {
			for (int k = 0; k < TILE_COLUMN_SIZE; ++k) {
				int columnIndex = ((i & 0x0F) * TILE_COLUMN_SIZE) + k;
				int rowIndex = ((i >> 4) * TILE_ROW_SIZE) + j;
				if (tiles[i].pattern[j][k] != 0) {

					//TODO remove that magic number. For debug only
					back_buffer[PATTERNTABLE_TEXTURE_HEIGHT * rowIndex + columnIndex]
					= set_pixel(patterntable_window.window, COLOUR_PALETTE[tiles[i].pattern[j][k] + 15]);
				}
			}
		}
	}
}

int cycle_patterntable_viewer() {
	static double last_check;
	static struct tile *left_tiles;
	static struct tile *right_tiles;
	double time_aux = 0;

	//Only allocate it the first time
	if (left_tiles == NULL) {
		left_tiles = calloc(TILES_PER_TABLE + 1, sizeof(struct tile));
	}
	if (right_tiles == NULL) {
		right_tiles = calloc(TILES_PER_TABLE + 1, sizeof(struct tile));
	}

	if ((time_aux = has_time_elapsed(last_check, refresh_time))) {
		last_check = time_aux;

		encode_as_tiles(vram_bank, TILES_PER_TABLE, left_tiles);
		encode_as_tiles(vram_bank + 0x1000, TILES_PER_TABLE, right_tiles);
		render_tiles(left_tiles, back_buffer_left);
		render_tiles(right_tiles, back_buffer_right);


		SDL_UpdateTexture(patterntable_window.left, NULL, back_buffer_left,
		                  PATTERNTABLE_TEXTURE_WIDTH * sizeof(uint));

		SDL_UpdateTexture(patterntable_window.right, NULL, back_buffer_right,
		                  PATTERNTABLE_TEXTURE_WIDTH * sizeof(uint));

		SDL_RenderClear(patterntable_window.renderer);
		SDL_RenderCopy(patterntable_window.renderer, patterntable_window.left, NULL, &patterntable_window.left_rect);
		SDL_RenderCopy(patterntable_window.renderer, patterntable_window.right, NULL, &patterntable_window.right_rect);
		SDL_RenderPresent(patterntable_window.renderer);
	}
	return 0;
}




