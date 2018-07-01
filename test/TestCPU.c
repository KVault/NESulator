#include "TestCPU.h"

/**
 * Simply calls each function to test. It's going to get messy but what the hell!....
 */
void testOpcodes(){
	test_BRK();
}

/**
 * For this test to succeed, after executing BRK, PC has to increase just by one,
 * the B flag and the Z flag have both to be 1
 */
void test_BRK(){
	int cachedPC = PC;
	//assert(bit_test(P,6) == 0);
	wmem(BYTE, PC, 0x00);
	//cpu_cycle();
	assert(cachedPC+1 == PC);
	//assert(bit_test(P,6) == 1);
	//assert(bit_test(P,6) == 1);
}