#include "mainWindow.h"


int build_window(){

	//First version. It'll change in the future
	SDL_Window *window = NULL;
	SDL_Surface *surface = NULL;

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