#include "mainWindow.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event e;

//It is an array of pointers so that we can assign more than one function per event
sdl_event_func efuncs[SDL_LASTEVENT][MAX_FUNC_PER_EVENT][MAX_FUNC_PER_EVENT];

/**
 * Creates an empty SDL window. It'll log any errors that may happen (Log level dependant)
 */
int build_window(){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		log_error("SDL could not initialize: %s", SDL_GetError());
		return 0;
	}

	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if(window == NULL){
		log_error("Window could not be created: %s", SDL_GetError());
		return 0;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//Initialize the functions to a known value. 0 in this case. any other value than 0 will be treated as a function
	for (int i = 0; i < SDL_LASTEVENT; ++i) {
		for(int k = 0; k < MAX_FUNC_PER_EVENT; ++k) {
			for (int j = 0; j < MAX_FUNC_PER_EVENT; ++j) {
				efuncs[i][k][j] = NULL;
			}
		}
	}
}

void gui_cycle(){
	//Handle the events
	while(SDL_PollEvent(&e) != 0){
		//iterate through the events and call any event subscribed to it
		for (int i = 0; i < MAX_FUNC_PER_EVENT; ++i) {
			switch (e.type){
				case SDL_WINDOWEVENT:
					if(efuncs[e.type][e.window.type][i] == NULL) break;
					efuncs[e.type][e.window.type][i](e);
					break;
				case SDL_QUIT:
					if(efuncs[e.type][0][i] == NULL) break;
					efuncs[e.type][0][i](e);
					break;
				case SDL_DISPLAYEVENT:
					if(efuncs[e.type][e.display.type][i] == NULL) break;
					efuncs[e.type][e.display.type][i](e);
					break;
				default:break;
			}
		}
	}

	//Redraw the image

	//Update the surfaces
	SDL_UpdateWindowSurface(window);
}

int on_close_window(SDL_Event e){
	SDL_Quit();
}

int on_window_resized_event(SDL_Event e){
	log_error("yolo");
	SDL_RenderSetViewport(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

int sevent(SDL_EventType event, Uint8 event_id, sdl_event_func func) {
	//Because the array is squashed (See uevent) as soon as we find an empty slot (NULL) we can assign our func to it
	for (int i = 0; i < MAX_FUNC_PER_EVENT; ++i) {
		if(efuncs[event][event_id][i] == NULL){
			efuncs[event][event_id][i] = func;
			return 1;
		}
	}
	return 0;
}

void uevent(SDL_EventType event, Uint8 event_id, sdl_event_func func) {
	for (int i = 0; i < MAX_FUNC_PER_EVENT; ++i) {
		if(efuncs[event][event_id][i] == func) {
			efuncs[event][event_id][i] = NULL;

			//Now at this point we've removed a function from the array. So we have to squash the remaining functions
			//This is so that we don't leave any gaps in the array and then when calling them we can simply stop
			//when a 0x0 (NULL) is found
			for (int j = i; j < MAX_FUNC_PER_EVENT; ++j) {
				efuncs[i][event_id][j] = efuncs[i][event_id][j-1];
			}

			//Early return. quit the loop and all that it's holy
			return;
		}
	}
}
