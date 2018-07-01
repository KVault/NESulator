#include "TestCPU.h"

/**
 * Simply calls each function to test. It's going to get messy but what the hell!....
 */
void testOpcodes() {
    power_up(0);
    test_BRK();
}

/**
 * For this test to succeed, after executing BRK, PC has to increase just by one,
 * the B flag and the Z flag have both to be 1
 */
void test_BRK() {
    int cachedPC = PC;
    int cachedCyclesThisSec = cyclesThisSec;
    bit_clear(&P, 4);
    assert(bit_test(P, 4) == 0);
    wmem_const(BYTE, PC, 0x00);
    cpu_cycle();
    assert(cachedPC + 1 == PC);
    assert(bit_test(P, 4) == 1);
    assert(bit_test(P, 1) == 1);
    assert(cachedCyclesThisSec + 7 == cyclesThisSec);
    printf("test_BRK test passed!\n");
}