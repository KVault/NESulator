#ifndef NESULATOR_TESTCPU_H
#define NESULATOR_TESTCPU_H

#include "../src/rom.h"
#include "../src/Utils.h"
#include "../src/cpu.h"
#include "../src/memory.h"
#include <assert.h>

void testOpcodes();

void test_BRK();

void test_ORA();

void test_ASL();

void test_JSR();

void test_PHP();

void test_PHA();

void test_PLA();

void test_PLP();

void test_AND();

void test_BIT();

void test_FLAGS();

void test_REGISTERS();

void test_ADC();

void test_BRANCH();

void test_NOP();

void test_SBC();

void test_INCDECMEM();

void test_LOADREGISTER();

void test_STOREREGISTER();

void test_RTS();

void test_RTI();

void test_COMPAREREGISTER();

void test_LSR();

void test_ROTATE();

void test_EOR();

void test_JMP();

void test_NESTEST();

#endif //NESULATOR_TESTCPU_H
