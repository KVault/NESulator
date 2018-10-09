#include "gui.h"

/**
 * Size of event_callbacks array. This can be incremented if needed, but for now this number will do
 */
#define efunc_size 10

/**
 * Holds the information to accurately match a callback with an event type and the id within that type
 */
struct Event_Callback_Info_Struct {
	int event_id;
	event_func callback;
};

/**
 *Array of fixed size. It could be done better, but for this many items, it doesn't matter
 */
struct Event_Callback_Info_Struct event_callbacks[efunc_size];

void process_event_callbacks(GUI_Event *pEvent);

/**
 * This is a bit messy function, but it needs to happen somewhere. Basically search in the event_callbacks array
 * for the one (if any) that matches the raised event. if found then call the function within it.
 */
void process_event_callbacks(GUI_Event *pEvent) {
	for (int i = 0; i < efunc_size; ++i) {
		//Cache the reference. It looks cleaner
		struct Event_Callback_Info_Struct *ec = &event_callbacks[i];

		if (ec->event_id == (int) pEvent) {
			ec->callback(*pEvent);
			uevent(*pEvent, event_callbacks[i].callback );//Remove the event from the list
		}
	}
}

int sevent(GUI_Event event, event_func func) {
	//Assign the callback to the first empty position. It does not guarantee the order
	for (int i = 0; i < efunc_size; ++i) {
		if (event_callbacks[i].callback == NULL) {
			event_callbacks[i].callback = func;
			event_callbacks[i].event_id = event;;
			return 0;
		}
	}

	//if we get to this point it means we've run out of slots, so raise an error. Ideally we should never see
	//this error. But it's going to help us when developing the emulator.
	log_error("event_callbacks has overflown. An event can't be assigned because the array is already full."
	          " Please increase the limit of the array\n");

	return 1;//Means error
}

void uevent(GUI_Event event, event_func func) {
	//Search for the subscribed event and if found, NULL the callback.
	for (int i = 0; i < efunc_size; ++i) {
		if (event_callbacks[i].callback == func && event_callbacks[i].event_id == event) {
			event_callbacks[i].callback = NULL;

			return;//No error
		}
	}

	log_error("The function given for the EventType: %i couldn't be found.\n", event);
}