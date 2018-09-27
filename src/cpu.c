#include "cpu.h"

//TODO this has some dupplicated code. Can we fix this?
void *cpu_run(void *arg) {
	cpu_running = 1;
	static struct timespec this_second;
	static struct timespec this_cycle;
	long nano_per_cycle = (long) ((1.0 / cpu_speed) * NANOSECOND);

	long should_have_elapsed, have_elapsed, elapsed_delta = 0;
	clock_gettime(CLOCK_REALTIME, &this_second);

	while (cpu_running) {
		//Check if enough time has passed, and sleep the process otherwise
		clock_gettime(CLOCK_REALTIME, &this_cycle);

		//New second. Reset stuff
		if (this_cycle.tv_sec > this_second.tv_sec) {
			this_second = this_cycle;
			log_info("CPU cycles last second (%i): %d\n", this_cycle.tv_sec, cpu_cyclesThisSec);
			cpu_cyclesThisSec = 0;
		}

		should_have_elapsed = nano_per_cycle * cpu_cyclesThisSec;
		have_elapsed = this_cycle.tv_nsec - this_second.tv_nsec;
		elapsed_delta = should_have_elapsed - have_elapsed;
		if (elapsed_delta > 0) {
			this_cycle.tv_nsec = elapsed_delta;
			this_cycle.tv_sec = 0;
			nanosleep(&this_cycle, NULL);
		}

		cpu_instruction();
	}
}

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
void cpu_power_up(int clock_speed_hertzs) {
	zero_ram();
	A = X = Y = 0;
	P = 0x24;
	SP = 0xFD;
	wmem_b(0x4015, 0);
	wmem_b(0x4017, 0);
	for (uint i = 0x4000; i <= 0x400F; ++i) {
		wmem_b(i, 0);
	}

	cpu_speed = clock_speed_hertzs;
	cpu_cyclesThisSec = 0;
	//TODO IRQ stuff
	//TODO LFSR stuff
}

void reset_pc() {
	PC = rmem_w(0xFFFC);
}

//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////NOP REGION////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void nop(int cycles, int pcIncrease) {
	log_instruction(0, "NOP\t\t\t");

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
}

void nop1() {
	nop(2, 1);
}

void nop2() {
	nop(2, 2);
}

void nop3() {
	nop(3, 3);
}

void breakpoint() {
	bit_set(&P, flagB);
	bit_set(&P, 5);
	cpu_cyclesThisSec += 7;
	PC++;
}

void nmi() {
	push_w(PC);
	bit_clear(&P, flagB);
	push_b(P);
	word nmi_vector = rmem_w(0xFFFA);
	bit_set(&P, flagI);
	PC = nmi_vector;
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
	log_instruction(pcIncrease - 1, "\tORA #$%02X\t", b);
	//Do the actual or operation, saving the result in the accumulator
	A = A | b;
	//Set the flags
	bit_val(&P, flagZ, A == 0x00);
	bit_val(&P, flagN, bit_test(A, 7));

	//Update cycles and pc
	cpu_cyclesThisSec += cycles;
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
	log_instruction(pcIncrease - 1, "\tASL #$%02X", b);
	bit_val(&P, flagC, bit_test(*b, flagN));

	byte shifted = *b << 1;
	bit_val(&P, flagN, bit_test(shifted, 7));
	bit_val(&P, flagZ, shifted == 0);
	*b = shifted;

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
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
	log_instruction(2, "JSR #$%02X\t", addr);

	push_w(cachedPC); // Stores the address of the next opcode minus one

	PC = addr;
	cpu_cyclesThisSec += 6;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PHP (PusH Processor status) REGION/////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void php() {
	log_instruction(0, "PHP\t\t\t");
	push_b(P);

	PC++;
	cpu_cyclesThisSec += 3;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PLP (PuLl Processor status) REGION/////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void plp() {
	log_instruction(0, "PLP\t\t\t");
	P = pop_b();

	//Bit 5 of P is unused, so clear it. It should always be 1.
	bit_set(&P, 5);
	bit_clear(&P, flagB);

	PC++;
	cpu_cyclesThisSec += 4;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PHA (PusH Acumulator) REGION///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void pha() {
	log_instruction(0, "PHA\t\t\t");
	push_b(A);

	PC++;
	cpu_cyclesThisSec += 3;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PLA (PuLl Acumulator) REGION///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void pla() {
	log_instruction(0, "PLA\t\t\t");
	A = pop_b();
	bit_val(&P, flagZ, A == 0);
	bit_val(&P, flagN, bit_test(A, 7));

	PC++;
	cpu_cyclesThisSec += 4;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////TSX (Transfer Stack pointer to X) REGION///////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void tsx() {
	log_instruction(0, "TSX\t\t\t");
	transfer_reg(&SP, &X);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////TXS (Transfer X to Stack pointer) REGION///////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void txs() {
	log_instruction(0, "TXS\t\t\t");
	SP = X;
	PC++;
	cpu_cyclesThisSec += 2;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////AND (bitwise AND accumulator) REGION///////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


void and(byte value, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tAND #$%02X\t", value);
	A &= value;
	bit_val(&P, flagZ, A == 0);
	bit_val(&P, flagN, bit_test(A, 7));

	cpu_cyclesThisSec += cycles;
	PC += pcIncrease;
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
	log_instruction(pcIncrease - 1, "\tBIT #$%02X\t", value);

	byte tmp = A & value;
	bit_val(&P, flagZ, tmp == 0);
	bit_val(&P, flagV, bit_test(value, 6));
	bit_val(&P, flagN, bit_test(value, 7));

	cpu_cyclesThisSec += cycles;
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
	cpu_cyclesThisSec += 2; //Constant. Always
	PC++; //Constant. Always
}

void clc() {
	log_instruction(0, "CLC\t\t\t");
	set_flag_value(flagC, 0);
}

void cld() {
	log_instruction(0, "CLD\t\t\t");
	set_flag_value(flagD, 0);
}

void cli() {
	log_instruction(0, "CLI\t\t\t");
	set_flag_value(flagI, 0);
}

void clv() {
	log_instruction(0, "CLV\t\t\t");
	set_flag_value(flagV, 0);
}

void sec() {
	log_instruction(0, "SEC\t\t\t");
	set_flag_value(flagC, 1);
}

void sei() {
	log_instruction(0, "SEI\t\t\t");
	set_flag_value(flagI, 1);
}

void sed() {
	log_instruction(0, "SED\t\t\t");
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
	bit_val(&P, flagZ, *to_reg == 0);
	bit_val(&P, flagN, bit_test(*to_reg, 7));
	PC++;
	cpu_cyclesThisSec += 2;
}

/**
 * All of this instructions have a length of one byte and require two machine cycles
 */
void dec_reg(byte *reg) {
	(*reg)--;
	bit_val(&P, flagZ, *reg == 0);
	bit_val(&P, flagN, bit_test(*reg, 7));
	PC++;
	cpu_cyclesThisSec += 2;
}

/**
 * All of this instructions have a length of one byte and require two machine cycles
 */
void inc_reg(byte *reg) {
	(*reg)++;
	bit_val(&P, flagZ, *reg == 0);
	bit_val(&P, flagN, bit_test(*reg, 7));
	PC++;
	cpu_cyclesThisSec += 2;
}

void tax() {
	log_instruction(0, "TAX\t\t\t");
	transfer_reg(&A, &X);
}

void txa() {
	log_instruction(0, "TXA\t\t\t");
	transfer_reg(&X, &A);
}

void dex() {
	log_instruction(0, "DEX\t\t\t");
	dec_reg(&X);
}

void inx() {
	log_instruction(0, "INX\t\t\t");
	inc_reg(&X);
}

void tay() {
	log_instruction(0, "TAY\t\t\t");
	transfer_reg(&A, &Y);
}

void tya() {
	log_instruction(0, "TYA\t\t\t");
	transfer_reg(&Y, &A);
}

void dey() {
	log_instruction(0, "DEY\t\t\t");
	dec_reg(&Y);
}

void iny() {
	log_instruction(0, "INY\t\t\t");
	inc_reg(&Y);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////ADC REGION//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void adc_internal(byte value);

void adc_internal(byte value) {
	byte carry = (byte) bit_test(P, flagC);
	uint result = A + carry + value;
	carry = (byte) ((result & 0x100) >> 8);

	bit_val(&P, flagC, carry);

	// If operands same source sign but different result sign
	int isOverflown = ((A ^ result) & (value ^ result) & 0x80);
	A = (byte) result;

	bit_val(&P, flagZ, A == 0);
	bit_val(&P, flagV, isOverflown);
	bit_val(&P, flagN, bit_test(A, 7));
}


void adc(byte value, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tADC #$%02X\t", value);

	adc_internal(value);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
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

void try_branch(byte flag, int req_flag_val, const char *mnemonic) {
	signed char value = rmem_b(PC + 1); //byte is unsigned but we need a signed char
	log_instruction(1, mnemonic, PC + 2 + value);
	cpu_cyclesThisSec += 2;//This is always constant

	PC += 2;
	if (bit_test(P, flag) == req_flag_val) {
		PC += value;
		cpu_cyclesThisSec++;
		//TODO +2 cycle if page crossed
	}
}

void bpl() {
	try_branch(flagN, 0, "\tBPL $%02X\t");
}

void bmi() {
	try_branch(flagN, 1, "\tBMI $%02X\t");
}

void bvc() {
	try_branch(flagV, 0, "\tBVC $%02X\t");
}

void bvs() {
	try_branch(flagV, 1, "\tBVS $%02X\t");
}

void bcc() {
	try_branch(flagC, 0, "\tBCC $%02X\t");
}

void bcs() {
	try_branch(flagC, 1, "\tBCS $%02X\t");
}

void bne() {
	try_branch(flagZ, 0, "\tBNE $%02X\t");
}

void beq() {
	try_branch(flagZ, 1, "\tBEQ $%02X\t");
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////SBC REGION//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void sbc(byte value, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tSBC #$%02X\t", value);

	adc_internal(~value);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
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
	cpu_cyclesThisSec += cycles;
}

void inc_mem(word memAddr, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tINC #$%02X = %02X", memAddr, memAddr + 1);
	delta_memory(memAddr, 1, cycles, pcIncrease);
}

void dec_mem(word memAddr, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tDEC #$%02X = %02X", memAddr, memAddr - 1);
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

void load_register(byte *regPtr, byte value, int cycles, int pcIncrease, const char *regMnemonic) {
	log_instruction(pcIncrease - 1, regMnemonic, value);
	*regPtr = value;

	bit_val(&P, flagZ, *regPtr == 0);
	bit_val(&P, flagN, bit_test(*regPtr, 7));

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
}

void lda_inmediate() {
	load_register(&A, rmem_b(PC + 1), 2, 2, "\tLDA #$%02X\t");
}

void lda_zpage() {
	load_register(&A, zpage_param(), 3, 2, "\tLDA #$%02X\t");
}

void lda_zpage_x() {
	load_register(&A, zpagex_param(), 4, 2, "\tLDA #$%02X\t");
}

void lda_absolute() {
	load_register(&A, absolute_param(), 4, 3, "\tLDA #$%02X\t");
}

void lda_absolute_x() {
	word addr = absolutex_addr(rmem_w(PC + 1));
	load_register(&A, rmem_b(addr), 4, 3, "LDA #$%02X\t");
	// TODO +1 if page crossed
}

void lda_absolute_y() {
	load_register(&A, absolutey_param(), 4, 3, "\tLDA #$%02X\t");
	// TODO +1 if page crossed
}

void lda_indirect_x() {
	load_register(&A, indirectx_param(), 6, 2, "\tLDA #$%02X\t");
}

void lda_indirect_y() {
	load_register(&A, indirecty_param(), 5, 2, "\tLDA #$%02X\t");
	// TODO +1 if page crossed
}

void ldx_immediate() {
	load_register(&X, rmem_b(PC + 1), 2, 2, "\tLDX #$%02X\t");
}

void ldx_zpage() {
	load_register(&X, zpage_param(), 3, 2, "\tLDX #$%02X\t");
}

void ldx_zpage_y() {
	load_register(&X, zpagey_param(), 4, 2, "\tLDX #$%02X\t");
}

void ldx_absolute() {
	load_register(&X, absolute_param(), 4, 3, "\tLDX #$%02X\t");
}

void ldx_absolute_y() {
	load_register(&X, absolutey_param(), 4, 3, "\tLDX #$%02X\t");
	// TODO +1 if page crossed
}

void ldy_immediate() {
	load_register(&Y, rmem_b(PC + 1), 2, 2, "\tLDY #$%02X\t");
}

void ldy_zpage() {
	load_register(&Y, zpage_param(), 3, 2, "\tLDY #$%02X\t");
}

void ldy_zpage_x() {
	load_register(&Y, zpagex_param(), 4, 2, "\tLDY #$%02X\t");
}

void ldy_absolute() {
	load_register(&Y, absolute_param(), 4, 3, "\tLDY #$%02X\t");
}

void ldy_absolute_x() {
	load_register(&Y, absolutex_param(), 4, 3, "\tLDY #$%02X\t");
	// TODO +1 if page crossed
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////STORE REGISTERS REGION///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/**
 * Store the specified value onto the pointed register
 */
void store_register(byte reg, word memAddr, int cycles, int pcIncrease, const char *regMnemonic) {
	log_instruction(pcIncrease - 1, regMnemonic, memAddr, reg);
	wmem_b(memAddr, reg);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
}

void sta_zpage() {
	store_register(A, zpage_addr(rmem_b(PC + 1)), 3, 2, "\tSTA $%02X = %02X");
}

void sta_zpage_x() {
	store_register(A, zpagex_addr(rmem_b(PC + 1)), 4, 2, "\tSTA $%02X = %02X");
}

void sta_absolute() {
	store_register(A, absolute_addr(rmem_w(PC + 1)), 4, 3, "STA $%02X = %02X");
}

void sta_absolute_x() {
	store_register(A, absolutex_addr(rmem_w(PC + 1)), 5, 3, "STA $%02X = %02X");
}

void sta_absolute_y() {
	store_register(A, absolutey_addr(rmem_w(PC + 1)), 5, 3, "STA $%02X = %02X");
}

void sta_indirect_x() {
	store_register(A, indirectx_addr(rmem_b(PC + 1)), 6, 2, "\tSTA $%02X = %02X");
}

void sta_indirect_y() {
	store_register(A, indirecty_addr(rmem_b(PC + 1)), 6, 2, "\tSTA $%02X = %02X");
}

void stx_zpage() {
	store_register(X, zpage_addr(rmem_b(PC + 1)), 3, 2, "\tSTX $%02X = %02X");
}

void stx_zpage_y() {
	store_register(X, zpagey_addr(rmem_b(PC + 1)), 4, 2, "\tSTX $%02X = %02X");
}

void stx_absolute() {
	store_register(X, absolute_addr(rmem_w(PC + 1)), 4, 3, "\tSTX $%02X = %02X");
}

void sty_zpage() {
	store_register(Y, zpage_addr(rmem_b(PC + 1)), 3, 2, "\tSTY $%02X = %02X");
}

void sty_zpage_x() {
	store_register(Y, zpagex_addr(rmem_b(PC + 1)), 4, 2, "\tSTY $%02X = %02X");
}

void sty_absolute() {
	store_register(Y, absolute_addr(rmem_w(PC + 1)), 4, 3, "\tSTY $%02X = %02X");
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////ReTurn from Interrupt REGION////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void rts() {
	log_instruction(0, "RTS\t\t\t");
	PC = pop_w();

	PC++; // JSR pushes the address -1, so when we recover (here) we have to add 1 to make up for that "1" lost
	cpu_cyclesThisSec += 6;
}

void rti() {
	log_instruction(0, "RTI\t\t\t");
	P = pop_b();
	bit_set(&P, flagUnused);//It has to be one. Always
	PC = pop_w(); //Unlike RTS. RTI pulls the correct PC address. No need to increment
	cpu_cyclesThisSec += 6;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////COMPARE REGISTERS REGION///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void compare_register(byte *regPtr, byte value, int cycles, int pcIncrease, const char *regMnemonic) {
	log_instruction(pcIncrease - 1, regMnemonic, value);

	byte temp_result = *regPtr - value;

	bit_val(&P, flagC, *regPtr >= value);

	//Need to do this since there are some positive numbers that should trigger this flag. i.e. 0x80
	bit_val(&P, flagN, bit_test(temp_result, 7));
	bit_val(&P, flagZ, *regPtr == value);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
}

void cmp_inmediate() {
	compare_register(&A, rmem_b(PC + 1), 2, 2, "\tCMP #$%02X\t");
}

void cmp_zpage() {
	compare_register(&A, zpage_param(), 3, 2, "\tCMP #$%02X\t");
}

void cmp_zpage_x() {
	compare_register(&A, zpagex_param(), 4, 2, "\tCMP #$%02X\t");
}

void cmp_absolute() {
	compare_register(&A, absolute_param(), 4, 3, "\tCMP #$%02X\t");
}

void cmp_absolute_x() {
	compare_register(&A, absolutex_param(), 4, 3, "\tCMP #$%02X\t");
	//TODO +1 if page crossed
}

void cmp_absolute_y() {
	compare_register(&A, absolutey_param(), 4, 3, "\tCMP #$%02X\t");
	//TODO +1 if page crossed
}

void cmp_indirect_x() {
	compare_register(&A, indirectx_param(), 6, 2, "\tCMP #$%02X\t");
}

void cmp_indirect_y() {
	compare_register(&A, indirecty_param(), 5, 2, "\tCMP #$%02X\t");
	//TODO +1 if page crossed
}

void cpx_immediate() {
	compare_register(&X, rmem_b(PC + 1), 2, 2, "\tCPX #$%02X\t");
}

void cpx_zpage() {
	compare_register(&X, zpage_param(), 3, 2, "\tCPX #$%02X\t");
}

void cpx_absolute() {
	compare_register(&X, absolute_param(), 4, 3, "\tCPX #$%02X\t");
}

void cpy_immediate() {
	compare_register(&Y, rmem_b(PC + 1), 2, 2, "\tCPY #$%02X\t");
}

void cpy_zpage() {
	compare_register(&Y, zpage_param(), 3, 2, "\tCPY #$%02X\t");
}

void cpy_absolute() {
	compare_register(&Y, absolute_param(), 4, 3, "\tCPY #$%02X\t");
}


//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////LSR (Logical Shift Right) REGION///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


void lsr_internal(byte *value);

void lsr_internal(byte *value) {
	bit_val(&P, flagC, bit_test(*value, 0));

	byte shifted = *value >> 1;

	bit_val(&P, flagZ, shifted == 0);
	bit_val(&P, flagN, bit_test(shifted, 7));
	*value = shifted;
}


void lsr(byte *value, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "LSR #$%02X", value);

	lsr_internal(value);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
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
	log_instruction(pcIncrease - 1, "ROL $%02X", value);

	byte cachedFlagC = (byte) bit_test(P, flagC);
	byte cached7 = (byte) bit_test(*value, 7);

	bit_val(&P, flagC, cached7);
	byte shifted = *value << 1;

	bit_val(&shifted, 0, cachedFlagC);
	bit_val(&P, flagZ, shifted == 0);
	bit_val(&P, flagN, bit_test(shifted, 7));
	*value = shifted;

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
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

void ror_internal(byte *value);

void ror_internal(byte *value) {
	byte cachedFlagC = (byte) bit_test(P, flagC);
	byte cached0 = (byte) bit_test(*value, 0);

	byte shifted = *value >> 1;
	bit_val(&shifted, 7, cachedFlagC);
	bit_val(&P, flagC, cached0);

	bit_val(&P, flagZ, shifted == 0);
	bit_val(&P, flagN, bit_test(shifted, 7));
	*value = shifted;
}

void ror(byte *value, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tROR $%02X\t", value);

	ror_internal(value);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
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
void eor_internal(byte value);

void eor_internal(byte value) {
	A ^= value;

	bit_val(&P, flagN, bit_test(A, 7));
	bit_val(&P, flagZ, A == 0);
}

void eor(byte value, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tEOR $%02X\t\t", value);

	eor_internal(value);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
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
	log_instruction(2, "JMP $%02X\t", addr);

	PC = addr;
	cpu_cyclesThisSec += cycles;
}

void jmp_absolute() {
	jmp(rmem_w(PC + 1), 3);
}

void jmp_indirect() {
	jmp(indirect_param(), 5);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////LAX REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void lax(byte value, int cycles, int pcIncrease) {

	log_instruction(pcIncrease - 1, "\tLAX $%02X\t", value);
	X = value;
	A = value;

	bit_val(&P, flagZ, value == 0);
	bit_val(&P, flagN, bit_test(value, 7));

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
}

void lax_absolute() {
	lax(absolute_param(), 4, 3);
}

void lax_absolute_y() {
	lax(absolutey_param(), 4, 3);
	//TODO +1 if page crossed
}

void lax_zpage() {
	lax(zpage_param(), 3, 2);
}

void lax_zpage_y() {
	lax(zpagey_param(), 4, 2);
}

void lax_indirect_x() {
	lax(indirectx_param(), 6, 2);
}

void lax_indirect_y() {
	lax(indirecty_param(), 5, 2);
	//TODO +1 if page crossed
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////AXS REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void axs(word addr, int cycles, int pcIncrease) {
	log_instruction(1, "\tSAX $%02X\t", addr);

	byte value = A & X;
	wmem_b(addr, value);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
}

void axs_indirect_x() {
	axs(indirectx_addr(rmem_b(PC + 1)), 6, 2);
}

void axs_zpage() {
	axs(zpage_addr(rmem_b(PC + 1)), 3, 2);
}

void axs_zpage_y() {
	axs(zpagey_addr(rmem_b(PC + 1)), 4, 2);
}

void axs_absolute() {
	axs(absolute_addr(rmem_w(PC + 1)), 4, 3);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////DCM (DCP) REGION///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * It is the equivalent as to do a dec_mem and a cmp, so we'll just execute those two on order. It should be fine
 */
void dcm(word addr, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tDCP $%02X", addr);

	delta_memory(addr, -1, 0, 0);

	byte value = rmem_b(addr);
	byte temp_result = A - value;

	bit_val(&P, flagC, A >= value);

	//Need to do this since there are some positive numbers that should trigger this flag. i.e. 0x80
	bit_val(&P, flagN, bit_test(temp_result, 7));
	bit_val(&P, flagZ, A == value);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
}

void dcm_absolute() {
	dcm(absolute_addr(rmem_w(PC + 1)), 6, 3);
}

void dcm_absolute_x() {
	dcm(absolutex_addr(rmem_w(PC + 1)), 7, 3);
}

void dcm_absolute_y() {
	dcm(absolutey_addr(rmem_w(PC + 1)), 7, 3);
}

void dcm_zpage() {
	dcm(zpage_addr(rmem_b(PC + 1)), 5, 2);
}

void dcm_zpage_x() {
	dcm(zpagex_addr(rmem_b(PC + 1)), 6, 2);
}

void dcm_indirect_x() {
	dcm(indirectx_addr(rmem_b(PC + 1)), 8, 2);
}

void dcm_indirect_y() {
	dcm(indirecty_addr(rmem_b(PC + 1)), 8, 2);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////DCM (DCP) REGION///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * It is the equivalent as to do a dec_mem and a cmp, so we'll just execute those two on order. It should be fine
 * Bear in mind, an sbc is the same as an asl with the parameter ~'ed
 */
void ins(word addr, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tINS $%02X\t", addr);

	delta_memory(addr, 1, 0, 0);

	byte value = rmem_b(addr);

	adc_internal(~value);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
}

void ins_absolute() {
	ins(absolute_addr(rmem_w(PC + 1)), 6, 3);
}

void ins_absolute_x() {
	ins(absolutex_addr(rmem_w(PC + 1)), 7, 3);
}

void ins_absolute_y() {
	ins(absolutey_addr(rmem_w(PC + 1)), 7, 3);
}

void ins_zpage() {
	ins(zpage_addr(rmem_b(PC + 1)), 5, 2);
}

void ins_zpage_x() {
	ins(zpagex_addr(rmem_b(PC + 1)), 6, 2);
}

void ins_indirect_x() {
	ins(indirectx_addr(rmem_b(PC + 1)), 8, 2);
}

void ins_indirect_y() {
	ins(indirecty_addr(rmem_b(PC + 1)), 8, 2);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////ASO (SLO) REGION///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void aso(word addr, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tASO $%02X", addr);

	//ASL
	byte value = rmem_b(addr);
	bit_val(&P, flagC, bit_test(value, flagN));

	byte shifted = value << 1;
	wmem_b(addr, shifted);

	//Now the ORA
	A = A | shifted;
	//Set the flags
	bit_val(&P, flagZ, A == 0x00);
	bit_val(&P, flagN, bit_test(A, 7));

	//Update cycles and pc
	cpu_cyclesThisSec += cycles;
	PC += pcIncrease;
}

void aso_absolute() {
	aso(absolute_addr(rmem_w(PC + 1)), 6, 3);
}

void aso_absolute_x() {
	aso(absolutex_addr(rmem_w(PC + 1)), 7, 3);
}

void aso_absolute_y() {
	aso(absolutey_addr(rmem_w(PC + 1)), 7, 3);
}

void aso_zpage() {
	aso(zpage_addr(rmem_b(PC + 1)), 5, 2);
}

void aso_zpage_x() {
	aso(zpagex_addr(rmem_b(PC + 1)), 6, 2);
}

void aso_indirect_x() {
	aso(indirectx_addr(rmem_b(PC + 1)), 8, 2);
}

void aso_indirect_y() {
	aso(indirecty_addr(rmem_b(PC + 1)), 8, 2);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////RLA REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * RLA ROLs the contents of a memory location and then ANDs the result with the accumulator.
 */
void rla(word addr, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tRLA $%02X\t", addr);

	byte value = rmem_b(addr);

	byte cachedFlagC = (byte) bit_test(P, flagC);
	byte cached7 = (byte) bit_test(value, 7);

	bit_val(&P, flagC, cached7);
	byte shifted = value << 1;

	bit_val(&shifted, 0, cachedFlagC);
	wmem_b(addr, shifted);

	A &= shifted;
	bit_val(&P, flagZ, A == 0);
	bit_val(&P, flagN, bit_test(A, 7));

	cpu_cyclesThisSec += cycles;
	PC += pcIncrease;
}

void rla_absolute() {
	rla(absolute_addr(rmem_w(PC + 1)), 6, 3);
}

void rla_absolute_x() {
	rla(absolutex_addr(rmem_w(PC + 1)), 7, 3);
}

void rla_absolute_y() {
	rla(absolutey_addr(rmem_w(PC + 1)), 7, 3);
}

void rla_zpage() {
	rla(zpage_addr(rmem_b(PC + 1)), 5, 2);
}

void rla_zpage_x() {
	rla(zpagex_addr(rmem_b(PC + 1)), 6, 2);
}

void rla_indirect_x() {
	rla(indirectx_addr(rmem_b(PC + 1)), 8, 2);
}

void rla_indirect_y() {
	rla(indirecty_addr(rmem_b(PC + 1)), 8, 2);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////LSE REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void lse(word addr, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tLSE $%02X\t", addr);

	byte data = rmem_b(addr);
	lsr_internal(&data);
	wmem_b(addr, data);

	eor_internal(data);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
}

void lse_absolute() {
	lse(absolute_addr(rmem_w(PC + 1)), 6, 3);
}

void lse_absolute_x() {
	lse(absolutex_addr(rmem_w(PC + 1)), 7, 3);
}

void lse_absolute_y() {
	lse(absolutey_addr(rmem_w(PC + 1)), 7, 3);
}

void lse_zpage() {
	lse(zpage_addr(rmem_b(PC + 1)), 5, 2);
}

void lse_zpage_x() {
	lse(zpagex_addr(rmem_b(PC + 1)), 6, 2);
}

void lse_indirect_x() {
	lse(indirectx_addr(rmem_b(PC + 1)), 8, 2);
}

void lse_indirect_y() {
	lse(indirecty_addr(rmem_b(PC + 1)), 8, 2);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////RRA REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * RRA RORs the contents of a memory location and then ADCs the result with the accumulator.
 */
void rra(word addr, int cycles, int pcIncrease) {
	log_instruction(pcIncrease - 1, "\tRRA $%02X\t", addr);

	byte value = rmem_b(addr);
	ror_internal(&value);
	wmem_b(addr, value);

	adc_internal(value);

	PC += pcIncrease;
	cpu_cyclesThisSec += cycles;
}

void rra_absolute() {
	rra(absolute_addr(rmem_w(PC + 1)), 6, 3);
}

void rra_absolute_x() {
	rra(absolutex_addr(rmem_w(PC + 1)), 7, 3);
}

void rra_absolute_y() {
	rra(absolutey_addr(rmem_w(PC + 1)), 7, 3);
}

void rra_zpage() {
	rra(zpage_addr(rmem_b(PC + 1)), 5, 2);
}

void rra_zpage_x() {
	rra(zpagex_addr(rmem_b(PC + 1)), 6, 2);
}

void rra_indirect_x() {
	rra(indirectx_addr(rmem_b(PC + 1)), 8, 2);
}

void rra_indirect_y() {
	rra(indirecty_addr(rmem_b(PC + 1)), 8, 2);
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////Invalid Opcodes REGION/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void invalid() {
	log_error("Invalid opcode %02X, PC=%02X \n", currentOpcode, PC);
	PC += 2;
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
		&breakpoint,    //$00       BRK           BReaK                           1       7
		&ora_ind_x,     //$01       ORA ($44, X)  bitwise OR with Accumulator     2       6
		&invalid,
		&aso_indirect_x,//$03       ASO
		&nop2,          //$04       NOP
		&ora_zpage,     //$05       ORA $44       bitwise OR with Accumulator     2       3
		&asl_zpage,     //$06       ASL $44       Arithmetic Shift Left           2       5
		&aso_zpage,     //$07       ASO
		&php,           //$08       PHP           PusH Processor status           1       3
		&ora_immediate, //$09       ORA #$44      bitwise OR with Accumulator     2       2
		&asl_accumulator,//$0A      ASL A         Arithmetic Shift Left           1       2
		&invalid,
		&nop3,          //$0C       NOP
		&ora_absolute,  //$0D       ORA $4400     bitwise OR with Accumulator     3       4
		&asl_absolute,  //$0E       ASL $4400     Arithmetic Shift Left           3       6
		&aso_absolute,  //$0F       ASO
		&bpl,           //$10       BPL           Branch if plus                  2       2(+2)
		&ora_ind_y,     //$11       ORA ($44), Y  bitwise OR with Accumulator     2       6
		&invalid,
		&aso_indirect_y,//$13       ASO
		&nop2,          //$14       NOP
		&ora_zpage_x,   //$15       ORA ($44), X  bitwise OR with Accumulator     2       4
		&asl_zpage_x,   //$16       ASL $44, X    Arithmetic Shift Left           2       6
		&aso_zpage_x,   //$07       ASO
		&clc,           //$18       CLC           CLear Carry flag                1       2
		&ora_absolute_y,//$19       ORA $440&invalid, Y  bitwise OR with Accumulator     3       4+
		&nop1,          //$1A       NOP
		&aso_absolute_y,//$1B       ASO
		&nop3,          //$1C       NOP
		&ora_absolute_x,//$1D       ORA $440&invalid, X  bitwise OR with Accumulator     3       4+
		&asl_absolute_x,//$1E       ASL $440&invalid, X  Arithmetic Shift Left           3       7
		&aso_absolute_x,//$1F       ASO
		&jsr_absolute,  //$20       JSR $4400     Jump to SubRoutine              3       6
		&and_indirect_x,//$21       AND ($44, X)  bitwise AND with accumulator    2       6
		&invalid,
		&rla_indirect_x, //$23      RLA
		&bit_zpage,     //$24       BIT $44       BIt Test                        2       3
		&and_zpage,     //$25       AND $44       bitwise AND with accumulator    2       3
		&rol_zpage,     //$26       ROL $44       Rotate Left                     2       5
		&rla_zpage,     //$27       RLA
		&plp,           //$28       PLP           PuLl to status                  1       4
		&and_immediate, //$29       AND #$44      bitwise AND with accumulator    2       2
		&rol_accumulator,//$2A      ROL $44       Rotate Left                     1       2
		&invalid,
		&bit_absolute,  //$2C       BIT $4400     BIt Test                        3       4
		&and_absolute,  //$2D       AND $4400     bitwise AND with accumulator    3       4
		&rol_absolute,  //$2E       ROL $44       Rotate Left                     3       6
		&rla_absolute,  //$2F       RLA
		&bmi,           //$30       BPL           Branch if minus                 2       2(+2)
		&and_indirect_y,//$31       AND ($44), Y  bitwise AND with accumulator    2       5+
		&invalid,
		&rla_indirect_y,//$33       RLA
		&nop2,          //$34       NOP
		&and_zpage_x,   //$35       AND $44, X    bitwise AND with accumulator    2       4
		&rol_zpage_x,   //$36       ROL $44       Rotate Left                     2       6
		&rla_zpage_x,   //$37       RLA
		&sec,           //$38       SEC           Sets Carry flag                 1       2
		&and_absolute_y,//$39       AND $440&invalid, Y  bitwise AND with accumulator    3       4+
		&nop1,          //$3A       NOP
		&rla_absolute_y,//$3B       RLA
		&nop3,          //$3C       NOP
		&and_absolute_x,//$3D       AND $440&invalid, X  bitwise AND with accumulator    3       4+
		&rol_absolute_x,//$3E       ROL $44       Rotate Left                     3       7
		&rla_absolute_x,//$3F       RLA
		&rti,           //$40       RTI           Returns from Interrupt          1       6
		&eor_indirect_x,//$41       EOR           Exclusive OR                    2       6
		&invalid,
		&lse_indirect_x,//$43       LSE
		&nop2,          //$44       NOP
		&eor_zpage,     //$45       EOR           Exclusive OR                     2       3
		&lsr_zpage,     //$46       LSR           Logical Shift Right              2       5
		&lse_zpage,     //$47       LSE
		&pha,           //$48       PHA           PusH Acumulator                  1       3
		&eor_immediate,  //$49      EOR          Exclusive OR                     2       2
		&lsr_accumulator,//$4A      LSR          Logical Shift Right              1       2
		&invalid,
		&jmp_absolute,  //$4C       JMP           JuMP                              3       3
		&eor_absolute,  //$4D       EOR           Exclusive OR                      3       4
		&lsr_absolute,  //$4E       LSR           Logical Shift Right               3       6
		&lse_absolute,  //$4F       LSE
		&bvc,           //$50       BVC           Branch if Overflow Clear        2       2(+2)
		&eor_indirect_y,//$51       EOR           Exclusive OR                    2       5
		&invalid,
		&lse_indirect_y,//$53       LSE
		&nop2,          //$54       NOP
		&eor_zpage_x,   //$55       EOR           Exclusive OR                      2       4
		&lsr_zpage_x,   //$56       LSR           Logical Shift Right               2       6
		&lse_zpage_x,   //$57       LSE
		&cli,           //$58       CLI           CLear Interrupt flag              1       2,
		&eor_absolute_y,//$59       EOR           Exclusive OR                      3       4
		&nop1,          //$5A       NOP
		&lse_absolute_y,//$5B       LSE
		&nop3,          //$5C       NOP
		&eor_absolute_x, //$5D      EOR           Exclusive OR                    3       4
		&lsr_absolute_x, //$5E      LSR           Logical Shift Right             3       7
		&lse_absolute_x,  //$5F     LSE
		&rts,            //$60      RTS          Returns from Subroutine           1       6
		&adc_indirect_x, //$61      ADC ($44, X) ADd with Carry                    2       6
		&invalid,
		&rra_indirect_x,//$63       RRA
		&nop2,          //$64       NOP
		&adc_zpage,     //$65       ADC $44      ADd with Carry                    2       3
		&ror_zpage,     //$66       ROR $44      Rotate Right                      2       5
		&rra_zpage,     //$67       RRA
		&pla,           //$68       PLA           PuLl Acumulator                  1       4
		&adc_immediate, //$69       ADC #$44      ADd with Carry                   2       2
		&ror_accumulator,//$6A      ROR $44      Rotate Right                      1       2
		&invalid,
		&jmp_indirect,  //$6C       JMP          JuMP                              3       5
		&adc_absolute,  //$6D       ADC $4400    ADd with Carry                    3       4
		&ror_absolute,  //$6E       ROR $44      Rotate Right                      3       6
		&rra_absolute,  //$6F       RRA
		&bvs,           //$70       BVS           Branch if plus                    2       2(+2)
		&adc_indirect_y,//$71       ADC ($44), X   ADd with Carry                   2       5+
		&invalid,
		&rra_indirect_y,//$73       RRA
		&nop2,          //$74       NOP
		&adc_zpage_x,   //$75       ADC $44, X    ADd with Carry                    2       4
		&ror_zpage_x,   //$76       ROR $44      Rotate Right                       2       6
		&rra_zpage_x,   //$77       RRA
		&sei,           //$78       SEI           Sets Interrupt flag               1       2,
		&adc_absolute_y,//$79       ADC $440&invalid, Y  ADd with Carry             3       4+
		&nop1,          //$7A       NOP
		&rra_absolute_y,//$7B       RRA
		&nop3,          //$7C       NOP
		&adc_absolute_x,//$7D       ADC $440&invalid, X  ADd with Carry             3       4+
		&ror_absolute_x,//$7E       ROR $44      Rotate Right                       3       7
		&rra_absolute_x,//$7F       RRA
		&nop2,          //$80       NOP
		&sta_indirect_x,//$81       STA ($44,X)    STore Accumulator                 2       6
		&nop2,          //$82       NOP
		&axs_indirect_x,//$83       AXS                                              2       2
		&sty_zpage,     //$84       STX $44       STore Y Register                   2       3
		&sta_zpage,     //$85       STA $44       STore Accumulator                  2       2
		&stx_zpage,     //$86       STX $44       STore X Register                   2       2
		&axs_zpage,     //$87       AXS
		&dey,           //$88       DEY           Decrements Y                      1       2
		&nop2,          //$89       NOP
		&txa,           //$8A       TXA           Transfer X to A                   1       2
		&invalid,
		&sty_absolute,  //$8C       STX $4400     STore Y Register                  3       4
		&sta_absolute,  //$8D       STA $4400     STore Accumulator                 3       4
		&stx_absolute,  //$8E       STX $4400     STore X Register                  3       4
		&axs_absolute,  //$8F       AXS
		&bcc,           //$90       BCC           Branch if carry clear             2       2(+2)
		&sta_indirect_y,//$91       STA ($44),Y    STore Accumulator                 2       6
		&invalid,
		&invalid,
		&sty_zpage_x,   //$94       STX $44,Y      STore Y Register                  2       4
		&sta_zpage_x,   //$95       STA $44,X      STore Accumulator                 2       4
		&stx_zpage_y,   //96        STX $44, Y     STore X Register                  2       4
		&axs_zpage_y,   //$97       AXS
		&tya,           //$98       TYA           Transfer Y to A                    1       2
		&sta_absolute_y,//$99       STA $440&invalidY    STore Accumulator           3       5
		&txs,           //9A
		&invalid,
		&invalid,
		&sta_absolute_x,//$9D       STA $440&invalidX    STore Accumulator                 3       5
		&invalid,
		&invalid,
		&ldy_immediate, //A0        LDA #$44       LoaD Accumulator                  2       2
		&lda_indirect_x,//A1        LDA ($44,X)    LoaD Accumulator                  2       6
		&ldx_immediate, //A2        LDA #$44       LoaD Accumulator                  2       2
		&lax_indirect_x,//$A3       LAX $4400     Load Accumulator and X            2       6
		&ldy_zpage,     //$A4       LDA $44       LoaD Accumulator                  2       3
		&lda_zpage,     //$A5       LDA $44       LoaD Accumulator                  2       3
		&ldx_zpage,     //$A6       LDA $44       LoaD Accumulator                  2       3
		&lax_zpage,     //$A7       LAX 44        Load Accumulator and X            2       3
		&tay,           //$A8       TAY           Transfer A to Y                   1       2
		&lda_inmediate, //$A9       LDA #$44      LoaD Accumulator                  2       2
		&tax,           //$AA       TAX           Transfer A to X                   1       2
		&invalid,
		&ldy_absolute,  //$AC       LDY $4400     LoaD Y Register                    3       4
		&lda_absolute,  //AD        LDA $4400     LoaD Accumulator                   3       4
		&ldx_absolute,  //AE        LDA $4400     LoaD Accumulator                   3       4
		&lax_absolute,  //$AF       LAX $4400     Load Accumulator and X             3       4
		&bcs,           //$B0       BCS           Branch id carry set               2       2(+2)
		&lda_indirect_y,//B1        LDA ($44),Y   LoaD Accumulator                  2       5+
		&invalid,
		&lax_indirect_y,//$B3       LAX $4400     Load Accumulator and X             2       5+
		&ldy_zpage_x,   //$B4       LDY $44, X    LoaD Y Register                    2       6
		&lda_zpage_x,   //$B5       LDA $44, X    LoaD Accumulator                   2       4
		&ldx_zpage_y,   //$B6       LDX $44, Y    LoaD Accumulator                   2       4
		&lax_zpage_y,   //$B7       LAX $44, Y    Load Accumulator and X             2       4
		&clv,           //$B8       CLV           CLear Overflow flag                1       2,
		&lda_absolute_y,//$B9       LDA $440&invalidY   LoaD Accumulator                   3       4+
		&tsx,           //BA
		&invalid,
		&ldy_absolute_x,//$BC       LDY $440&invalidY    LoaD Y Register                    3       4+
		&lda_absolute_x,//BD        LDA $440&invalidX    LoaD Accumulator                   3       4+
		&ldx_absolute_y,//BE        LDA $440&invalidY    LoaD Accumulator                   3       4+
		&lax_absolute_y,  //$BF     LAX $4400,Y          Load Accumulator and X             3       4+
		&cpy_immediate,     //$C0   CPX #$44      Compare                           2       2
		&cmp_indirect_x,    //$C1   CMP ($44,X)   Compare                           2       6
		&nop2,          //$C2       NOP
		&dcm_indirect_x,//$C3       DCM $44       Decrement and CoMpare
		&cpy_zpage,     //$C4       CPX $44       Compare                           2       3
		&cmp_zpage,     //$C5       CMP $44       Compare                           2       3
		&dec_mem_zpage, //$C6       DEC $44       DEcrement Memory                  2       5
		&dcm_zpage,     //$C7       DCM $44       Decrement and CoMpare
		&iny,           //$C8       INY           Increments Y                      1       2
		&cmp_inmediate, //$C9       CMP #$44      Compare                           2       2
		&dex,           //$CA       DEX           Decrements X register             1       2
		&invalid,
		&cpy_absolute,    //$CC     CPX $4400     Compare                           3       4
		&cmp_absolute,    //$CD     CMP $4400     Compare                           3       4
		&dec_mem_absolute,//$CE     DEC $4400     DEcrement Memory                  3       6
		&dcm_absolute,    //$CF     DCM $4400     Decrement and CoMpare
		&bne,           //$D0       BNE           Branch now equals                 2       2(+2)
		&cmp_indirect_y,    //$D1   CMP ($44),X   Compare                           2       5+
		&invalid,
		&dcm_indirect_y,//$D3       DCM $44       Decrement and CoMpare
		&nop2,          //$D4       NOP
		&cmp_zpage_x,       //$D5   CMP $44,X     Compare                           2       4
		&dec_mem_zpage_x,   //$D6   DEC $44,X     DEcrement Memory                  2       6
		&dcm_zpage_x,       //$D7   DCM $44       Decrement and CoMpare
		&cld,               //$D8   CLD           CLear Decimal flag                1       2
		&cmp_absolute_y,    //$D9   CMP $440&invalidY   Compare                          3       4+
		&nop1,          //$DA       NOP
		&dcm_absolute_y,//$DF       DCM $4400     Decrement and CoMpare
		&nop3,          //$DC       NOP
		&cmp_absolute_x,    //$DD   CMP $440&invalidX   Compare                          3       4+
		&dec_mem_absolute_x,//$DE   DEC $440&invalidX   DEcrement Memory                 3       7
		&dcm_absolute_x,    //$DF   DCM $4400     Decrement and CoMpare
		&cpx_immediate,    //$E0    CPX #$44      Compare                          2       2
		&sbc_indirect_x,//$E1       SBC ($44,X)   SuBstract with Carry             2       6
		&nop2,          //$E2       NOP
		&ins_indirect_x,//$E3       INS
		&cpx_zpage,    //$E4        CPX $44       Compare                          2       3
		&sbc_zpage,     //$E5       SBC $44       SuBstract with Carry             2       3
		&inc_mem_zpage, //$E6       INC $44       INcrement Memory                 2       5
		&ins_zpage,     //$E7       INS
		&inx,           //$E8       INX           Increments X register            1       7
		&sbc_immediate, //$E9       SBC #$44      SuBstract with Carry             2       2
		&nop1,          //$EA       NOP           No OPeration                     1       2
		&sbc_immediate, //$EB       SBC #$44       Invalid opcode. works like sbc
		&cpx_absolute,  //$EC       CPX $4400       Compare                         3       4
		&sbc_absolute, //$ED        SBC $4400       SuBstract with Carry            3       4
		&inc_mem_absolute,//$EE     INC $4400       INcrement Memory                3       6
		&ins_absolute,  //$EF       INS
		&beq,           //$F0       BEQ           Branch if equals                 2       2(+2)
		&sbc_indirect_y,//$F1       SBC ($44),Y   SuBstract with Carry             2       5+
		&invalid,
		&ins_indirect_y,//$F3       INS
		&nop2,          //$F4       NOP
		&sbc_zpage_x,   //$F5       SBC $44,X     SuBstract with Carry             2       4
		&inc_mem_zpage_x,//$F6      INC $44, X    INcrement Memory                 2       6
		&ins_zpage_x,   //$E7       INS
		&sed,           //$F8       SED           Sets Decimal flag                1       2
		&sbc_absolute_y,//$F9       SBC $440&invalidY   SuBstract with Carry             3       4+
		&nop1,          //$FA       NOP
		&ins_absolute_y,//$EF       INS
		&nop3,          //$FC       NOP
		&sbc_absolute_x,//$FD       SBC $440&invalidX   SuBstract with Carry             3       4+
		&inc_mem_absolute_x,//$FE   INC $440&invalidX   INcrement Memory               3       7
		&ins_absolute_x,//$EF       INS
};

byte cpu_instruction() {
	static int cycle_before = 0;
	cycle_before = cpu_cyclesThisSec;
	decOpcode();
	exeOpcode();
	return (byte) (cpu_cyclesThisSec - cycle_before);
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
	word targetAddr = 0x0000;
	// This is a 6502 bug when instead of reading from $C0FF/$C100 it reads from $C0FF/$C000
	if ((addr & 0xFF) == 0xFF) {
		// Buggy code
		targetAddr = (rmem_b(addr & 0xFF00) << 8) + rmem_b(addr);
	} else {
		// Normal code
		targetAddr = rmem_w(addr);
	}

	return targetAddr;
}

void log_instruction(int num_params, const char *mnemonic, ...) {
	log_debug("%02X %02X ", PC, currentOpcode);
	for (uint i = 1; i <= num_params; i++) {
		log_debug("%02X ", rmem_b(PC + i));
	}
	log_debug("\t");
	if (num_params == 0) {
		log_debug("\t");
	}
	va_list args;
	va_start(args, mnemonic);
	vlog(mnemonic, ConsoleDebug, FileDebug, args);
	va_end(args);
	log_debug("\t\t\t");
	log_debug("A:%02X X:%02X Y:%02X ", A, X, Y);

	//Now for the status register. A bit of dirty code but the logs will be much clearer
	log_debug("P:");
	log_debug(bit_test(P, flagN) ? "N" : "n");
	log_debug(bit_test(P, flagV) ? "V" : "v");
	log_debug(bit_test(P, flagUnused) ? "U" : "u");
	log_debug(bit_test(P, flagB) ? "B" : "b");
	log_debug(bit_test(P, flagD) ? "D" : "d");
	log_debug(bit_test(P, flagI) ? "I" : "i");
	log_debug(bit_test(P, flagZ) ? "Z" : "z");
	log_debug(bit_test(P, flagC) ? "C" : "c");

	log_debug(" ");

	//And the rest of the registers
	log_debug("SP:%02X CYC:%d", SP, cpu_cyclesThisSec);
	log_debug("\n");
}