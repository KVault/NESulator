#include "patterntableViewer.h"

static struct patterntable_viewer_window window;

uint back_buffer_right[PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT];
uint back_buffer_left [PATTERNTABLE_TEXTURE_WIDTH * PATTERNTABLE_TEXTURE_HEIGHT];


int build_patterntable_viewer(int refresh_rate) {
	window.refresh_time = (1.0 / refresh_rate) * MILLISECOND;
	window.window = SDL_CreateWindow(PATTERNTABLE_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
	                                              SDL_WINDOWPOS_UNDEFINED, PATTERNTABLE_WINDOW_WIDTH,
	                                              PATTERNTABLE_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

	if (window.window == NULL) {
		log_error("Patterntable window could not be created: %s\n", SDL_GetError());
	}

	window.renderer = SDL_CreateRenderer(window.window, -1, SDL_RENDERER_ACCELERATED);
	
	window.left = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGB888,
	                                             SDL_TEXTUREACCESS_STATIC, PATTERNTABLE_TEXTURE_WIDTH,
	                                             PATTERNTABLE_TEXTURE_HEIGHT);

	window.right = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGB888,
	                                              SDL_TEXTUREACCESS_STATIC, PATTERNTABLE_TEXTURE_WIDTH,
	                                              PATTERNTABLE_TEXTURE_HEIGHT);

	window.left_rect.x = 0;
	window.left_rect.y = 0;
	window.left_rect.h = PATTERNTABLE_WINDOW_HEIGHT;
	window.left_rect.w = PATTERNTABLE_WINDOW_WIDTH / 2;

	window.right_rect.x = PATTERNTABLE_WINDOW_WIDTH / 2;
	window.right_rect.y = 0;
	window.right_rect.h = PATTERNTABLE_WINDOW_HEIGHT;
	window.right_rect.w = PATTERNTABLE_WINDOW_WIDTH / 2;

	register_window_cycle(&cycle_patterntable_viewer);
	sevent(SDL_WINDOWEVENT, SDL_WINDOWEVENT_CLOSE, &on_quit_patterntable_viewer_window);
}

int on_quit_patterntable_viewer_window(SDL_Event e) {
	if (e.window.windowID == SDL_GetWindowID(window.window)) {
		SDL_HideWindow(window.window);
		unregister_window_cycle(&cycle_patterntable_viewer);
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
					= set_pixel(window.window, COLOUR_PALETTE[tiles[i].pattern[j][k] + 15]);
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

	if ((time_aux = has_time_elapsed(last_check, window.refresh_time))) {
		last_check = time_aux;

		encode_as_tiles(&PPU_PATTERN_LEFT, TILES_PER_TABLE, left_tiles);
		encode_as_tiles(&PPU_PATTERN_RIGHT, TILES_PER_TABLE, right_tiles);
		render_tiles(left_tiles, back_buffer_left);
		render_tiles(right_tiles, back_buffer_right);


		SDL_UpdateTexture(window.left, NULL, back_buffer_left,
		                  PATTERNTABLE_TEXTURE_WIDTH * sizeof(uint));

		SDL_UpdateTexture(window.right, NULL, back_buffer_right,
		                  PATTERNTABLE_TEXTURE_WIDTH * sizeof(uint));

		SDL_RenderClear(window.renderer);
		SDL_RenderCopy(window.renderer, window.left, NULL, &window.left_rect);
		SDL_RenderCopy(window.renderer, window.right, NULL, &window.right_rect);
		SDL_RenderPresent(window.renderer);
	}
	return 0;
}




