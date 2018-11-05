#include <stdio.h>

/**
 * This code is quite simple to "understand" all we're doing here is starting up the whole thing but in a not so
 * usual way. Because we want to keep debugging and making this essentially a C application, the entry point must
 * be a C `main` method. So here it is.
 *
 * This main will call a Python script that will run the .NET GUI in a new thread. That UI will internally use
 * the NESulator backend essentially closing up the circle and coming back here. That way we can still make use
 * of the debugging tools.
 *
 * yada yada we do it because we can.
 */
int main(){
	printf("yolo");
}