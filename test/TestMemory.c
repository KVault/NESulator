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
    byte data = 'd';
    wmem(BYTE, 0, &data);
    assert(memoryBank[0] == 'd');

    zeroMemory();

    //Test two byte write on random positions
    assert(memoryBank[512] == 0);
    assert(memoryBank[513] == 0);
    assert(memoryBank[514] == 0);
    byte *nesText = "NES";
    wmem(3, 512, nesText);

    assert(memoryBank[512] == 'N');
    assert(memoryBank[513] == 'E');
    assert(memoryBank[514] == 'S');

    //Test the memory write repeat
    zeroMemory();
    assert(memoryBank[512] == 0);
    assert(memoryBank[513] == 0);
    wmem_const(WORD, 512, 'S');
    assert(memoryBank[512] == 'S');
    assert(memoryBank[513] == 'S');


    printf("Write memory test passed!\n");
}

/**
 * Tests how the system reads from memory. Because the write test has passed all the asserts before this one
 * We can assume it works fine so we'll use wmem to ease out the writing
 */
void testRead() {
    zeroMemory();

    //Test one byte read
    //Test the empty values
    byte readSingleByte[1];
    rmem(BYTE, 0, readSingleByte);
    assert(readSingleByte[0] == 0);
    //Fill in the test data
    byte data = 'd';
    wmem(BYTE, 0, &data);
    //Make sure the test data is in there
    rmem(BYTE, 0, readSingleByte);
    assert(readSingleByte[0] == 'd');

    //Test three bytes read
    //Test the empty values
    zeroMemory();
    byte readMultipleBytes[3];
    rmem(3, 512, readMultipleBytes);
    assert(readMultipleBytes[0] == 0);
    assert(readMultipleBytes[1] == 0);
    assert(readMultipleBytes[2] == 0);
    //Fill in the test data
    byte *nesData = "NES";
    wmem(3, 512, nesData);
    //Make sure the test data is in there
    rmem(3, 512, readMultipleBytes);
    assert(readMultipleBytes[0] == 'N');
    assert(readMultipleBytes[1] == 'E');
    assert(readMultipleBytes[2] == 'S');

    printf("Read memory test passed!\n");
}

/**
 * Tests how the indexed indirect mode works
 */
void testIndirectXAddr() {
    word addr = 0;
    // First test
    zeroMemory();
    wmem_const(BYTE, 112, 0x30);
    wmem_const(BYTE, 113, 0xAC);

    X = 100;
    addr = indirectx_addr(12);
    assert(addr == 0xAC30);

    // Second test. Example got from here http://www.emulator101.com/6502-addressing-modes.html
    zeroMemory();
    wmem_const(BYTE, 126, 0x74);
    wmem_const(BYTE, 127, 0x20);

    X = 100;
    addr = indirectx_addr(26);
    assert(addr == 0x2074);

    printf("Test IndirectXAddr passed!\n");
}

void testIndirectYAddr() {
    //First test. Example got from here http://www.emulator101.com/6502-addressing-modes.html
    zeroMemory();
    wmem_const(BYTE, 0x86, 0x28);
    wmem_const(BYTE, 0x87, 0x40);
    Y = 0x10;
    word addr = indirecty_addr(0x86);

    assert(addr == 0x4038);

    printf("Test IndirectYAddr passed!\n");
}

void testZeroPageAddr() {
    //First test. http://www.emulator101.com/6502-addressing-modes.html
    zeroMemory();
    word addr = 0;

    // zeropagex_addr test
    X = 0x60;
    addr = zeropagex_addr(0xC0);
    assert(0x0020 == addr);
    printf("Test ZeroPageXAddr passed!\n");

    // zeropagey_addr test
    Y = 0x10;
    addr = zeropagey_addr(0xFB);
    assert(0x000B == addr);
    printf("Test ZeroPageYAddr passed!\n");

    // zeropage_addr test
    addr = zeropage_addr(0x6969);
    assert(0x0069 == addr);
    printf("Test ZeroPageAddr passed!\n");
}

void testAbsoluteAddr() {
    //First test. Absolute. I know, silly but necessary
    zeroMemory();
    word addr;
    byte param[2] = {0x69, 0x69};
    addr = absolute_addr(&param[0]);
    assert(addr == 0x6969);

    //Second test. Absolute X
    zeroMemory();
    byte param2[2] = {0x69, 0x69};
    X = 0x10;
    addr = absolutex_addr(&param2[0]);
    assert(addr == 0x6969);

    //Third test. Absolute Y
    zeroMemory();
    byte param3[2] = {0x69, 0x69};
    Y = 0x20;
    addr = absolutey_addr(&param3[0]);
    assert(addr == 0x6969);

    printf("Test AbsoluteAddr passed! \n");
}
