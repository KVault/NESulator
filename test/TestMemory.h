#ifndef NESULATOR_TESTMEMORY_H
#define NESULATOR_TESTMEMORY_H

#include "../src/memory.h"
#include <stdlib.h>

void testWrite();

void testRead();

void testIndirectXAddr();

void testIndirectYAddr();

/**
 * Includes the tests for three modes. Zero Page X, Zero Page Y and Zero Page.
 */
void testZeroPageAddr();

/**
 * Includes the tests for three modes. Absolute X, Absolute Y and Zero Page.
 */
void testAbsoluteAddr();

#endif //NESULATOR_TESTMEMORY_H
