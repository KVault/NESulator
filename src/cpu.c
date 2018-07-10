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
	wmem_b(0x4015, 0);
	wmem_b(0x4017, 0);
	for (int i = 0x4000; i <= 0x400F; ++i) {
		wmem_b(i, 0);
	}

	PC = rmem_w(0xFFFC);
	speed = clockSpeed;
	//TODO IRQ stuff
	//TODO LFSR stuff
}

void brk() {
	int cycles = 7;
	PC++;
	bit_set(&P, flagZ);
	bit_set(&P, flagB);
	cyclesThisSec += cycles;
}

//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////ORA REGION////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * This is the one actually doing the magic. The result of the operation will be saved in A and after doing that
 * if the acumulator is zero, then we have to set the FlagZ to one. If the acumulator is negative then we set
 * the flagN to one.
 */
void ora(byte b, int cycles, int pcIncrease) {
	//Do the actual or operation, saving the result in the accumulator
	A = A | b;
	//Set the flags
	if (A == 0x00) bit_set(&P, flagZ);
	if (bit_test(A, 7)) bit_set(&P, flagN);

	//Update cycles and pc
	cyclesThisSec += cycles;
	PC += pcIncrease;
}

void ora_ind_x() {
	word addr = indirectx_addr(rmem_b(PC + 1));
	ora(rmem_b(addr), 6, 2);
}

void ora_ind_y() {
	word addr = indirecty_addr(rmem_b(PC + 1));
	// TODO +1 if page crossed
	ora(rmem_b(addr), 5, 2);
}

void ora_zpage() {
	word addr = zeropage_addr(rmem_b(PC + 1));
	ora(rmem_b(addr), 2, 2);
}

void ora_zpage_x() {
	word addr = zeropagex_addr(rmem_b(PC + 1));
	ora(rmem_b(addr), 4, 2);
}

void ora_immediate() {
	ora(rmem_b(PC + 1), 2, 2);
}

void ora_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	ora(rmem_b(addr), 4, 3);
}

void ora_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	ora(rmem_b(addr), 4, 3);
}

void ora_absolute_y() {
	word addr = absolutey_addr(rmem_w(PC + 1));
	ora(rmem_b(addr), 4, 3);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////ASL (Arithmetic Shift Left) REGION/////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * The one really doing the hard work here. First, if the most significant bit is set, then it would overflow after
 * the shifting, so we set the carry flag.
 *
 * if the number is negative AFTER the shifting the we set the negative flag.
 */
void asl(byte *b, int cycles, int pcIncrease) {
	if (bit_test(*b, flagN)) {
		bit_set(&P, flagC);
	}

	byte shifted = *b << 1;
	if (bit_test(shifted, 7)) {
		bit_set(&P, flagN);
	}
	if (A == 0) {
		bit_set(&P, flagZ);
	}

	*b = shifted;
	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void asl_zpage() {
	word addr = zeropage_addr(rmem_b(PC + 1));
	byte data = rmem_b(addr);
	asl(&data, 5, 2);
	wmem_b(addr, data);
}

void asl_accumulator() {
	asl(&A, 2, 1);
}

void asl_zpage_x() {
	word addr = zeropagex_addr(rmem_b(PC + 1));
	byte data = rmem_b(addr);
	asl(&data, 6, 2);
	wmem_b(addr, data);
}

void asl_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	byte data = rmem_b(addr);
	asl(&data, 6, 3);
	wmem_b(addr, data);
}


void asl_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	byte data = rmem_b(addr);
	asl(&data, 7, 3);
	wmem_w(addr, data);
}


//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////JSR (Jump to SubRoutine) REGION////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void jsr_absolute() {
	byte param[2];
	word cachedPC = (word) (PC - 0x01);
	word addr = absolute_addr(rmem_w(PC + 1));
	push_w(cachedPC);
	PC = addr;
	cyclesThisSec += 6;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PHP (PusH Processor status) REGION/////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void php() {
	push_b(P);
	PC++;
	cyclesThisSec += 3;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PLP (PuLl Processor status) REGION/////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void plp() {
	P = pop_b();
	PC++;
	cyclesThisSec += 4;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PHA (PusH Acumulator) REGION///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void pha() {
	push_b(A);
	PC++;
	cyclesThisSec += 3;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PLA (PuLl Acumulator) REGION///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void pla() {
	A = pop_b();
	if (A == 0) {
		bit_set(&P, flagZ);
	}
	if (bit_test(A, 7)) {
		bit_set(&P, flagN);
	}
	PC++;
	cyclesThisSec += 4;
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
		&php,           //$08       PHP           PusH Processor status           1       3
		&ora_immediate, //$09       ORA #$44      bitwise OR with Accumulator     2       2
		&asl_accumulator,//$0A      ASL A         Arithmetic Shift Left           1       2
		0,
		0,
		&ora_absolute,  //$0D       ORA $4400     bitwise OR with Accumulator     3       4
		&asl_absolute,  //$0E       ASL $4400     Arithmetic Shift Left           3       6
		0,
		0,
		&ora_ind_y,     //$11       ORA ($44), Y  bitwise OR with Accumulator     2       6
		0,
		0,
		0,
		&ora_zpage_x,   //$15       ORA ($44), X  bitwise OR with Accumulator     2       4
		&asl_zpage_x,   //$16       ASL $44, X    Arithmetic Shift Left           2       6
		0,
		0,
		&ora_absolute_y,//$19       ORA $4400, Y  bitwise OR with Accumulator     3       4+
		0,
		0,
		0,
		&ora_absolute_x,//$1D       ORA $4400, X  bitwise OR with Accumulator     3       4+
		&asl_absolute_x,//$1E       ASL $4400, X  Arithmetic Shift Left           3       7
		0,
		&jsr_absolute,  //$20       JSR $4400     Jump to SubRoutine              3       6
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		&plp,           //$28       PLP           PuLl to status                  1       4
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		&pha,           //$48       PHA           PusH Acumulator                  1       3
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		&pla,           //$68       PLA           PuLl Acumulator                  1       4
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
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
	currentOpcode = rmem_b(PC);
}

void exeOpcode() {
	((gen_opcode_func) opcodeFunctions[currentOpcode])();
}
