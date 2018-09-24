#include "nametableViewer.h"

static struct nametable_viewer_window window;

/**
 * This back buffer is shared among all the textures.
 */
uint back_buffer[NAMETABLE_TEX_HEIGHT][NAMETABLE_TEX_WIDTH];

int build_nametable_viewer(int refresh_rate) {
	window.refresh_time = (1.0/ refresh_rate) * MILLISECOND;
	window.window = SDL_CreateWindow(NAMETABLE_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, NAMETABLE_WINDOW_WIDTH, NAMETABLE_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

	if(window.window == NULL){
		log_error("Nametable Window could not be created: %s", SDL_GetError());
		return 0;
	}

	window.renderer = SDL_CreateRenderer(window.window, -1, SDL_RENDERER_ACCELERATED);

	window.top_left = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGB888,
			SDL_TEXTUREACCESS_STATIC, NAMETABLE_TEX_WIDTH, NAMETABLE_TEX_HEIGHT);

	window.top_right = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGB888,
			SDL_TEXTUREACCESS_STATIC, NAMETABLE_TEX_WIDTH, NAMETABLE_TEX_HEIGHT);

	window.bottom_left = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGB888,
	        SDL_TEXTUREACCESS_STATIC, NAMETABLE_TEX_WIDTH, NAMETABLE_TEX_HEIGHT);

	window.bottom_right = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGB888,
			SDL_TEXTUREACCESS_STATIC, NAMETABLE_TEX_WIDTH, NAMETABLE_TEX_HEIGHT);

	//Now fill in the rects to be used in by the textures. Each one of them in a corner
	window.top_left_rect.x = 0;
	window.top_left_rect.y = 0;
	window.top_left_rect.h = NAMETABLE_WINDOW_HEIGHT/2;
	window.top_left_rect.w = NAMETABLE_WINDOW_WIDTH/2;

	window.top_right_rect.x = NAMETABLE_WINDOW_WIDTH/2;
	window.top_right_rect.y = 0;
	window.top_right_rect.h = NAMETABLE_WINDOW_HEIGHT/2;
	window.top_right_rect.w = NAMETABLE_WINDOW_WIDTH/2;

	window.bottom_left_rect.x = 0;
	window.bottom_left_rect.y = NAMETABLE_WINDOW_HEIGHT/2;
	window.bottom_left_rect.h = NAMETABLE_WINDOW_HEIGHT/2;
	window.bottom_left_rect.w = NAMETABLE_WINDOW_WIDTH/2;

	window.bottom_right_rect.x = NAMETABLE_WINDOW_WIDTH/2;
	window.bottom_right_rect.y = NAMETABLE_WINDOW_HEIGHT/2;
	window.bottom_right_rect.h = NAMETABLE_WINDOW_HEIGHT/2;
	window.bottom_right_rect.w = NAMETABLE_WINDOW_WIDTH/2;

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

void render_tile(struct tile *tile, uint row_id, uint column_id) {
	for (int i = 0; i < TILE_ROW_SIZE; ++i) {
		for (int j = 0; j < TILE_COLUMN_SIZE; ++j) {
			uint draw_color = tile->pattern[i][j] != 0 ? 255 : 0; //TODO remove magic numbers. For Debug only
			back_buffer[(row_id * TILE_ROW_SIZE) + i][(column_id *TILE_COLUMN_SIZE)+ j] = draw_color;
		}
	}
}

void render_nametable_map(word start_addr){
	for (uint i = 0; i < NAMETABLE_ROWS_MAP; ++i) {
		for (uint j = 0; j < NAMETABLE_TILES_PER_ROW; ++j) {
			byte tile_id = rmem_b_vram(start_addr + ((i * NAMETABLE_TILES_PER_ROW)  + j));
			struct tile tile = nametable_tile(tile_id);
			render_tile(&tile, i, j);
		}
	}
}

int cycle_nametable_viewer() {
	static double last_check = 0;
	double time_aux = 0;
	if((time_aux = has_time_elapsed(last_check, window.refresh_time))){
		last_check = time_aux;

		SDL_RenderClear(window.renderer);

		render_nametable_map(get_nt_start_addr(NT_TOP_LEFT));
		SDL_UpdateTexture(window.top_left, NULL, back_buffer,NAMETABLE_TEX_WIDTH * sizeof(uint));
		SDL_RenderCopy(window.renderer, window.top_left, NULL, &window.top_left_rect);

		render_nametable_map(get_nt_start_addr(NT_TOP_RIGHT));
		SDL_UpdateTexture(window.top_right, NULL, back_buffer,NAMETABLE_TEX_WIDTH * sizeof(uint));
		SDL_RenderCopy(window.renderer, window.top_right, NULL, &window.top_right_rect);

		render_nametable_map(get_nt_start_addr(NT_BOTTOM_LEFT));
		SDL_UpdateTexture(window.bottom_left, NULL, back_buffer,NAMETABLE_TEX_WIDTH * sizeof(uint));
		SDL_RenderCopy(window.renderer, window.bottom_left, NULL, &window.bottom_left_rect);

		render_nametable_map(get_nt_start_addr(NT_BOTTOM_RIGHT));
		SDL_UpdateTexture(window.bottom_right, NULL, back_buffer,NAMETABLE_TEX_WIDTH * sizeof(uint));
		SDL_RenderCopy(window.renderer, window.bottom_right, NULL, &window.bottom_right_rect);

		SDL_RenderPresent(window.renderer);

		//Update the surfaces
		SDL_UpdateWindowSurface(window.window);
	}
	return 0;
}
