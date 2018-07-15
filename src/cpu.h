#ifndef NESULATOR_CPU_H
#define NESULATOR_CPU_H

#include "nes.h"
#include "Utils.h"

#define OPCODE_COUNT 256

typedef void (*gen_opcode_func)();

enum StateFlagEnum {
	flagC = 0, flagZ = 1, flagI = 2, flagD = 3, flagB = 4, flagV = 6, flagN = 7
};

byte A;     // Accumulator, deal with carry, overflow and so on...
byte X;     // General purpose
byte Y;     // General purpose
word PC;    // Program Counter
byte SP;    // Stack Pointer, from 0x100 to 0x1FF address

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
 * Returns a byte by reading the PC + 1 and treating it as a zero page parameter. See addressing modes
 */
byte zpage_param();

/**
 * Returns a byte by reading the PC + 1 and treating it as a zero page X parameter. See addressing modes
 */
byte zpagex_param();

/**
 * Returns a byte by reading the PC + 1 and treating it as a zero page Y parameter. See addressing modes
 */
byte zpagey_param();

/**
 * Returns a byte by reading the PC + 1 and treating it as a absolute parameter. See addressing modes
 */
byte absolute_param();

/**
 * Returns a byte by reading the PC + 1 and treating it as a absolute X parameter. See addressing modes
 */
byte absolutex_param();

/**
 * Returns a byte by reading the PC + 1 and treating it as a absolute Y parameter. See addressing modes
 */
byte absolutey_param();

/**
 * Returns a byte by reading the PC + 1 and treating it as a indirect X parameter. See addressing modes
 */
byte indirectx_param();

/**
 * Returns a byte by reading the PC + 1 and treating it as a indirect Y parameter. See addressing modes
 */
byte indirecty_param();


/**
 * Executes an instruction on the cpu. Internally it would decode the opcode and execute it (for now)
 * In  the future it would also have to call some routine to match the original cpu speed
 */
void cpu_cycle();


/**
 * BRK causes a non-maskable interrupt and increments the program counter by one.
 * Therefore an RTI will go to the address of the BRK +2 so that BRK may be used to replace a two-byte instruction
 * for debugging and the subsequent RTI will be correct.
 */
void brk();

/**
 * Just nop
 */
void nop();

//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////ORA REGION////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * An inclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory.
 * Flags Z and N affected
 */
void ora(byte b, int cycles, int pcIncrease);

/**
 * Bitwise OR with accumulator
 */
void ora_ind_x();

/**
 * Bitwise OR with accumulator
 */
void ora_ind_y();

/**
 * Bitwise OR with accumulator
 */
void ora_zpage();

/**
 * Bitwise OR with accumulator
 */
void ora_immediate();

/**
 * Bitwise OR with accumulator
 */
void ora_absolute();

/**
 * Bitwise OR with accumulator
 */
void ora_zpage_x();

/**
 * Bitwise OR with accumulator
 */
void ora_absolute_y();

/**
 * Bitwise OR with accumulator
 */
void ora_absolute_x();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////ASL (Arithmetic Shift Left) REGION/////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * Arithmetic Shift Left on the accumulator
 */
void asl(byte *b, int cycles, int pcIncrease);

/**
 * Arithmetic Shift Left on the accumulator
 */
void asl_accumulator();

/**
 * Arithmetic Shift Left on the accumulator
 */
void asl_zpage();

/**
 * Arithmetic Shift Left on the accumulator
 */
void asl_zpage_x();

/**
 * Arithmetic Shift Left on the accumulator
 */
void asl_absolute();

/**
 * Arithmetic Shift Left on the accumulator
 */
void asl_absolute_x();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////JSR (Jump to SubRoutine) REGION////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * Jump to SubRoutine
 */
void jsr_absolute();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PHP (PusH Processor status) REGION/////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * PusH Processor status makes a push of the status flag onto the stack
 */
void php();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PLP (PuLl Processor status) REGION/////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * PuLl Processor status makes a pull from the stack onto the status flag
 */
void plp();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PHA (PusH Acumulator) REGION/////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * PusH Acumulator makes a push from the acumulator register onto the stack
 */
void pha();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PLA (PuLl Acumulator) REGION/////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * PuLl Acumulator makes a pull from the stack onto the acumulator register
 */
void pla();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////AND (bitwise AND accumulator) REGION///////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * Bitwise and operation on the accumulator
 */
void and(byte value, int cycles, int pcIncrease);

/**
 * Bitwise and operation on the accumulator
 */
void and_immediate();

/**
 * Bitwise and operation on the accumulator
 */
void and_zpage();

/**
 * Bitwise and operation on the accumulator
 */
void and_zpage_x();

/**
 * Bitwise and operation on the accumulator
 */
void and_absolute();

/**
 * Bitwise and operation on the accumulator
 */
void and_absolute_x();

/**
 * Bitwise and operation on the accumulator
 */
void and_absolute_y();

/**
 * Bitwise and operation on the accumulator
 */
void and_indirect_x();

/**
 * Bitwise and operation on the accumulator
 */
void and_indirect_y();


//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////BIT (BIt Test) REGION//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * BIt Test checks if one or more bits of a memory position are set
 */
void bit(byte value, int cycles, int pcIncrease);

/**
 * BIt Test checks if one or more bits of a memory position are set
 */
void bit_zpage();

/**
 * BIt Test checks if one or more bits of a memory position are set
 */
void bit_absolute();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Flags REGION/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * Sets the given flag to the 'isSet' value
 */
void set_flag_value(byte flag, int isSet);

/**
 * Clear carry flag
 */
void clc();

/**
 * Clear decimal mode
 */
void cld();

/**
 * Clear interrupt flag
 */
void cli();

/**
 * Clear overflow flag
 */
void clv();

/**
 * Sets the carry flag
 */
void sec();

/**
 * Sets the interrupt flag
 */
void sei();

/**
 * Set the decimal flag
 */
void sed();

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Registers REGION///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/**
 * Transfers one register to another
 */
void transfer_reg(byte *from_reg, byte *to_reg);

/**
 * Decrements the given register
 */
void dec_reg(byte *reg);

/**
 * Increments the given register
 */
void inc_reg(byte *reg);

/**
 * Transfer A to X
 */
void tax();

/**
 * Transfer X to A
 */
void txa();

/**
 * Decrement X
 */
void dex();

/**
 * Increment X
 */
void inx();

/**
 * Transfer A to Y
 */
void tay();

/**
 * Transfer Y to A
 */
void tya();

/**
 * Decrement Y
 */
void dey();

/**
 * Increment Y
 */
void iny();

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////ADC REGION//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/**
 * Add with carry
 */
void adc(byte value, int cycles, int pcIncrease);

/**
 * Add with carry
 */
void adc_immediate();

/**
 * Add with carry
 */
void adc_zpage();

/**
 * Add with carry
 */
void adc_zpage_x();

/**
 * Add with carry
 */
void adc_absolute();

/**
 * Add with carry
 */
void adc_absolute_x();

/**
 * Add with carry
 */
void adc_absolute_y();

/**
 * Add with carry
 */
void adc_indirect_x();

/**
 * Add with carry
 */
void adc_indirect_y();

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////BRANCH REGION///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/**
 * Tries to branch the PC by adding the relative displacement (value) if the flag's value
 * has the req_flag_value.
 */
void try_branch(byte flag, int req_flag_val);

void bpl();

void bmi();

void bvc();

void bvs();

void bcc();

void bcs();

void bne();

void beq();



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////SBC REGION//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/**
 * Substract with carry
 */
void sbc(byte value, int cycles, int pcIncrease);

/**
 * Substract with carry
 */
void sbc_immediate();

/**
 * Substract with carry
 */
void sbc_zpage();

/**
 * Substract with carry
 */
void sbc_zpage_x();

/**
 * Substract with carry
 */
void sbc_absolute();

/**
 * Substract with carry
 */
void sbc_absolute_x();

/**
 * Substract with carry
 */
void sbc_absolute_y();

/**
 * Substract with carry
 */
void sbc_indirect_x();

/**
 * Substract with carry
 */
void sbc_indirect_y();

#endif //NESULATOR_CPU_H
