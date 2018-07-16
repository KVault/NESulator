#include <assert.h>
#include "TestMemory.h"

/**
 * Tests how the system writes to memory. in order to actually only test the writing, we have to compare
 * the results by accessing the array directly. We wouldn't do that usually.
 */
void testWrite() {
	zeroMemory();

	//Test one byte write
	assert(memoryBank[0] == 0);
	wmem_b(0, 'd');
	assert(memoryBank[0] == 'd');

	zeroMemory();

	//Test two byte write on random positions
	assert(memoryBank[512] == 0);
	assert(memoryBank[513] == 0);
	assert(memoryBank[514] == 0);
	wmem_w(512, 0x6969);

	assert(memoryBank[512] == 0x69);
	assert(memoryBank[513] == 0x69);

	//Test the memory write repeat
	zeroMemory();
	assert(memoryBank[512] == 0);
	assert(memoryBank[513] == 0);

	printf("Write memory test passed!\n");
}

/**
 * Tests how the system reads from memory. Because the write test has passed all the asserts before this one
 * We can assume it works fine so we'll use wmem to ease out the writing
 */
void testRead() {
	zeroMemory();

	//Test one byte read
	assert(rmem_b(0) == 0);
	//Fill in the test data
	wmem_b(0, 'd');
	//Make sure the test data is in there
	assert(rmem_b(0) == 'd');

	//Test three bytes read
	//Test the empty values
	zeroMemory();
	assert(rmem_w(512) == 0x0000);
	//Fill in the test data
	wmem_w(512, (word) 0x6942);
	assert(rmem_w(512) == 0x6942);

	printf("Read memory test passed!\n");
}

/**
 * Tests how the stack works
 */
void testStack() {
	power_up(0);
	zeroMemory();
	word pushParam = 0x6942;

	push_w(pushParam);
	assert(SP == 0xFB);
	assert(memoryBank[0x01FD] == 0x69);
	assert(memoryBank[0x01FC] == 0x42);
	assert(peek_w() == 0x6942);
	assert(SP == 0xFB);
	assert(pop_w() == 0x6942);
	assert(SP == 0xFD);

	printf("Stack memory test passed!\n");
}

/**
 * Tests how the indexed indirect mode works
 */
void testIndirectXAddr() {
	word addr = 0;
	// First test
	zeroMemory();
	wmem_w(112, 0xAC30);
	X = 100;
	addr = indirectx_addr(12);
	assert(addr == 0xAC30);

	// Second test. Example got from here http://www.emulator101.com/6502-addressing-modes.html
	zeroMemory();
	wmem_w(126, 0x2074);
	X = 100;
	addr = indirectx_addr(26);
	assert(addr == 0x2074);

	printf("Test IndirectXAddr passed!\n");
}

void testIndirectYAddr() {
	//First test. Example got from here http://www.emulator101.com/6502-addressing-modes.html
	zeroMemory();
	wmem_w(0x86, 0x4028);
	Y = 0x10;
	word addr = indirecty_addr(0x86);
	assert(addr == 0x4038);
	printf("Test IndirectYAddr passed!\n");
}

void testZeroPageAddr() {
	//First test. http://www.emulator101.com/6502-addressing-modes.html
	zeroMemory();
	word addr = 0;

	// zpagex_addr test
	X = 0x60;
	addr = zpagex_addr(0xC0);
	assert(0x0020 == addr);
	printf("Test ZeroPageXAddr passed!\n");

	// zpagey_addr test
	Y = 0x10;
	addr = zpagey_addr(0xFB);
	assert(0x000B == addr);
	printf("Test ZeroPageYAddr passed!\n");

	// zpage_addr test
	addr = zpage_addr(0x6969);
	assert(0x0069 == addr);
	printf("Test ZeroPageAddr passed!\n");
}

void testAbsoluteAddr() {
	//First test. Absolute. I know, silly but necessary
	zeroMemory();
	word addr;
	word param = 0x6969;
	addr = absolute_addr(param);
	assert(addr == 0x6969);

	//Second test. Absolute X
	zeroMemory();
	word param2 = 0x6959;
	X = 0x10;
	addr = absolutex_addr(param2);
	assert(addr == 0x6969);

	//Third test. Absolute Y
	zeroMemory();
	word param3 = 0x6949;
	Y = 0x20;
	addr = absolutey_addr(param3);
	assert(addr == 0x6969);

	printf("Test AbsoluteAddr passed! \n");
}
