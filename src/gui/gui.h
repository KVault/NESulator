#ifndef NESULATOR_GUI_H
#define NESULATOR_GUI_H

#include "gui_events.h"
#include "../utils/log.h"
#include "../utils/Utils.h"

/**
 * Function header for the callbacks. Every function has to match this header to be able to register itself
 * as a callback.
 */
typedef int (*event_func)(GUI_Event event);


/**
 * This is a bit messy function, but it needs to happen somewhere. Basically search in the event_callbacks array
 * for the one (if any) that matches the raised event. if found then call the function within it.
 */
void process_event_callbacks(GUI_Event *pEvent);

/**
 * Subscribes to the given event with a function to be called when the event is raised
 * @return 1 if succeeded, something else if didn't
 */
int sevent(GUI_Event, event_func func);

/**
 * Unsubscribes from the given event
 * @return 1 if succeeded, something else if didn't
 */
void uevent(GUI_Event, event_func func);

/**
 * Runs the gui event thread. Each frame (Defined elsewhere) the thread calling this function will
 */
void *gui_event_thread_run();

#endif //NESULATOR_GUI_H
