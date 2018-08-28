#include "mainWindow.h"

SDL_Window *window;
SDL_Event e;

//It is an array of pointers so that we can assign more than one function per event
sdl_event_func *efuncs[SDL_LASTEVENT];

/**
 * Creates an empty SDL window. It'll log any errors that may happen (Log level dependant)
 */
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
}

void gui_cycle(){
	//Handle the events
	while(SDL_PollEvent(&e) != 0){
		if(e.type == SDL_QUIT){

		}
	}

	//Redraw the image

	//Update the surfaces
	SDL_UpdateWindowSurface(window);
}

int close_window(){
	SDL_Quit();
}

int sevent(SDL_EventType event, sdl_event_func func) {
	*(*(efuncs+ sizeof(func))) = func;
	return 0;
}

void uevent(SDL_EventType event, sdl_event_func func) {

	//loop through the functions in the array for the specified event.
	//If any of the functions share the same pointer then delete it
}
