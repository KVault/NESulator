#include "mainWindow.h"
#include "../nes.h"

SDL_Window *window;
SDL_Event e;

int build_window(){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		log_error("SDL could not initialize: %s", SDL_GetError());
		return 0;
	}

	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if(window == NULL){
		log_error("Window could not be created: %s", SDL_GetError());
		return 0;
	}

	SDL_UpdateWindowSurface( window );
}

void gui_cycle(){
	while(SDL_PollEvent(&e) != 0){
		if(e.type == SDL_QUIT){
			stopEmulation();
		}
	}
}

int close_window(){
	SDL_Quit();
}