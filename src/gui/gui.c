#include "gui.h"
#include "nametableViewer.h"

/**
 * Size of event_callbacks array. This can be incremented if needed, but for now this number will do
 */
#define efunc_size 10

/**
 * Max number of windows. This is static, never changed.
 */
#define GUI_WINDOW_COUNT 5

gui_window_cycle window_cycles[GUI_WINDOW_COUNT];
/**
 * Used when pooling, just allocate it now and be doen with it
 */
SDL_Event e;

/**
 * Holds the information to accurately match a callback with an event type and the id within that type
 */
struct Event_Callback_Info_Struct {
	int etype; //Event type. It should be a value of SDL_EventType. this value should be unique
	int eid; //Event id within the SDL_EventType. This value can be different
	sdl_event_func callback;
};

/**
 *Array of fixed size. It could be done better, but for this many items, it doesn't matter
 */
struct Event_Callback_Info_Struct event_callbacks[efunc_size];
void process_event_callbacks(SDL_Event *pEvent);

/**
 * This is a bit messy function, but it needs to happen somewhere. Basically search in the event_callbacks array
 * for the one (if any) that matches the raised event. if found then call the function within it.
 */
void process_event_callbacks(SDL_Event *pEvent) {
	for (int i = 0; i < efunc_size; ++i) {

		//Cache the reference. It looks cleaner
		struct Event_Callback_Info_Struct *ec = &event_callbacks[i];

		//Sorry :'(
		if (ec->etype == pEvent->type &&
		    (ec->eid == pEvent->display.event
		     || ec->eid == pEvent->window.event
		     || ec->eid == pEvent->button.button
		     || ec->eid == pEvent->common.type
		     || ec->eid == pEvent->quit.type
		     || ec->eid == pEvent->key.keysym.sym
		     || ec->eid == pEvent->user.code)) {
			ec->callback(*pEvent); //And all of that for this little thing. Where the magic happens
		}
	}
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

int register_window_cycle(gui_window_cycle cycle_func){
	for (int i = 0; i < GUI_WINDOW_COUNT; ++i) {
		if(window_cycles[i] == NULL){
			window_cycles[i] = *cycle_func;
			return 1;
		}
	}
	return 0;
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

int gui_init(){
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log_error("SDL could not initialize: %s", SDL_GetError());
		return 0;
	}


	//Initialize the struct array to a known value. NULL for the callback and -1 for the events.
	for (int i = 0; i < efunc_size; ++i) {
		event_callbacks[i].callback = NULL;
		event_callbacks[i].eid = -1;
		event_callbacks[i].etype = -1;
	}

	//Now Build all the windows
	build_main_window(60);
	build_nametable_viewer(20);
}

double has_time_elapsed(double last_refresh, double time){
	static struct timespec ts;
	static double current_time = 0;

	clock_gettime(CLOCK_REALTIME, &ts);
	current_time = ts.tv_nsec / 2.0e6;//ts.tv_nsec comes in nanoseconds. This will convert it to milliseconds as well.

	//Now we're all on the same page, we can compare it
	if (fabs(current_time - last_refresh) > time) {
		return current_time;
	}
	return 0;
}

void gui_cycle() {
	//Handle the events
	while (SDL_PollEvent(&e) != 0) {
		process_event_callbacks(&e);
	}

	/**
	 * For each window, call its cycle. each window will know what to do depending on its framerate (if any)
	 * or if the window is visible or not, this function does not care about that. It's not its job.
	 */
	for (int i = 0; i < GUI_WINDOW_COUNT; ++i) {
		if(window_cycles[i]!= NULL){
			window_cycles[i]();
		}
	}


}