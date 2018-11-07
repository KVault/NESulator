#include <stdlib.h>
#include "pthread.h"
#include <stdio.h>

pthread_t gui_thread;

void *startup_gui(void *arg){
	system("dotnet GUItulator.dll");
	return NULL;
}

/**
 * This code should quite simple to "understand" all we're doing here is starting up the whole thing but in a not so
 * usual way. Because we want to keep debugging and making this essentially a C application, the entry point must
 * be a C `main` method. So here it is.
 *
 * This main will call a Python script that will run the .NET GUI in a new thread. That UI will internally use
 * the NESulator backend essentially closing up the circle and coming back here. That way we can still make use
 * of the debugging tools.
 *
 * yada yada yada..... we do it because we can.
 */
int main() {
	pthread_create(&gui_thread, NULL, startup_gui, NULL);
	pthread_join(gui_thread, NULL);
}