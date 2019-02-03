#ifndef NESULATOR_CPU_H
#define NESULATOR_CPU_H

#include "utils/Utils.h"

#define OPCODE_COUNT 256

typedef void (*gen_opcode_func)();

enum StateFlagEnum {
	flagC = 0, flagZ = 1, flagI = 2, flagD = 3, flagB = 4, flagUnused = 5, flagV = 6, flagN = 7
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
int cpu_cyclesThisSec; //Counter of elapsed cycles (Hz) this current second.
int cpu_cyclesLastSec; //Counter of elapsed cycles last second. Used by the GUI to keep accurate track of the speed
int cpu_speed;  //Speed of the CPU in Hz. Used to slow down the emulation to match the NES's clock speed
int nanoseconds_cpu_cycle; // Nanoseconds that would take the given CPU per cycle given a certain speed
byte currentOpcode; // The opcode of this cycle
uint cpu_running;

/**
 * Main function for the CPU. It runs on a separate thread, and it takes care of keeping the right speed
 */
void *cpu_run(void *arg);

/**
 * Initializes a CPU with the specified clock speed. This will set every register and pointer
 * to the right value. Basically it simulates switching on the NES.
 */
void cpu_power_up(int clock_speed);

/**
 * Modifies the current CPU clock speed
 */
void cpu_set_speed(int speed_hertzs);

/**
 * Resets the PC to the value held in the reset vector $FFFC and $FFFD
 */
void reset_pc();

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
byte absolutex_param(bool check_page_crossed);

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
 * The instruction contains a 16 bit address which identifies the location of the least significant byte of another
 * 16 bit memory address which is the real target of the instruction.
 */
word indirect_param();

/**
 * Executes an instruction on the cpu. Internally it would decode the opcode and execute it (for now)
 * In  the future it would also have to call some routine to match the original cpu speed
 */
byte cpu_instruction();

/**
 * Log an instruction with all the CPU information at that point: PC and registers.
 */
void log_instruction(int num_params, const char *mnemonic, ...);


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////INTERRUPTIONS REGION////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * BRK causes a non-maskable interrupt and increments the program counter by one.
 * Therefore an RTI will go to the address of the BRK +2 so that BRK may be used to replace a two-byte instruction
 * for debugging and the subsequent RTI will be correct.
 */
void breakpoint();

void nmi();


//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////NOP REGION////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/*
 * There are a bunch of invalid opcodes that are NOPs. This is a workaround to fix that.
 */
/**
 * The generic nop
 */
void nop(int cycles, int pcIncrease);

/**
 * Valid NOP opcode
 */
void nop1();

/**
 * Invalid NOP opcode
 */
void nop2();

/**
 * Invalid NOP opcode
 */
void nop3();

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
///////////////////////////PHA (PusH Acumulator) REGION///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * PusH Acumulator makes a push from the acumulator register onto the stack
 */
void pha();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PLA (PuLl Acumulator) REGION///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * PuLl Acumulator makes a pull from the stack onto the acumulator register
 */
void pla();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////TSX (Transfer Stack pointer to X) REGION///////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void tsx();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////TXS (Transfer X to Stack pointer) REGION///////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void txs();

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
void try_branch(byte flag, int req_flag_val, const char *mnemonic);

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

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////MEMORY INC/DEC REGION//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/**
 * Applies the delta to the content of the memory address. Sets the Z and N flags as appropiate
 */
void delta_memory(word memAddr, int delta, int cycles, int pcIncrease);

void inc_mem(word memAddr, int cycles, int pcIncrease);

void dec_mem(word memAddr, int cycles, int pcIncrease);

void inc_mem_zpage();

void inc_mem_zpage_x();

void inc_mem_absolute();

void inc_mem_absolute_x();

void dec_mem_zpage();

void dec_mem_zpage_x();

void dec_mem_absolute();

void dec_mem_absolute_x();


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////LOAD REGISTERS REGION///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/**
 * Loads the specified value onto the pointed register
 * the mnemonic is only used for logging purposes
 */
void load_register(byte *regPtr, byte value, int cycles, int pcIncrease, const char *regMnemonic);

void lda_inmediate();

void lda_zpage();

void lda_zpage_x();

void lda_absolute();

void lda_absolute_x();

void lda_absolute_y();

void lda_indirect_x();

void lda_indirect_y();

void ldx_immediate();

void ldx_zpage();

void ldx_zpage_y();

void ldx_absolute();

void ldx_absolute_y();

void ldy_immediate();

void ldy_zpage();

void ldy_zpage_x();

void ldy_absolute();

void ldy_absolute_x();

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////STORE REGISTERS REGION///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/**
 * Loads the specified value onto the pointed register
 * The mnmemonic is used for logging
 */
void store_register(byte reg, word memAddr, int cycles, int pcIncrease, const char *regMnemonic);

void sta_zpage();

void sta_zpage_x();

void sta_absolute();

void sta_absolute_x();

void sta_absolute_y();

void sta_indirect_x();

void sta_indirect_y();

void stx_zpage();

void stx_zpage_y();

void stx_absolute();

void sty_zpage();

void sty_zpage_x();

void sty_absolute();

////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////ReTurns REGION//////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/**
 * Returns from subroutine
 */
void rts();

/**
 * Returns from interrupt
 */
void rti();

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////COMPARE REGISTERS REGION///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/**
 * Compares the specified value with the pointed register
 */
void compare_register(byte *regPtr, byte value, int cycles, int pcIncrease, const char *regMnemonic);

void cmp_inmediate();

void cmp_zpage();

void cmp_zpage_x();

void cmp_absolute();

void cmp_absolute_x();

void cmp_absolute_y();

void cmp_indirect_x();

void cmp_indirect_y();

void cpx_immediate();

void cpx_zpage();

void cpx_absolute();

void cpy_immediate();

void cpy_zpage();

void cpy_absolute();


//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////LSR (Logical Shift Right) REGION///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * Logical Shift Right on the accumulator
 */
void lsr(byte *b, int cycles, int pcIncrease);

/**
 * Logical Shift Right on the accumulator
 */
void lsr_accumulator();

/**
 * Logical Shift Right on the accumulator
 */
void lsr_zpage();

/**
 * Logical Shift Right on the accumulator
 */
void lsr_zpage_x();

/**
 * Logical Shift Right on the accumulator
 */
void lsr_absolute();

/**
 * Logical Shift Right on the accumulator
 */
void lsr_absolute_x();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////JMP REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////Rotate REGION//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * Rotate left
 */
void rol(byte *b, int cycles, int pcIncrease);

/**
 * Rotate left
 */
void rol_accumulator();

/**
 * Rotate left
 */
void rol_zpage();

/**
 * Rotate left
 */
void rol_zpage_x();

/**
 * Rotate left
 */
void rol_absolute();

/**
 * Rotate left
 */
void rol_absolute_x();

/**
 * Rotate Right
 */
void ror(byte *b, int cycles, int pcIncrease);

/**
 * Rotate Right
 */
void ror_accumulator();

/**
 * Rotate Right
 */
void ror_zpage();

/**
 * Rotate Right
 */
void ror_zpage_x();

/**
 * Rotate Right
 */
void ror_absolute();

/**
 * Rotate Right
 */
void ror_absolute_x();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////EOR REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * Exclusive OR
 */
void eor(byte value, int cycles, int pcIncrease);

void eor_immediate();

void eor_zpage();

void eor_zpage_x();

void eor_absolute();

void eor_absolute_x();

void eor_absolute_y();

void eor_indirect_x();

void eor_indirect_y();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////JMP REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void jmp(word addr, int cycles);

void jmp_absolute();

void jmp_indirect();



//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////LAX REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * Transfers the content of a memory location to both the X and the A registers
 */
void lax(byte value, int cycles, int pcIncrease);

void lax_absolute();

void lax_absolute_y();

void lax_zpage();

void lax_zpage_y();

void lax_indirect_x();

void lax_indirect_y();


//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////AXS REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * AXS ANDs the contents of the A and X registers (without changing the
 * contents of either register) and stores the result in memory.
 * AXS does not affect any flags in the processor status register.
 */
void axs(word addr, int cycles, int pcIncrease);

void axs_indirect_x();

void axs_zpage();

void axs_zpage_y();

void axs_absolute();


//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////DCM (DCP) REGION///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * This opcode DECs the contents of a memory location and then CMPs the result with the A register.
 */
void dcm(word addr, int cycles, int pcIncrease);

void dcm_absolute();

void dcm_absolute_x();

void dcm_absolute_y();

void dcm_zpage();

void dcm_zpage_x();

void dcm_indirect_x();

void dcm_indirect_y();


//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////INS (ISC) REGION///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * This opcode INCs the contents of a memory location and then SBCs the result from the A register.
 */
void ins(word addr, int cycles, int pcIncrease);

void ins_absolute();

void ins_absolute_x();

void ins_absolute_y();

void ins_zpage();

void ins_zpage_x();

void ins_indirect_x();

void ins_indirect_y();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////ASO (SLO) REGION///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * This opcode ASLs the contents of a memory location and then ORs the result with the accumulator.
 */
void aso(word addr, int cycles, int pcIncrease);

void aso_absolute();

void aso_absolute_x();

void aso_absolute_y();

void aso_zpage();

void aso_zpage_x();

void aso_indirect_x();

void aso_indirect_y();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////RLA REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * RLA ROLs the contents of a memory location and then ANDs the result with the accumulator.
 */
void rla(word addr, int cycles, int pcIncrease);

void rla_absolute();

void rla_absolute_x();

void rla_absolute_y();

void rla_zpage();

void rla_zpage_x();

void rla_indirect_x();

void rla_indirect_y();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////LSE REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * LSE LSRs the contents of a memory location and then EORs the result with the accumulator.
 */
void lse(word addr, int cycles, int pcIncrease);

void lse_absolute();

void lse_absolute_x();

void lse_absolute_y();

void lse_zpage();

void lse_zpage_x();

void lse_indirect_x();

void lse_indirect_y();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////RRA REGION/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * RRA RORs the contents of a memory location and then ADCs the result with the accumulator.
 */
void rra(word addr, int cycles, int pcIncrease);

void rra_absolute();

void rra_absolute_x();

void rra_absolute_y();

void rra_zpage();

void rra_zpage_x();

void rra_indirect_x();

void rra_indirect_y();

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////Invalid Opcodes REGION/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void invalid();

#endif //NESULATOR_CPU_H
