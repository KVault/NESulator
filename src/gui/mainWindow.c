#include "mainWindow.h"
#include "../cpu.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *back_buffer_tex;
SDL_Event e;
static int refresh_rate;//Normally the GUI refresh rate is measured in FPS, not Hz but we'll assume it's the same
uint back_buffer[SCREEN_WIDTH][SCREEN_HEIGHT];

/**
 * Size of event_callbacks array. This can be incremented if needed, but for now this number will do
 */
#define efunc_size 10

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


/**
 * Creates an empty SDL window. It'll log any errors that may happen (Log level dependant)
 */
int build_window(int speed) {
	refresh_rate = speed;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log_error("SDL comuld not initialize: %s", SDL_GetError());
		return 0;
	}

	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (window == NULL) {
		log_error("Window could not be created: %s", SDL_GetError());
		return 0;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	back_buffer_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Initialize the struct array to a known value. NULL for the callback and -1 for the events.
	for (int i = 0; i < efunc_size; ++i) {
		event_callbacks[i].callback = NULL;
		event_callbacks[i].eid = -1;
		event_callbacks[i].etype = -1;
	}
}


void process_event_callbacks(SDL_Event *pEvent);

/**
 * This is a bit messy function, but it needs to happen somewhere. Basically search in the event_callbacks array
 * for the one (if any) that matches the raised event. if found then call the function within it.
 */
void process_event_callbacks(SDL_Event *pEvent) {
	for (int i = 0; i < efunc_size; ++i) {

		//Cache the reference. It looks cleaner
		struct Event_Callback_Info ec = event_callbacks[i];

		//Sorry :'(
		if (ec.etype == pEvent->type &&
		    (ec.eid == pEvent->display.event
		     || ec.eid == pEvent->window.event
		     || ec.eid == pEvent->button.button
		     || ec.eid == pEvent->common.type
		     || ec.eid == pEvent->quit.type
		     || ec.eid == pEvent->key.keysym.sym
		     || ec.eid == pEvent->user.code)) {
			ec.callback(*pEvent); //And all of that for this little thing. Where the magic happens
		}
	}
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

void gui_cycle() {
	if (!should_refresh()) return;

	gui_fps++;
	//Handle the events
	while (SDL_PollEvent(&e) != 0) {
		process_event_callbacks(&e);
	}

	//Redraw the image
	for (int i = 0; i < SCREEN_WIDTH; ++i) {
		for (int j = 0; j < SCREEN_HEIGHT; ++j) {
			back_buffer[i][j] = (unsigned)(i * j);
		}
	}
	SDL_UpdateTexture(back_buffer_tex, NULL, back_buffer, SCREEN_WIDTH* sizeof(uint));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, back_buffer_tex, NULL, NULL);
	SDL_RenderPresent(renderer);

	//Update the surfaces
	SDL_UpdateWindowSurface(window);
}

int on_close_window(SDL_Event e) {
	SDL_DestroyTexture(back_buffer_tex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

/**
 * This function WILL scale up or down the image
 */
int on_window_resized_event(SDL_Event e) {
	//TODO resize the canvas (But keep the aspect ratio of the NES's)
	SDL_RenderSetViewport(renderer, NULL);
}

int sevent(SDL_EventType event, uint event_id, sdl_event_func func) {
	//Assign the callback to the first empty position. It does not guarantee the order
	for (int i = 0; i < efunc_size; ++i) {
		if (event_callbacks[i].callback == NULL) {
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
		if (event_callbacks[i].callback == func
		    && event_callbacks[i].etype == event
		    && event_callbacks[i].eid == event_id) {
			event_callbacks[i].callback = NULL;
			event_callbacks[i].etype = -1;
			event_callbacks[i].eid = -1;

			return;//No error
		}
	}

	log_error("The function given for the EventType: %i and the EventID: %i couldn't be found.\n", event, event_id);
}