#ifndef NESULATOR_CPU_H
#define NESULATOR_CPU_H

#include "nes.h"
#include "Utils.h"

typedef int (*gen_opcode_func)(void);

byte A;     // Accumulator, deal with carry, overflow and so on...
byte X;     // General purpose
byte Y;     // General purpose
word PC;    // Program Counter
byte SP;    // Stack Pointer

/*
 * Flags of the status register:
 * The processor status register has 8 bits, where 7 are used as flags: (From 7 to 0)
 * N = negative flag (1 when result is negative)
 * V = overflow flag (1 on signed overflow)
 * # = unused (always 1)
 * B = break flag (1 when interrupt was caused by a BRK)
 * D = decimal flag (1 when CPU in BCD mode)
 * I = IRQ flag (when 1, no interrupts will occur (exceptions are IRQs forced by BRK and NMIs))
 * Z = zero flag (1 when all bits of a result are 0)
 * C = carry flag (1 on unsigned overflow)
 */
byte P;     // Status register, CPU flags
int cyclesThisSec; //Counter of elapsed cycles (Hz) this current second.
int speed;  //Speed of the CPU in Hz. Used to slow down the emulation to match the NES's clock speed
byte currentOpcode; // The opcode of this cycle

/**
 * Initializes a CPU with the specified clock speed. This will set every register and pointer
 * to the right value. Basically it simulates switching on the NES.
 */
void power_up(int clockSpeed);

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
