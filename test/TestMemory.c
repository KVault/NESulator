#include <assert.h>
#include "TestMemory.h"

/**
 * Tests how the system writes to memory. in order to actually only test the writing, we have to compare
 * the results by accessing the array directly. We wouldn't do that usually.
 */
void testWrite() {
	zeroMemory();

	//Test one byte write
	assert(address[0] == 0);
	byte data = 'd';
	wmem(BYTE, 0, &data);
	assert(address[0] == 'd');

	zeroMemory();

	//Test two byte write on random positions
	assert(address[512] == 0);
	assert(address[513] == 0);
	assert(address[514] == 0);
	byte* nesText= "NES";
	wmem(3, 512, nesText);

	assert(address[512] == 'N');
	assert(address[513] == 'E');
	assert(address[514] == 'S');

	printf("Write memory test passed!\n");
}

/**
 * Tests how the system reads from memory. Because the write test has passed all the asserts before this one
 * We can assume it works fine so we'll use wmem to ease out the writing
 */
void testRead() {
	zeroMemory();

	//Test one byte write
	byte *readData = malloc(1);
	rmem(BYTE, 0, &readData);
	assert(readData[0] == 0);
	byte data = 'd';
	wmem(BYTE, 0, &data);
	rmem(BYTE, 0, &readData);
	assert(readData[0] == 'd');
	free(readData);

	printf("Read memory test passed!\n");
}