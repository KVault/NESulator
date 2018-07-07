#include <assert.h>
#include "../src/Utils.h"
#include "TestMemory.h"
#include "TestCPU.h"

/**
 * Tests the utils, we'll put this here for now
 */
void testUtils();

int main() {
	//Good old utils :3
	testUtils();

	//Memory tests
	testWrite();
	testRead();
	testStack();

	//Memory modes
	testIndirectXAddr();
	testIndirectYAddr();
	testZeroPageAddr();
	testAbsoluteAddr();

	//CPU tests
	testOpcodes();
	printf("\n*** All test passed! ***\n");
}

void testUtils() {
	byte testVal = 0b00000000;
	assert(bit_test(testVal, 0) == 0);
	assert(bit_test(testVal, 2) == 0);
	assert(bit_test(testVal, 3) == 0);
	assert(bit_test(testVal, 5) == 0);
	assert(bit_test(testVal, 7) == 0);
	testVal = 0b10100100;
	assert(bit_test(testVal, 0) == 0);
	assert(bit_test(testVal, 2) == 1);
	assert(bit_test(testVal, 3) == 0);
	assert(bit_test(testVal, 5) == 1);
	assert(bit_test(testVal, 7) == 1);
	printf("testUtils test passed!\n");
}