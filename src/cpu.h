#ifndef NESULATOR_CPU_H
#define NESULATOR_CPU_H

#include "nes.h"
typedef int (*gen_opcode_func)(void);

/**
 * Fetch the next X bytes (TBD) from where the PC is pointing to. It will map it and decide which opcode is
 * to be executed.
 */
void decOpcode();

/**
 * Still TBD exactly how this is going to work, but it will get the decoded opcode and execute whatever
 * it has to do with it.
 */
void exeOpcode();

/**
 * Executes an instruction on the cpu. Internally it would decode the opcode and execute it (for now)
 * In  the future it would also have to call some routine to match the original cpu speed
 */
void cpu_cycle();


void brk();

#endif //NESULATOR_CPU_H
