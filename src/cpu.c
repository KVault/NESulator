#include "cpu.h"
#include "nes.h"

// CPU struct
struct cpu {
	// Registers
	BYTE A;     // Accumulator, deal with carry, overflow and so on...
	BYTE X;     // General purpose
	BYTE Y;     // General purpose
	NIBBLE PC;  // Program Counter
	BYTE SP;    // Stack Pointer
	BYTE P;     // Status register, CPU flags
};

void cpu_cycle() {
	decOpcode();
	exeOpcode();
}

void decOpcode() {

}

void exeOpcode() {

}
