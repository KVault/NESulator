#include "cpu.h"
#include "memory.h"

/**
 * More info here http://wiki.nesdev.com/w/index.php/CPU_power_up_state
 *
 *  P = $34(00110100)(IRQ disabled)
 *  A, X, Y = 0
 *  S = $FD
 *  $4017 = $00 (frame irq enabled)
 *  $4015 = $00 (all channels disabled)
 *  $4000-$400F = $00 (not sure about $4010-$4013)
 *  All 15 bits of noise channel LFSR = $0000.
 *  The first time the LFSR is clocked from the all-0s state, it will shift in a 1.
 */
void power_up(int clockSpeed) {
	zeroMemory();
	A = X = Y = 0;
	P = 0x34; //00110100
	SP = 0xFD;
	wmem_const(BYTE, 0x4015, 0);
	wmem_const(BYTE, 0x4017, 0);
	wmem_const(16, 0x4000, 0);

	byte tmpFFFC, tmpFFFD;
	rmem(BYTE, 0xFFFC, &tmpFFFC);
	rmem(BYTE, 0xFFFD, &tmpFFFD);

	PC = (word) (tmpFFFD * 256 + tmpFFFC);
	speed = clockSpeed;
	//TODO IRQ stuff
	//TODO LFSR stuff
}

void brk() {
	int cycles = 7;
	PC++;
	bit_set(&P, 1); // Z flag
	bit_set(&P, 4); // B flag
	cyclesThisSec += cycles;
}

void ora(byte *b, int cycles, int pcIncrease) {
	//Do the actual or operation, saving the result in the accumulator
	A = A | *b;
	//Set the flags
	if (A == 0x00) bit_set(&P, 1);
	if (bit_test(A, 7)) bit_set(&P, 7);

	//Update cycles and pc
	cyclesThisSec += cycles;
	PC += pcIncrease;
}

void ora_ind_x() {
	byte param, data;

	rmem(BYTE, PC + 1, &param);
	word addr = indirectx_addr(param);
	rmem(BYTE, addr, &data);

	ora(&data, 6, 2);
}

void ora_ind_y() {
	byte param, data;

	rmem(BYTE, PC + 1, &param);
	word addr = indirecty_addr(param);
	rmem(BYTE, addr, &data);

	// TODO +1 if page crossed
	ora(&data, 5, 2);
}

void ora_zpage() {
	byte param, data;

	rmem(BYTE, PC + 1, &param);
	word addr = zeropage_addr(param);
	rmem(BYTE, addr, &data);

	ora(&data, 2, 2);
}

void ora_zpage_x() {
	byte param, data;

	rmem(BYTE, PC + 1, &param);
	word addr = zeropagex_addr(param);
	rmem(BYTE, addr, &data);

	ora(&data, 4, 2);
}

void ora_immediate() {
	byte data;
	rmem(BYTE, PC + 1, &data);

	ora(&data, 2, 2);
}

void ora_absolute() {
	byte param[2];
	byte data;

	rmem(WORD, PC + 1, param);
	word addr = absolute_addr(to_mem_addr(param));
	rmem(BYTE, addr, &data);

	ora(&data, 4, 3);
}

void ora_absolute_x() {
	byte param[2];
	byte data;

	rmem(WORD, PC + 1, param);
	word addr = absolutex_addr(to_mem_addr(param));
	rmem(BYTE, addr, &data);

	ora(&data, 4, 3);
}

void ora_absolute_y() {
	byte param[2];
	byte data;

	rmem(WORD, PC + 1, param);
	word addr = absolutey_addr(to_mem_addr(param));
	rmem(BYTE, addr, &data);

	ora(&data, 4, 3);
}

/**
 * Massive function pointer array that holds a call to each opcode. Valid or invalid.
 *
 * gen_opcode_func is a pointer to a void function, that we will cast each time to the correct function.
 * From StackOverflow:
 * You need to pick a function pointer type to use as a "generic function pointer",
 * use that type to define your array, and use explicit casts.
 * Casting one function pointer type to another and then back again is guaranteed to preserve the value.
 * https://stackoverflow.com/a/7670827/6265003
 */
gen_opcode_func opcodeFunctions[OPCODE_COUNT] = {
		//                Opcode    Syntax        Description                     Len     Tim
		&brk,           //$00       BRK           BReaK                           1       7
		&ora_ind_x,     //$01       ORA ($44, X)  bitwise OR with Accumulator     2       6
		0,
		0,
		0,
		&ora_zpage,     //$05       ORA $44       bitwise OR with Accumulator     2       3
		&asl_zpage,     //$06       ASL $44       Arithmetic Shift Left           2       5
		0,
		0,
		&ora_immediate, //$09       ORA #$44      bitwise OR with Accumulator     2       2
		0,
		0,
		0,
		&ora_absolute,  //$0D       ORA $4400     bitwise OR with Accumulator     3       4
		0,
		0,
		0,
		&ora_ind_y,     //$11       ORA ($44), Y  bitwise OR with Accumulator     2       6
		0,
		0,
		0,
		&ora_zpage_x,   //$15       ORA ($44), X  bitwise OR with Accumulator     2       4
		0,
		0,
		0,
		&ora_absolute_y,//$19       ORA $4400, Y  bitwise OR with Accumulator     3       4+
		0,
		0,
		0,
		&ora_absolute_x,//$1D       ORA $4400, X  bitwise OR with Accumulator     3       4+
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

void cpu_cycle() {
	decOpcode();
	exeOpcode();
}

void decOpcode() {
	rmem(BYTE, PC, &currentOpcode);
}

void exeOpcode() {
	((gen_opcode_func)opcodeFunctions[currentOpcode])();
}