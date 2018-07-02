#include <assert.h>
#include "../src/Utils.h"
#include "TestMemory.h"
#include "TestCPU.h"

int main() {
    testUtils();
    testWrite();
    testRead();
    testIndirectXAddr();
    testOpcodes();
}

/**
 * Tests the utils, we'll put this here for now
 */
void testUtils();

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