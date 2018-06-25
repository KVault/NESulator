#include "cpu.h"

// CPU struct
struct cpu {
	// Registers
	byte A;     // Accumulator, deal with carry, overflow and so on...
	byte X;     // General purpose
	byte Y;     // General purpose
	nibble PC;  // Program Counter
	byte SP;    // Stack Pointer
	byte P;     // Status register, CPU flags

	
};

void cpu_cycle() {
	decOpcode();
	exeOpcode();
}

void decOpcode() {

}

void exeOpcode() {

}
