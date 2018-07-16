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
	for (unsigned int i = 0x4000; i <= 0x400F; ++i) {
		wmem_b(i, 0);
	}

	PC = rmem_w(0xFFFC);
	speed = clockSpeed;
	//TODO IRQ stuff
	//TODO LFSR stuff
}

void nop() {
	PC++;
	cyclesThisSec += 2;
}

void brk() {
	bit_set(&P, flagZ);
	bit_set(&P, flagB);
	cyclesThisSec += 7;
	PC++;
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

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////AND (bitwise AND accumulator) REGION///////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


void and(byte value, int cycles, int pcIncrease) {
	A &= value;
	cyclesThisSec += cycles;
	PC += pcIncrease;
	(bit_test(value, 7)) ? bit_set(&P, flagC) : bit_clear(&P, flagC);

	if (A == 0) {
		bit_set(&P, flagZ);
	}
	if (bit_test(A, 7)) {
		bit_set(&P, flagN);
	}
}

void and_immediate() {
	byte value = rmem_b(PC + 1);
	and(value, 2, 2);
}

void and_zpage() {
	word addr = zeropage_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	and(value, 3, 2);
}

void and_zpage_x() {
	word addr = zeropagex_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	and(value, 4, 2);
}

void and_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	byte value = rmem_b(addr);
	and(value, 4, 3);
}

void and_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	byte value = rmem_b(addr);
	and(value, 4, 3);
	//TODO +1 cycle if page crossed
}

void and_absolute_y() {
	word addr = absolutey_addr(rmem_w(PC + 1));
	byte value = rmem_b(addr);
	and(value, 4, 3);
	//TODO +1 cycle if page crossed
}

void and_indirect_x() {
	word addr = indirectx_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	and(value, 6, 2);
}

void and_indirect_y() {
	word addr = indirecty_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	and(value, 5, 2);
	//TODO +1 cycle if page crossed
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////BIT (BIt Test) REGION//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * BIt Test checks if one or more bits of a memory position are set
 */
void bit(byte value, int cycles, int pcIncrease) {

	byte tmp = A & value;
	(tmp == 0) ? bit_clear(&P, flagZ) : bit_set(&P, flagZ);
	(bit_test(value, 6)) ? bit_set(&P, flagV) : bit_clear(&P, flagV);
	(bit_test(value, 7)) ? bit_set(&P, flagN) : bit_clear(&P, flagN);

	cyclesThisSec += cycles;
	PC += pcIncrease;
}

/**
 * BIt Test checks if one or more bits of a memory position are set
 */
void bit_zpage() {
	word addr = zeropage_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	bit(value, 3, 2);
}

/**
 * BIt Test checks if one or more bits of a memory position are set
 */
void bit_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	byte value = rmem_b(addr);
	bit(value, 4, 3);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Clear flags (CLear flags) REGION///////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * All of this instructions have a length of one byte and require two machine cycles
 */
void set_flag_value(byte flag, int isSet) {
	(isSet) ? bit_set(&P, flag) : bit_clear(&P, flag);//Set or clear the flag depending on isSet
	cyclesThisSec += 2; //Constant. Always
	PC++; //Constant. Always
}

void clc() {
	set_flag_value(flagC, 0);
}

void cld() {
	set_flag_value(flagD, 0);
}

void cli() {
	set_flag_value(flagI, 0);
}

void clv() {
	set_flag_value(flagV, 0);
}

void sec() {
	set_flag_value(flagC, 1);
}

void sei() {
	set_flag_value(flagI, 1);
}

void sed() {
	set_flag_value(flagD, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Registers REGION///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/**
 * All of this instructions have a length of one byte and require two machine cycles
 */
void transfer_reg(byte *from_reg, byte *to_reg) {
	*to_reg = *from_reg;
	PC++;
	cyclesThisSec += 2;
}

/**
 * All of this instructions have a length of one byte and require two machine cycles
 */
void dec_reg(byte *reg) {
	(*reg)--;
	PC++;
	cyclesThisSec += 2;
}

/**
 * All of this instructions have a length of one byte and require two machine cycles
 */
void inc_reg(byte *reg) {
	(*reg)++;
	PC++;
	cyclesThisSec += 2;
}

void tax() {
	transfer_reg(&A, &X);
}

void txa() {
	transfer_reg(&X, &A);
}

void dex() {
	dec_reg(&X);
}

void inx() {
	inc_reg(&X);
}

void tay() {
	transfer_reg(&A, &Y);
}

void tya() {
	transfer_reg(&Y, &A);
}

void dey() {
	dec_reg(&Y);
}

void iny() {
	inc_reg(&Y);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////ADC REGION//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void adc(byte value, int cycles, int pcIncrease) {
	byte carry = (byte) bit_test(P, flagC);
	int result = A + carry + value;
	carry = (byte) ((result & 0x100) >> 8);
	bit_val(&P, flagC, carry);

	// If operands same source sign but different result sign
	int isOverflown = ((A ^ result) & (value ^ result) & 0x80);
	A = (byte) result;

	bit_val(&P, flagZ, A == 0);
	bit_val(&P, flagV, isOverflown);
	bit_val(&P, flagN, bit_test(A, 7));
	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void adc_immediate() {
	byte value = rmem_b(PC + 1);
	adc(value, 2, 2);
}

void adc_zpage() {
	word addr = zeropage_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	adc(value, 3, 2);
}

void adc_zpage_x() {
	word addr = zeropagex_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	adc(value, 4, 2);

}

void adc_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	byte value = rmem_b(addr);
	adc(value, 4, 3);
}

void adc_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	byte value = rmem_b(addr);
	//TODO +1 cycle if page crossed
	adc(value, 4, 3);
}

void adc_absolute_y() {
	word addr = absolutey_addr(rmem_w(PC + 1));
	byte value = rmem_b(addr);
	//TODO +1 cycle if page crossed
	adc(value, 4, 3);
}

void adc_indirect_x() {
	word addr = indirectx_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	adc(value, 6, 2);
}

void adc_indirect_y() {
	word addr = indirecty_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	//TODO +1 cycle if page crossed
	adc(value, 5, 2);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////BRANCH REGION///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void try_branch(byte flag, int req_flag_val) {
	byte value = rmem_b(PC + 1);
	cyclesThisSec += 2;//This is always constant
	PC += 2;
	if (bit_test(P, flag) == req_flag_val) {
		PC += value;
		cyclesThisSec++;
		//TODO +2 cycle if page crossed
	}
}

void bpl() {
	try_branch(flagN, 0);
}

void bmi() {
	try_branch(flagN, 1);
}

void bvc() {
	try_branch(flagV, 0);
}

void bvs() {
	try_branch(flagV, 1);
}

void bcc() {
	try_branch(flagC, 0);
}

void bcs() {
	try_branch(flagC, 1);
}

void bne() {
	try_branch(flagZ, 0);
}

void beq() {
	try_branch(flagZ, 1);
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////SBC REGION//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void sbc(byte value, int cycles, int pcIncrease) {
	byte carry = (byte) bit_test(P, flagC);
	byte carryNop = !carry;
	int result = A - value - carryNop;
	carry = (byte) ((result & 0x100) >> 8);
	bit_val(&P, flagC, carry);

	// If operands same source sign but different result sign
	int isOverflown = ((A ^ result) & (value ^ result) & 0x80);
	A = (byte) result;

	bit_val(&P, flagZ, A == 0);
	bit_val(&P, flagV, !isOverflown); // we need to clear the flagV
	bit_val(&P, flagN, bit_test(A, 7));
	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void sbc_immediate() {
	byte value = rmem_b(PC + 1);
	sbc(value, 2, 2);
}

void sbc_zpage() {
	word addr = zeropage_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	sbc(value, 3, 2);
}

void sbc_zpage_x() {
	word addr = zeropagex_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	sbc(value, 4, 2);

}

void sbc_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	byte value = rmem_b(addr);
	sbc(value, 4, 3);
}

void sbc_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	byte value = rmem_b(addr);
	//TODO +1 cycle if page crossed
	sbc(value, 4, 3);
}

void sbc_absolute_y() {
	word addr = absolutey_addr(rmem_w(PC + 1));
	byte value = rmem_b(addr);
	//TODO +1 cycle if page crossed
	sbc(value, 4, 3);
}

void sbc_indirect_x() {
	word addr = indirectx_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	sbc(value, 6, 2);
}

void sbc_indirect_y() {
	word addr = indirecty_addr(rmem_b(PC + 1));
	byte value = rmem_b(addr);
	//TODO +1 cycle if page crossed
	sbc(value, 5, 2);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////MEMORY INC/DEC REGION//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/**
 * Applies the delta to the content of the memory address. Sets the Z and N flags as appropiate
 */
void delta_memory(word memAddr, int delta, int cycles, int pcIncrease) {
	byte value = rmem_b(memAddr);
	value += delta;
	wmem_w(memAddr, value);

	bit_val(&P, flagZ, value == 0);
	bit_val(&P, flagN, bit_test(value, 7));

	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void inc_mem(word memAddr, int cycles, int pcIncrease) {
	delta_memory(memAddr, 1, cycles, pcIncrease);
}

void dec_mem(word memAddr, int cycles, int pcIncrease) {
	delta_memory(memAddr, -1, cycles, pcIncrease);
}

void inc_mem_zpage() {
	word addr = zeropage_addr(rmem_b(PC + 1));
	inc_mem(addr, 5, 2);
}

void inc_mem_zpage_x() {
	word addr = zeropagex_addr(rmem_b(PC + 1));
	inc_mem(addr, 6, 2);
}

void inc_mem_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	inc_mem(addr, 6, 3);
}

void inc_mem_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	inc_mem(addr, 7, 3);
}

void dec_mem_zpage() {
	word addr = zeropage_addr(rmem_b(PC + 1));
	dec_mem(addr, 5, 2);
}

void dec_mem_zpage_x() {
	word addr = zeropagex_addr(rmem_b(PC + 1));
	dec_mem(addr, 6, 2);
}

void dec_mem_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	dec_mem(addr, 6, 3);
}

void dec_mem_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	dec_mem(addr, 7, 3);
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////LOAD REGISTERS INC/DEC REGION///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void load_register(byte *regPtr, byte value, int cycles, int pcIncrease) {
	*regPtr = value;

	bit_val(&P, flagZ, *regPtr == 0);
	bit_val(&P, flagN, bit_test(*regPtr, 7));

	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void lda_inmediate() {
	load_register(&A, rmem_b(PC + 1), 2, 2);
}

void lda_zpage() {
	word addr = zeropage_addr(rmem_b(PC + 1));
	load_register(&A, rmem_b(addr), 3, 2);
}

void lda_zpage_x() {
	word addr = zeropagex_addr(rmem_b(PC + 1));
	load_register(&A, rmem_b(addr), 4, 2);
}

void lda_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	load_register(&A, rmem_b(addr), 4, 3);
}

void lda_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	load_register(&A, rmem_b(addr), 4, 3);
	// TODO +1 if page crossed
}

void lda_absolute_y() {
	word addr = absolutey_addr(rmem_w(PC + 1));
	load_register(&A, rmem_b(addr), 4, 3);
	// TODO +1 if page crossed
}

void lda_indirect_x() {
	word addr = indirectx_addr(rmem_b(PC + 1));
	load_register(&A, rmem_b(addr), 6, 2);
}

void lda_indirect_y() {
	word addr = indirecty_addr(rmem_b(PC + 1));
	load_register(&A, rmem_b(addr), 5, 2);
	// TODO +1 if page crossed
}

void ldx_immediate() {
	load_register(&X, rmem_b(PC + 1), 2, 2);
}

void ldx_zpage() {
	word addr = zeropage_addr(rmem_b(PC + 1));
	load_register(&X, rmem_b(addr), 3, 2);
}

void ldx_zpage_y() {
	word addr = zeropagey_addr(rmem_b(PC + 1));
	load_register(&X, rmem_b(addr), 4, 2);
}

void ldx_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	load_register(&X, rmem_b(addr), 4, 3);
}

void ldx_absolute_y() {
	word addr = absolutey_addr(rmem_w(PC + 1));
	load_register(&X, rmem_b(addr), 4, 3);
	// TODO +1 if page crossed
}

void ldy_immediate() {
	load_register(&Y, rmem_b(PC + 1), 2, 2);
}

void ldy_zpage() {
	word addr = zeropage_addr(rmem_b(PC + 1));
	load_register(&Y, rmem_b(addr), 3, 2);
}

void ldy_zpage_x() {
	word addr = zeropagex_addr(rmem_b(PC + 1));
	load_register(&Y, rmem_b(addr), 4, 2);
}

void ldy_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	load_register(&Y, rmem_b(addr), 4, 3);
}

void ldy_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	load_register(&Y, rmem_b(addr), 4, 3);
	// TODO +1 if page crossed
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
		//              Opcode      Syntax        Description                     Len     Tim
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
		&bpl,           //$10       BPL           Branch if plus                  2       2(+2)
		&ora_ind_y,     //$11       ORA ($44), Y  bitwise OR with Accumulator     2       6
		0,
		0,
		0,
		&ora_zpage_x,   //$15       ORA ($44), X  bitwise OR with Accumulator     2       4
		&asl_zpage_x,   //$16       ASL $44, X    Arithmetic Shift Left           2       6
		0,
		&clc,           //$18       CLC           CLear Carry flag                1       2
		&ora_absolute_y,//$19       ORA $4400, Y  bitwise OR with Accumulator     3       4+
		0,
		0,
		0,
		&ora_absolute_x,//$1D       ORA $4400, X  bitwise OR with Accumulator     3       4+
		&asl_absolute_x,//$1E       ASL $4400, X  Arithmetic Shift Left           3       7
		0,
		&jsr_absolute,  //$20       JSR $4400     Jump to SubRoutine              3       6
		&and_indirect_x,//$21       AND ($44, X)  bitwise AND with accumulator    2       6
		0,
		0,
		&bit_zpage,     //$24       BIT $44       BIt Test                        2       3
		&and_zpage,     //$25       AND $44       bitwise AND with accumulator    2       3
		0,
		0,
		&plp,           //$28       PLP           PuLl to status                  1       4
		&and_immediate, //$29       AND #$44      bitwise AND with accumulator    2       2
		0,
		0,
		&bit_absolute,  //$2C       BIT $4400     BIt Test                        3       4
		&and_absolute,  //$2D       AND $4400     bitwise AND with accumulator    3       4
		0,
		0,
		&bmi,           //$30       BPL           Branch if minus                 2       2(+2)
		&and_indirect_y,//$31       AND ($44), Y  bitwise AND with accumulator    2       5+
		0,
		0,
		0,
		&and_zpage_x,   //$35       AND $44, X    bitwise AND with accumulator    2       4
		0,
		0,
		&sec,           //$38       SEC           Sets Carry flag                 1       2
		&and_absolute_y,//$39       AND $4400, Y  bitwise AND with accumulator    3       4+
		0,
		0,
		0,
		&and_absolute_x,//$3D       AND $4400, X  bitwise AND with accumulator    3       4+
		0,
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
		&bvc,           //$50       BVC           Branch if Overflow Clear        2       2(+2)
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		&cli,           //$58       CLI           CLear Interrupt flag                1       2,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		&adc_indirect_x, //$61      ADC ($44, X) ADd with Carry                    2       6
		0,
		0,
		0,
		&adc_zpage,     //$65       ADC $44      ADd with Carry                    2       3
		0,
		0,
		&pla,           //$68       PLA           PuLl Acumulator                  1       4
		&adc_immediate, //$69       ADC #$44      ADd with Carry                   2       2
		0,
		0,
		0,
		&adc_absolute,  //$6D       ADC $4400    ADd with Carry                    3       4
		0,
		0,
		&bvs,           //$70      BVS           Branch if plus                  2       2(+2)
		&adc_indirect_y,//$71      ADC ($44), X   ADd with Carry                   2       5+
		0,
		0,
		0,
		&adc_zpage_x,   //$75       ADC $44, X    ADd with Carry                    2       4
		0,
		0,
		&sei,           //$78       SEI           Sets Interrupt flag               1       2,
		&adc_absolute_y,//$79       ADC $4400, Y  ADd with Carry                    3       4+
		0,
		0,
		0,
		&adc_absolute_x,//$7D       ADC $4400, X  ADd with Carry                    3       4+
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		&dey,           //$88       DEY           Decrements Y                      1       2
		0,
		&txa,           //$8A       TXA           Transfer X to A                   1       2
		0,
		0,
		0,
		0,
		0,
		&bcc,           //$90       BCC           Branch if carry clear             2       2(+2)
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		&tya,           //$98       TYA           Transfer Y to A                   1       2
		0,
		0,
		0,
		0,
		0,
		0,
		0, &ldy_immediate, //A0       LDA #$44       LoaD Accumulator                  2       2
		&lda_indirect_x,//A1       LDA ($44,X)    LoaD Accumulator                  2       6
		&ldx_immediate, //A2       LDA #$44       LoaD Accumulator                  2       2
		0,
		&ldy_zpage,     //$A4       LDA $44       LoaD Accumulator                  2       3
		&lda_zpage,     //$A5       LDA $44       LoaD Accumulator                  2       3
		&ldx_zpage,     //$A6       LDA $44       LoaD Accumulator                  2       3
		0,
		&tay,           //$A8       TAY           Transfer A to Y                   1       2
		&lda_inmediate, //$A9       LDA #$44      LoaD Accumulator                  2       2
		&tax,           //$AA       TAX           Transfer A to X                   1       2
		0,
		&ldy_absolute,  //$A4      LDY $4400     LoaD Y Register                    3       4
		&lda_absolute,  //AD       LDA $4400     LoaD Accumulator                   3       4
		&ldx_absolute,  //AE       LDA $4400     LoaD Accumulator                   3       4
		0,
		&bcs,           //$B0       BCS           Branch id carry set               2       2(+2)
		&lda_indirect_y,//B1        LDA ($44),Y   LoaD Accumulator                  2       5+
		0,
		0,
		&ldy_zpage_x,   //$B4       LDY $44, X    LoaD Y Register                    2       6
		&lda_zpage_x,   //$B5       LDA $44, X    LoaD Accumulator                   2       4
		&ldx_zpage_y,   //$B6       LDX $44, Y    LoaD Accumulator                   2       4
		0,
		&clv,           //$B8       CLV           CLear Overflow flag                1       2,
		&lda_absolute_y,//$B9       LDA $4400,Y   LoaD Accumulator                   3       4+
		0,
		0,
		&ldy_absolute_x,//$A4      LDY $4400,Y    LoaD Y Register                    3       4+
		&lda_absolute_x,//BD       LDA $4400,X    LoaD Accumulator                   3       4+
		&ldx_absolute_y,//BE       LDA $4400,Y    LoaD Accumulator                   3       4+
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		&dec_mem_zpage, //$C6       DEC $44       DEcrement Memory                  2       5
		0,
		&iny,           //$C8       INY           Increments Y                      1       2
		0,
		&dex,           //$CA       DEX           Decrements X register             1       2
		0,
		0,
		0,
		&dec_mem_absolute,//$CE     DEC $4400    DEcrement Memory                   3       6
		0,
		&bne,           //$D0       BNE           Branch now equals                 2       2(+2)
		0,
		0,
		0,
		0,
		0,
		&dec_mem_zpage_x,//$D6      DEC $44,X     DEcrement Memory                  2       6
		0,
		&cld,           //$D8       CLD           CLear Decimal flag                1       2
		0,
		0,
		0,
		0,
		0,
		&dec_mem_absolute_x,//$DE   DEC $4400,X   DEcrement Memory                 3       7
		0,
		0,
		&sbc_indirect_x,//$E1       SBC ($44,X)   SuBstract with Carry             2       6
		0,
		0,
		0,
		&sbc_zpage,     //$E5       SBC $44       SuBstract with Carry             2       3
		&inc_mem_zpage, //$E6       INC $44       INcrement Memory                 2       5
		0,
		&inx,           //$E8       INX           Increments X register            1       7
		&sbc_immediate, //$E9       SBC #$44      SuBstract with Carry             2       2
		&nop,           //$EA       NOP           No OPeration                     1       2
		0,
		0,
		&sbc_absolute, //$ED       SBC $4400       SuBstract with Carry            3       4
		&inc_mem_absolute,//$EE    INC $4400       INcrement Memory                3       6
		0,
		&beq,           //$F0       BEQ           Branch if equals                 2       2(+2)
		&sbc_indirect_y,//$F1       SBC ($44),Y   SuBstract with Carry             2       5+
		0,
		0,
		0,
		&sbc_zpage_x,   //$F5       SBC $44,X     SuBstract with Carry             2       4
		&inc_mem_zpage_x,//$F6      INC $44, X    INcrement Memory                 2       6
		0,
		&sed,           //$F8       SED           Sets Decimal flag                1       2
		&sbc_absolute_y,//$F9       SBC $4400,Y   SuBstract with Carry             3       4+
		0,
		0,
		0,
		&sbc_absolute_x,//$FD       SBC $4400,X   SuBstract with Carry             3       4+
		&inc_mem_absolute_x,//$FE     INC $4400,X   INcrement Memory                 3       7
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
