#include "mainWindow.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event e;

#define efunc_size 20

/**
 * Holds the information to accurately match a callback with an event type and the id within that type
 */
typedef struct Event_Callback_Info {
	int etype; //Event type. It should be a value of SDL_EventType. this value should be unique
	int eid; //Event id within the SDL_EventType. This value can be different
	sdl_event_func callback;
} Event_Callback_Info;

//Array of fixed size. It could be done better, but for this many items, it doesn't matter
struct Event_Callback_Info event_callbacks[efunc_size];

void process_event_callbacks(SDL_Event *pEvent);

/**
 * Creates an empty SDL window. It'll log any errors that may happen (Log level dependant)
 */
int build_window(){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		log_error("SDL comuld not initialize: %s", SDL_GetError());
		return 0;
	}

	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if(window == NULL){
		log_error("Window could not be created: %s", SDL_GetError());
		return 0;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//Initialize the struct array to a known value. NULL for the callback and -1 for the events.
	for (int i = 0; i < efunc_size; ++i) {
		event_callbacks[i].callback = NULL;
		event_callbacks[i].eid = -1;
		event_callbacks[i].etype = -1;
	}
}

void gui_cycle(){
	//Handle the events
	while(SDL_PollEvent(&e) != 0){
		process_event_callbacks(&e);
	}

	//Redraw the image

	//Update the surfaces
	SDL_UpdateWindowSurface(window);
}

/**
 * This is a bit messy function, but it needs to happen somewhere. Basically search in the event_callbacks array
 * for the one (if any) that matches the raised event. if found then call the function within it.
 */
void process_event_callbacks(SDL_Event *pEvent) {
	for (int i = 0; i < efunc_size; ++i) {
		struct Event_Callback_Info ec = event_callbacks[i];
		if(ec.etype == pEvent->type &&
			(ec.eid == pEvent->display.event
			|| ec.eid == pEvent->window.event
			|| ec.eid == pEvent->button.button
			|| ec.eid == pEvent->common.type
			|| ec.eid == pEvent->quit.type
			|| ec.eid == pEvent->key.keysym.sym
			|| ec.eid == pEvent->user.code)){
			ec.callback(*pEvent);
		}
	}
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

int sevent(SDL_EventType event, uint event_id, sdl_event_func func) {
	//Asign the callback to the first empty position. It does not guarantee the order
	for (int i = 0; i < efunc_size; ++i) {
		if(event_callbacks[i].callback == NULL){
			event_callbacks[i].callback = func;
			event_callbacks[i].etype = event;
			event_callbacks[i].eid = event_id;
			return 0;
		}
	}

	//if we get to this point it means we've run out of slots, so raise an error. Ideally we should never see
	//this error. But it's going to help us when developing the emulator.
	log_error("event_callbacks has overflown. An event can't be assigned because the array is already full."
	          " Please increase the limit of the array\n");

	return 1;//Means error
}

void uevent(SDL_EventType event, uint event_id, sdl_event_func func) {
	//Search for the subscribed event and if found, NULL the callback.
	for (int i = 0; i < efunc_size; ++i) {
		if(event_callbacks[i].callback == func
			&& event_callbacks[i].etype == event
			&& event_callbacks[i].eid == event_id){
			event_callbacks[i].callback = NULL;
			event_callbacks[i].etype = -1;
			event_callbacks[i].eid = -1;

			return;//No error
		}
	}

	log_error("The function given for the EventType: %i and the EventID: %i couldn't be found.\n", event, event_id);
}
