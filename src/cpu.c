#include "cpu.h"

// CPU struct
struct cpu {
	// Registers
	byte A;     // Accumulator, deal with carry, overflow and so on...
	byte X;     // General purpose
	byte Y;     // General purpose
	word PC;    // Program Counter
	byte SP;    // Stack Pointer
	byte P;     // Status register, CPU flags
	int cyclesThisSec; //Counter of elapsed cycles (Hz) this current second.
	int speed;  //Speed of the CPU in Hz. Used to slow down the emulation to match the NES's clock speed
};

void cpu_cycle() {
	decOpcode();
	exeOpcode();
}

void decOpcode() {

}

void exeOpcode() {

}

/**BRK causes a non-maskable interrupt and increments the program counter by one.
 * Therefore an RTI will go to the address of the BRK +2 so that BRK may be used to replace a two-byte instruction
 * for debugging and the subsequent RTI will be correct.
 */
void brk() {
	int cycles = 7;
	printf("BRK not implemented");
}

/**
 * Massive function pointer array that holds a call to each opcode. Valid or invalid.
 *
 * gen_opcode_func is a pointer to a void function, that we will cast each time to the correct function.
 * From StackOverflow:
 * You need to pick a function pointer type to use as a "generic function pointer",
 * use that type to define your array, and use explicit casts.
 * Casting one function pointer type to another and then back again is guaranteed to preserve the value.
 */
gen_opcode_func *opcodeFunctions[256] = {
		        //Opcode      Syntax        Description                     Len     Tim
		&brk,   //$00         BRK           BReaK                           1       7
		0,      //$01         ORA ($44, X)  bitwise OR with Accumulator     2       6
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
};
