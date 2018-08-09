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
	for (uint i = 0x4000; i <= 0x400F; ++i) {
		wmem_b(i, 0);
	}

	speed = clockSpeed;
	//TODO IRQ stuff
	//TODO LFSR stuff
}

void resetPC(){
	PC = rmem_w(0xFFFC);
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
	bit_val(&P, flagZ, A == 0x00);
	bit_val(&P, flagN, bit_test(A, 7));

	//Update cycles and pc
	cyclesThisSec += cycles;
	PC += pcIncrease;
}

void ora_ind_x() {
	ora(indirectx_param(), 6, 2);
}

void ora_ind_y() {
	ora(indirecty_param(), 5, 2);
	// TODO +1 if page crossed
}

void ora_zpage() {
	ora(zpage_param(), 2, 2);
}

void ora_zpage_x() {
	ora(zpagex_param(), 4, 2);
}

void ora_immediate() {
	ora(rmem_b(PC + 1), 2, 2);
}

void ora_absolute() {
	ora(absolute_param(), 4, 3);
}

void ora_absolute_x() {
	ora(absolutex_param(), 4, 3);
}

void ora_absolute_y() {
	ora(absolutey_param(), 4, 3);
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
	bit_val(&P, flagC, bit_test(*b, flagN));

	byte shifted = *b << 1;
	bit_val(&P, flagN, bit_test(shifted, 7));
	bit_val(&P, flagZ, A == 0);


	*b = shifted;
	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void asl_zpage() {
	word addr = zpage_addr(rmem_b(PC + 1));
	byte data = zpage_param();
	asl(&data, 5, 2);
	wmem_b(addr, data);
}

void asl_accumulator() {
	asl(&A, 2, 1);
}

void asl_zpage_x() {
	word addr = zpagex_addr(rmem_b(PC + 1));
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
	word cachedPC = (word) (PC + 0x02);
	word addr = absolute_addr(rmem_w(PC + 1));
	push_w(cachedPC); // Stores the address of the next opcode minus one
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
	bit_val(&P, flagZ, A == 0);
	bit_val(&P, flagN, bit_test(A, 7));
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
	bit_val(&P, flagC, bit_test(value, 7));
	bit_val(&P, flagZ, A == 0);
	bit_val(&P, flagN, bit_test(A, 7));
}

void and_immediate() {
	byte value = rmem_b(PC + 1);
	and(value, 2, 2);
}

void and_zpage() {
	and(zpage_param(), 3, 2);
}

void and_zpage_x() {
	and(zpagex_param(), 4, 2);
}

void and_absolute() {
	and(absolute_param(), 4, 3);
}

void and_absolute_x() {
	and(absolutex_param(), 4, 3);
	//TODO +1 cycle if page crossed
}

void and_absolute_y() {
	and(absolutey_param(), 4, 3);
	//TODO +1 cycle if page crossed
}

void and_indirect_x() {
	and(indirectx_param(), 6, 2);
}

void and_indirect_y() {
	and(indirecty_param(), 5, 2);
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
	bit_val(&P, flagZ, tmp != 0);
	bit_val(&P, flagV, bit_test(value, 6));
	bit_val(&P, flagN, bit_test(value, 7));

	cyclesThisSec += cycles;
	PC += pcIncrease;
}

/**
 * BIt Test checks if one or more bits of a memory position are set
 */
void bit_zpage() {
	bit(zpage_param(), 3, 2);
}

/**
 * BIt Test checks if one or more bits of a memory position are set
 */
void bit_absolute() {
	bit(absolute_param(), 4, 3);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Clear flags (CLear flags) REGION///////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * All of this instructions have a length of one byte and require two machine cycles
 */
void set_flag_value(byte flag, int isSet) {
	bit_val(&P, flag, isSet);
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
	adc(zpage_param(), 3, 2);
}

void adc_zpage_x() {
	adc(zpagex_param(), 4, 2);

}

void adc_absolute() {
	adc(absolute_param(), 4, 3);
}

void adc_absolute_x() {
	adc(absolutex_param(), 4, 3);
	//TODO +1 cycle if page crossed
}

void adc_absolute_y() {
	adc(absolutey_param(), 4, 3);
	//TODO +1 cycle if page crossed
}

void adc_indirect_x() {
	adc(indirectx_param(), 6, 2);
}

void adc_indirect_y() {
	adc(indirecty_param(), 5, 2);
	//TODO +1 cycle if page crossed
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
	byte carryNop = (byte) !carry;
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
	sbc(zpage_param(), 3, 2);
}

void sbc_zpage_x() {
	sbc(zpagex_param(), 4, 2);

}

void sbc_absolute() {
	sbc(absolute_param(), 4, 3);
}

void sbc_absolute_x() {
	//TODO +1 cycle if page crossed
	sbc(absolutex_param(), 4, 3);
}

void sbc_absolute_y() {
	//TODO +1 cycle if page crossed
	sbc(absolutey_param(), 4, 3);
}

void sbc_indirect_x() {
	sbc(indirectx_param(), 6, 2);
}

void sbc_indirect_y() {
	//TODO +1 cycle if page crossed
	sbc(indirecty_param(), 5, 2);
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
	word addr = zpage_addr(rmem_b(PC + 1));
	inc_mem(addr, 5, 2);
}

void inc_mem_zpage_x() {
	word addr = zpagex_addr(rmem_b(PC + 1));
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
	word addr = zpage_addr(rmem_b(PC + 1));
	dec_mem(addr, 5, 2);
}

void dec_mem_zpage_x() {
	word addr = zpagex_addr(rmem_b(PC + 1));
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
	load_register(&A, zpage_param(), 3, 2);
}

void lda_zpage_x() {
	load_register(&A, zpagex_param(), 4, 2);
}

void lda_absolute() {
	load_register(&A, absolute_param(), 4, 3);
}

void lda_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	load_register(&A, rmem_b(addr), 4, 3);
	// TODO +1 if page crossed
}

void lda_absolute_y() {
	load_register(&A, absolutey_param(), 4, 3);
	// TODO +1 if page crossed
}

void lda_indirect_x() {
	load_register(&A, absolutex_param(), 6, 2);
}

void lda_indirect_y() {
	load_register(&A, indirecty_param(), 5, 2);
	// TODO +1 if page crossed
}

void ldx_immediate() {
	load_register(&X, rmem_b(PC + 1), 2, 2);
}

void ldx_zpage() {
	load_register(&X, zpage_param(), 3, 2);
}

void ldx_zpage_y() {
	load_register(&X, zpagey_param(), 4, 2);
}

void ldx_absolute() {
	load_register(&X, absolute_param(), 4, 3);
}

void ldx_absolute_y() {
	load_register(&X, absolutey_param(), 4, 3);
	// TODO +1 if page crossed
}

void ldy_immediate() {
	load_register(&Y, rmem_b(PC + 1), 2, 2);
}

void ldy_zpage() {
	load_register(&Y, zpage_param(), 3, 2);
}

void ldy_zpage_x() {
	load_register(&Y, zpagex_param(), 4, 2);
}

void ldy_absolute() {
	load_register(&Y, absolute_param(), 4, 3);
}

void ldy_absolute_x() {
	load_register(&Y, absolutex_param(), 4, 3);
	// TODO +1 if page crossed
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////STORE REGISTERS REGION///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/**
 * Store the specified value onto the pointed register
 */
void store_register(byte reg, word memAddr, int cycles, int pcIncrease) {
	wmem_b(memAddr, reg);
	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void sta_zpage() {
	store_register(A, zpage_addr(rmem_b(PC + 1)), 3, 2);
}

void sta_zpage_x() {
	store_register(A, zpagex_addr(rmem_b(PC + 1)), 4, 2);
}

void sta_absolute() {
	store_register(A, absolute_addr(rmem_b(PC + 1)), 4, 3);
}

void sta_absolute_x() {
	store_register(A, absolutex_addr(rmem_b(PC + 1)), 5, 3);
}

void sta_absolute_y() {
	store_register(A, absolutey_addr(rmem_b(PC + 1)), 5, 3);
}

void sta_indirect_x() {
	store_register(A, indirectx_addr(rmem_b(PC + 1)), 6, 2);
}

void sta_indirect_y() {
	store_register(A, indirecty_addr(rmem_b(PC + 1)), 6, 2);
}

void stx_zpage() {
	store_register(X, zpagex_addr(rmem_b(PC + 1)), 3, 2);
}

void stx_zpage_y() {
	store_register(X, zpagey_addr(rmem_b(PC + 1)), 4, 2);
}

void stx_absolute() {
	store_register(X, absolute_addr(rmem_b(PC + 1)), 4, 3);
}

void sty_zpage() {
	store_register(Y, zpage_addr(rmem_b(PC + 1)), 3, 2);
}

void sty_zpage_x() {
	store_register(Y, zpagex_addr(rmem_b(PC + 1)), 4, 2);
}

void sty_absolute() {
	store_register(Y, absolute_addr(rmem_b(PC + 1)), 4, 3);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////ReTurn from Interrupt REGION////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void rts() {
	PC = pop_w();
	PC++; // JSR pushes the address -1, so when we recover (here) we have to add 1 to make up for that "1" lost
	cyclesThisSec += 6;
}

void rti() {
	P = pop_b();
	PC = pop_w(); //Unlike RTS. RTI pulls the correct PC address. No need to increment
	cyclesThisSec += 6;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////COMPARE REGISTERS REGION///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void compare_register(byte *regPtr, byte value, int cycles, int pcIncrease) {
	bit_val(&P, flagC, *regPtr >= value);
	bit_val(&P, flagN, *regPtr > value);
	bit_val(&P, flagZ, *regPtr == value);

	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void cmp_inmediate() {
	compare_register(&A, rmem_b(PC + 1), 2, 2);
}

void cmp_zpage() {
	compare_register(&A, zpage_param(), 3, 2);
}

void cmp_zpage_x() {
	compare_register(&A, zpagex_param(), 4, 2);
}

void cmp_absolute() {
	compare_register(&A, absolute_param(), 4, 3);
}

void cmp_absolute_x() {
	compare_register(&A, absolutex_param(), 4, 3);
	//TODO +1 if page crossed
}

void cmp_absolute_y() {
	compare_register(&A, absolutey_param(), 4, 3);
	//TODO +1 if page crossed
}

void cmp_indirect_x() {
	compare_register(&A, indirectx_param(), 6, 2);
}

void cmp_indirect_y() {
	compare_register(&A, indirecty_param(), 5, 2);
	//TODO +1 if page crossed
}

void cpx_immediate() {
	compare_register(&X, rmem_b(PC + 1), 2, 2);
}

void cpx_zpage() {
	compare_register(&X, zpage_param(), 3, 2);
}

void cpx_absolute() {
	compare_register(&X, absolute_param(), 4, 3);
}

void cpy_immediate() {
	compare_register(&Y, rmem_b(PC + 1), 2, 2);
}

void cpy_zpage() {
	compare_register(&Y, zpage_param(), 2, 3);
}

void cpy_absolute() {
	compare_register(&Y, absolute_param(), 3, 4);
}


//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////LSR (Logical Shift Right) REGION///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void lsr(byte *value, int cycles, int pcIncrease) {

	bit_val(&P, flagC, bit_test(*value, 0));

	byte shifted = *value >> 1;

	bit_val(&P, flagZ, shifted == 0);
	*value = shifted;

	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void lsr_zpage() {
	word addr = zpage_addr(rmem_b(PC + 1));
	byte data = zpage_param();
	lsr(&data, 5, 2);
	wmem_b(addr, data);
}

void lsr_accumulator() {
	lsr(&A, 2, 1);
}

void lsr_zpage_x() {
	word addr = zpagex_addr(rmem_b(PC + 1));
	byte data = rmem_b(addr);
	lsr(&data, 6, 2);
	wmem_b(addr, data);
}

void lsr_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	byte data = rmem_b(addr);
	lsr(&data, 6, 3);
	wmem_b(addr, data);
}


void lsr_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	byte data = rmem_b(addr);
	lsr(&data, 7, 3);
	wmem_w(addr, data);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////Rotate REGION//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void rol(byte *value, int cycles, int pcIncrease) {
	byte cachedFlagC = (byte) bit_test(P, flagC);
	byte cached7 = (byte) bit_test(*value, 7);

	bit_val(&P, flagC, cached7);
	byte shifted = *value << 1;

	bit_val(&shifted, 0, cachedFlagC);
	bit_val(&P, flagZ, shifted == 0);
	bit_val(&P, flagN, bit_test(shifted, 7));
	*value = shifted;

	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void rol_zpage() {
	word addr = zpage_addr(rmem_b(PC + 1));
	byte data = zpage_param();
	rol(&data, 5, 2);
	wmem_b(addr, data);
}

void rol_accumulator() {
	rol(&A, 2, 1);
}

void rol_zpage_x() {
	word addr = zpagex_addr(rmem_b(PC + 1));
	byte data = rmem_b(addr);
	rol(&data, 6, 2);
	wmem_b(addr, data);
}

void rol_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	byte data = rmem_b(addr);
	rol(&data, 6, 3);
	wmem_b(addr, data);
}

void rol_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	byte data = rmem_b(addr);
	rol(&data, 7, 3);
	wmem_w(addr, data);
}


void ror(byte *value, int cycles, int pcIncrease) {
	byte cachedFlagC = (byte) bit_test(P, flagC);
	byte cached0 = (byte) bit_test(*value, 0);

	byte shifted = *value >> 1;
	bit_val(&shifted, 7, cachedFlagC);
	bit_val(&P, flagC, cached0);

	bit_val(&P, flagZ, shifted == 0);
	bit_val(&P, flagN, bit_test(shifted, 7));
	*value = shifted;

	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void ror_zpage() {
	word addr = zpage_addr(rmem_b(PC + 1));
	byte data = zpage_param();
	ror(&data, 5, 2);
	wmem_b(addr, data);
}

void ror_accumulator() {
	ror(&A, 2, 1);
}

void ror_zpage_x() {
	word addr = zpagex_addr(rmem_b(PC + 1));
	byte data = rmem_b(addr);
	ror(&data, 6, 2);
	wmem_b(addr, data);
}

void ror_absolute() {
	word addr = absolute_addr(rmem_w(PC + 1));
	byte data = rmem_b(addr);
	ror(&data, 6, 3);
	wmem_b(addr, data);
}


void ror_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	byte data = rmem_b(addr);
	ror(&data, 7, 3);
	wmem_w(addr, data);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////EOR REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void eor(byte value, int cycles, int pcIncrease) {
	A ^= value;

	bit_val(&P, flagN, bit_test(A, 7));
	bit_val(&P, flagZ, A == 0);

	PC += pcIncrease;
	cyclesThisSec += cycles;
}

void eor_immediate() {
	byte value = rmem_b(PC + 1);
	eor(value, 2, 2);
}

void eor_zpage() {
	eor(zpage_param(), 3, 2);
}

void eor_zpage_x() {
	eor(zpagex_param(), 4, 2);
}

void eor_absolute() {
	eor(absolute_param(), 4, 3);
}

void eor_absolute_x() {
	eor(absolutex_param(), 4, 3);
	//TODO +1 if page crossed
}

void eor_absolute_y() {
	eor(absolutey_param(), 4, 3);
	//TODO +1 if page crossed
}

void eor_indirect_x() {
	eor(indirectx_param(), 6, 2);
}

void eor_indirect_y() {
	eor(indirecty_param(), 5, 2);
	//TODO +1 if page crossed
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////JMP REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void jmp(word addr, int cycles) {
	PC = addr;
	cyclesThisSec += cycles;
}

void jmp_absolute() {
	jmp(absolutey_param(), 3);
}

void jmp_indirect() {
	jmp(indirect_param(), 5);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////Invalid Opcodes REGION/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void invalid() {
	printf("Invalid opcode %X, PC=%X \n", currentOpcode, PC);
	PC++;
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
		&invalid,
		&invalid,
		&invalid,
		&ora_zpage,     //$05       ORA $44       bitwise OR with Accumulator     2       3
		&asl_zpage,     //$06       ASL $44       Arithmetic Shift Left           2       5
		&invalid,
		&php,           //$08       PHP           PusH Processor status           1       3
		&ora_immediate, //$09       ORA #$44      bitwise OR with Accumulator     2       2
		&asl_accumulator,//$0A      ASL A         Arithmetic Shift Left           1       2
		&invalid,
		&invalid,
		&ora_absolute,  //$0D       ORA $4400     bitwise OR with Accumulator     3       4
		&asl_absolute,  //$0E       ASL $4400     Arithmetic Shift Left           3       6
		&invalid,
		&bpl,           //$10       BPL           Branch if plus                  2       2(+2)
		&ora_ind_y,     //$11       ORA ($44), Y  bitwise OR with Accumulator     2       6
		&invalid,
		&invalid,
		&invalid,
		&ora_zpage_x,   //$15       ORA ($44), X  bitwise OR with Accumulator     2       4
		&asl_zpage_x,   //$16       ASL $44, X    Arithmetic Shift Left           2       6
		&invalid,
		&clc,           //$18       CLC           CLear Carry flag                1       2
		&ora_absolute_y,//$19       ORA $440&invalid, Y  bitwise OR with Accumulator     3       4+
		&invalid,
		&invalid,
		&invalid,
		&ora_absolute_x,//$1D       ORA $440&invalid, X  bitwise OR with Accumulator     3       4+
		&asl_absolute_x,//$1E       ASL $440&invalid, X  Arithmetic Shift Left           3       7
		&invalid,
		&jsr_absolute,  //$20       JSR $4400     Jump to SubRoutine              3       6
		&and_indirect_x,//$21       AND ($44, X)  bitwise AND with accumulator    2       6
		&invalid,
		&invalid,
		&bit_zpage,     //$24       BIT $44       BIt Test                        2       3
		&and_zpage,     //$25       AND $44       bitwise AND with accumulator    2       3
		&rol_zpage,     //$26       ROL $44       Rotate Left                     2       5
		&invalid,
		&plp,           //$28       PLP           PuLl to status                  1       4
		&and_immediate, //$29       AND #$44      bitwise AND with accumulator    2       2
		&rol_accumulator,//$2A      ROL $44       Rotate Left                     1       2
		&invalid,
		&bit_absolute,  //$2C       BIT $4400     BIt Test                        3       4
		&and_absolute,  //$2D       AND $4400     bitwise AND with accumulator    3       4
		&rol_absolute,  //$2E       ROL $44       Rotate Left                     3       6
		&invalid,
		&bmi,           //$30       BPL           Branch if minus                 2       2(+2)
		&and_indirect_y,//$31       AND ($44), Y  bitwise AND with accumulator    2       5+
		&invalid,
		&invalid,
		&invalid,
		&and_zpage_x,   //$35       AND $44, X    bitwise AND with accumulator    2       4
		&rol_zpage_x,   //$36       ROL $44       Rotate Left                     2       6
		&invalid,
		&sec,           //$38       SEC           Sets Carry flag                 1       2
		&and_absolute_y,//$39       AND $440&invalid, Y  bitwise AND with accumulator    3       4+
		&invalid,
		&invalid,
		&invalid,
		&and_absolute_x,//$3D       AND $440&invalid, X  bitwise AND with accumulator    3       4+
		&rol_absolute_x,//$3E       ROL $44       Rotate Left                     3       7
		&invalid,
		&rti,           //$40       RTI           Returns from Interrupt          1       6
		&eor_indirect_x,//$41       EOR           Exclusive OR                    2       6
		&invalid,
		&invalid,
		&invalid,
		&eor_zpage,     //$45       EOR           Exclusive OR                     2       3
		&lsr_zpage,     //$46       LSR           Logical Shift Right              2       5
		&invalid,
		&pha,           //$48       PHA           PusH Acumulator                  1       3
		&eor_immediate,  //$49       EOR          Exclusive OR                     2       2
		&lsr_accumulator,//$4A       LSR          Logical Shift Right              1       2
		&invalid,
		&jmp_absolute,  //$4C       JMP           JuMP                              3       3
		&eor_absolute,  //$4D       EOR           Exclusive OR                      3       4
		&lsr_absolute,  //$4E       LSR           Logical Shift Right               3       6
		&invalid,
		&bvc,           //$50       BVC           Branch if Overflow Clear        2       2(+2)
		&eor_indirect_y,//$51       EOR           Exclusive OR                    2       5
		&invalid,
		&invalid,
		&invalid,
		&eor_zpage_x,   //$55       EOR           Exclusive OR                      2       4
		&lsr_zpage_x,   //$56       LSR           Logical Shift Right               2       6
		&invalid,
		&cli,           //$58       CLI           CLear Interrupt flag              1       2,
		&eor_absolute_y,//$59       EOR           Exclusive OR                      3       4
		&invalid,
		&invalid,
		&invalid,
		&eor_absolute_x, //$5D       EOR           Exclusive OR                    3       4
		&lsr_absolute_x, //$5E       LSR           Logical Shift Right             3       7
		&invalid,
		&rts,            //$60      RTS          Returns from Subroutine           1       6
		&adc_indirect_x, //$61      ADC ($44, X) ADd with Carry                    2       6
		&invalid,
		&invalid,
		&invalid,
		&adc_zpage,     //$65       ADC $44      ADd with Carry                    2       3
		&ror_zpage,     //$66       ROR $44      Rotate Right                      2       5
		&invalid,
		&pla,           //$68       PLA           PuLl Acumulator                  1       4
		&adc_immediate, //$69       ADC #$44      ADd with Carry                   2       2
		&ror_accumulator,//$6A      ROR $44      Rotate Right                      1       2
		&invalid,
		&jmp_indirect,  //$6C       JMP          JuMP                             3       5
		&adc_absolute,  //$6D       ADC $4400    ADd with Carry                    3       4
		&ror_absolute,  //$6E       ROR $44      Rotate Right                      3       6
		&invalid,
		&bvs,           //$70      BVS           Branch if plus                    2       2(+2)
		&adc_indirect_y,//$71      ADC ($44), X   ADd with Carry                   2       5+
		&invalid,
		&invalid,
		&invalid,
		&adc_zpage_x,   //$75       ADC $44, X    ADd with Carry                    2       4
		&ror_zpage_x,   //$76       ROR $44      Rotate Right                       2       6
		&invalid,
		&sei,           //$78       SEI           Sets Interrupt flag               1       2,
		&adc_absolute_y,//$79       ADC $440&invalid, Y  ADd with Carry                    3       4+
		&invalid,
		&invalid,
		&invalid,
		&adc_absolute_x,//$7D       ADC $440&invalid, X  ADd with Carry                    3       4+
		&ror_absolute_x,//$7E       ROR $44      Rotate Right                       3       7
		&invalid,
		&invalid,
		&sta_indirect_x,//$81      STA ($44,X)    STore Accumulator                 2       6
		&invalid,
		&invalid,
		&sty_zpage,     //$84      STX $44       STore Y Register                   2       3
		&sta_zpage,     //$85      STA $44       STore Accumulator                  2       2
		&stx_zpage,     //$86      STX $44       STore X Register                   2       2
		&invalid,
		&dey,           //$88       DEY           Decrements Y                      1       2
		&invalid,
		&txa,           //$8A       TXA           Transfer X to A                   1       2
		&invalid,
		&sty_absolute,  //$8C       STX $4400     STore Y Register                  3       4
		&sta_absolute,  //$8D       STA $4400     STore Accumulator                 3       4
		&stx_absolute,  //$8E       STX $4400     STore X Register                  3       4
		&invalid,
		&bcc,           //$90       BCC           Branch if carry clear             2       2(+2)
		&sta_indirect_y,//$91      STA ($44),Y    STore Accumulator                 2       6
		&invalid,
		&invalid,
		&sty_zpage_x,   //$94      STX $44,Y      STore Y Register                  2       4
		&sta_zpage_x,   //$95      STA $44,X      STore Accumulator                 2       4
		&stx_zpage_y,   //96       STX $44, Y     STore X Register                  2       4
		&invalid,
		&tya,           //$98      TYA           Transfer Y to A                    1       2
		&sta_absolute_y,//$9D      STA $440&invalidY    STore Accumulator                 3       5
		&invalid,
		&invalid,
		&invalid,
		&sta_absolute_x,//$9D      STA $440&invalidX    STore Accumulator                 3       5
		&invalid,
		&invalid,
		&ldy_immediate, //A0       LDA #$44       LoaD Accumulator                  2       2
		&lda_indirect_x,//A1       LDA ($44,X)    LoaD Accumulator                  2       6
		&ldx_immediate, //A2       LDA #$44       LoaD Accumulator                  2       2
		&invalid,
		&ldy_zpage,     //$A4       LDA $44       LoaD Accumulator                  2       3
		&lda_zpage,     //$A5       LDA $44       LoaD Accumulator                  2       3
		&ldx_zpage,     //$A6       LDA $44       LoaD Accumulator                  2       3
		&invalid,
		&tay,           //$A8       TAY           Transfer A to Y                   1       2
		&lda_inmediate, //$A9       LDA #$44      LoaD Accumulator                  2       2
		&tax,           //$AA       TAX           Transfer A to X                   1       2
		&invalid,
		&ldy_absolute,  //$A4      LDY $4400     LoaD Y Register                    3       4
		&lda_absolute,  //AD       LDA $4400     LoaD Accumulator                   3       4
		&ldx_absolute,  //AE       LDA $4400     LoaD Accumulator                   3       4
		&invalid,
		&bcs,           //$B0       BCS           Branch id carry set               2       2(+2)
		&lda_indirect_y,//B1        LDA ($44),Y   LoaD Accumulator                  2       5+
		&invalid,
		&invalid,
		&ldy_zpage_x,   //$B4       LDY $44, X    LoaD Y Register                    2       6
		&lda_zpage_x,   //$B5       LDA $44, X    LoaD Accumulator                   2       4
		&ldx_zpage_y,   //$B6       LDX $44, Y    LoaD Accumulator                   2       4
		&invalid,
		&clv,           //$B8       CLV           CLear Overflow flag                1       2,
		&lda_absolute_y,//$B9       LDA $440&invalidY   LoaD Accumulator                   3       4+
		&invalid,
		&invalid,
		&ldy_absolute_x,//$A4      LDY $440&invalidY    LoaD Y Register                    3       4+
		&lda_absolute_x,//BD       LDA $440&invalidX    LoaD Accumulator                   3       4+
		&ldx_absolute_y,//BE       LDA $440&invalidY    LoaD Accumulator                   3       4+
		&invalid,
		&cpy_immediate,     //$C0   CPX #$44      Compare                           2       2
		&cmp_indirect_x,    //$C1   CMP ($44,X)   Compare                           2       6
		&invalid,
		&invalid,
		&cpy_zpage,     //$C4       CPX $44       Compare                           2       3
		&cmp_zpage,     //$C5       CMP $44       Compare                           2       3
		&dec_mem_zpage, //$C6       DEC $44       DEcrement Memory                  2       5
		&invalid,
		&iny,           //$C8       INY           Increments Y                      1       2
		&cmp_inmediate, //$C9       CMP #$44      Compare                           2       2
		&dex,           //$CA       DEX           Decrements X register             1       2
		&invalid,
		&cpy_absolute,    //$CC     CPX $4400     Compare                           3       4
		&cmp_absolute,    //$CD     CMP $4400     Compare                           3       4
		&dec_mem_absolute,//$CE     DEC $4400     DEcrement Memory                  3       6
		&invalid,
		&bne,           //$D0       BNE           Branch now equals                 2       2(+2)
		&cmp_indirect_y,    //$D1   CMP ($44),X   Compare                           2       5+
		&invalid,
		&invalid,
		&invalid,
		&cmp_zpage_x,       //$D5      CMP $44,X     Compare                           2       4
		&dec_mem_zpage_x,   //$D6      DEC $44,X     DEcrement Memory                  2       6
		&invalid,
		&cld,               //$D8      CLD           CLear Decimal flag                1       2
		&cmp_absolute_y,    //$DD      CMP $440&invalidY   Compare                          3       4+
		&invalid,
		&invalid,
		&invalid,
		&cmp_absolute_x,    //$DD   CMP $440&invalidX   Compare                          3       4+
		&dec_mem_absolute_x,//$DE   DEC $440&invalidX   DEcrement Memory                 3       7
		&invalid,
		&cpx_immediate,    //$E0    CPX #$44      Compare                          2       2
		&sbc_indirect_x,//$E1       SBC ($44,X)   SuBstract with Carry             2       6
		&invalid,
		&invalid,
		&cpx_zpage,    //$E4        CPX $44       Compare                          2       3
		&sbc_zpage,     //$E5       SBC $44       SuBstract with Carry             2       3
		&inc_mem_zpage, //$E6       INC $44       INcrement Memory                 2       5
		&invalid,
		&inx,           //$E8       INX           Increments X register            1       7
		&sbc_immediate, //$E9       SBC #$44      SuBstract with Carry             2       2
		&nop,           //$EA       NOP           No OPeration                     1       2
		&invalid,
		&cpx_absolute,  //$EC      CPX $4400       Compare                         3       4
		&sbc_absolute, //$ED       SBC $4400       SuBstract with Carry            3       4
		&inc_mem_absolute,//$EE    INC $4400       INcrement Memory                3       6
		&invalid,
		&beq,           //$F0       BEQ           Branch if equals                 2       2(+2)
		&sbc_indirect_y,//$F1       SBC ($44),Y   SuBstract with Carry             2       5+
		&invalid,
		&invalid,
		&invalid,
		&sbc_zpage_x,   //$F5       SBC $44,X     SuBstract with Carry             2       4
		&inc_mem_zpage_x,//$F6      INC $44, X    INcrement Memory                 2       6
		&invalid,
		&sed,           //$F8       SED           Sets Decimal flag                1       2
		&sbc_absolute_y,//$F9       SBC $440&invalidY   SuBstract with Carry             3       4+
		&invalid,
		&invalid,
		&invalid,
		&sbc_absolute_x,//$FD       SBC $440&invalidX   SuBstract with Carry             3       4+
		&inc_mem_absolute_x,//$FE     INC $440&invalidX   INcrement Memory               3       7
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

byte zpage_param() {
	word addr = zpage_addr(rmem_b(PC + 1));
	return rmem_b(addr);
}

byte zpagex_param() {
	word addr = zpagex_addr(rmem_b(PC + 1));
	return rmem_b(addr);
}

byte zpagey_param() {
	word addr = zpagey_addr(rmem_b(PC + 1));
	return rmem_b(addr);
}

byte absolute_param() {
	word addr = absolute_addr(rmem_w(PC + 1));
	return rmem_b(addr);
}

byte absolutex_param() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	return rmem_b(addr);
}

byte absolutey_param() {
	word addr = absolutey_addr(rmem_w(PC + 1));
	return rmem_b(addr);
}

byte indirectx_param() {
	word addr = indirectx_addr(rmem_b(PC + 1));
	return rmem_b(addr);
}

byte indirecty_param() {
	word addr = indirecty_addr(rmem_b(PC + 1));
	return rmem_b(addr);
}

word indirect_param() {
	word addr = rmem_w(PC + 1);
	byte least_significant = rmem_b(addr);
	byte most_significant = rmem_b(addr + 1);

	return (most_significant << 8) + least_significant;
}
