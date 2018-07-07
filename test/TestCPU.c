#include "TestCPU.h"

/**
 * Simply calls each function to test. It's going to get messy but what the hell!....
 */
void testOpcodes() {
	power_up(0);
	// We did something wrong with the BRK opcode function, need to be fixed
	test_BRK();
	test_ORA();
	test_ASL();
	test_JSR();
}

/**
 * For this test to succeed, after executing BRK, PC has to increase just by one,
 * the B flag and the Z flag have both to be 1
 */
void test_BRK() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cyclesThisSec;
	bit_clear(&P, flagB);
	assert(bit_test(P, flagB) == 0);
	wmem_const(BYTE, PC, 0x00); // brk opcode injected
	cpu_cycle();
	assert(cachedPC + 1 == PC);
	assert(bit_test(P, flagB) == 1);
	assert(bit_test(P, flagZ) == 1);
	assert(cachedCyclesThisSec + 7 == cyclesThisSec);
	printf("Test BRK passed!\n");
}

/**
 * For this test to succeed, PC has to increase by two or three and only Z and N flags can be modified
 */
void test_ORA() {
	// Testing ora_ind_x() through ora()
	int cachedPC = PC;
	int cachedCyclesThisSec = cyclesThisSec;
	wmem_const(BYTE, PC, 0x01); // ora_ind_x opcode injected
	wmem_const(BYTE, PC + 1, 0x42); // value injected at the next PC position
	A = 0x80; //Inject a value in the acumulator to do the "OR" with
	word addr = indirectx_addr(0x42);//ora_x will use this addr to get the value. So put it there
	wmem_const(BYTE, addr, 0x58);
	cpu_cycle();
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 6 == cyclesThisSec);
	assert(bit_test(P, flagN) == 1);
	assert(A == 0xD8);

	//testing ora_absolute_x
	cachedPC = PC;
	cachedCyclesThisSec = cyclesThisSec;
	wmem_const(BYTE, PC, 0x19);
	word param = 0x6959;
	wmem(WORD, PC + 1, (byte *) &param);
	Y = 0x10;
	addr = absolutey_addr((byte *) &param);
	wmem_const(WORD, addr, 0x58);
	A = 0x80;
	cpu_cycle();
	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 4 == cyclesThisSec);
	assert(bit_test(P, flagN) == 1);
	assert(A == 0xD8);

	printf("Test ORA passed!\n");
}

void test_ASL() {
	// Testing asl_accumulator()
	int cachedPC = PC;
	int cachedCyclesThisSec = cyclesThisSec;
	P = 0;
	wmem_const(BYTE, PC, 0x0A); // asl_accumulator opcode injected
	A = 0b01000101;
	cpu_cycle();
	assert(cachedPC + 1 == PC);
	assert(cachedCyclesThisSec + 2 == cyclesThisSec);
	assert(bit_test(P, flagN) == 1);
	assert(bit_test(P, flagC) == 0);
	assert(bit_test(P, flagZ) == 0);

	// Testing asl_zpage()
	cachedPC = PC;
	cachedCyclesThisSec = cyclesThisSec;
	P = 0;
	wmem_const(BYTE, PC, 0x06); // asl_zage opcode injected
	wmem_const(BYTE, PC + 1, 0x56); // where the data is save
	word addr = zeropage_addr((word) 0x56);
	wmem_const(BYTE, addr, 0b10000101);
	cpu_cycle();
	byte result = 0;
	rmem(BYTE, addr, &result);
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 5 == cyclesThisSec);
	assert(bit_test(P, flagN) == 0);
	assert(bit_test(P, flagC) == 1);
	assert(bit_test(P, flagZ) == 0);
	assert(result == 0b00001010);

	// Testing asl_absolute()
	cachedPC = PC;
	cachedCyclesThisSec = cyclesThisSec;
	P = 0;
	A = 0;
	byte param[2] = {0x59, 0x59};

	wmem_const(BYTE, PC, 0x0E); // asl_absolute opcode injected
	wmem(WORD, PC + 1, param);
	addr = absolute_addr(param);
	wmem_const(BYTE, addr, 0b00100101);
	cpu_cycle();
	rmem(BYTE, addr, &result);

	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 6 == cyclesThisSec);
	assert(bit_test(P, flagN) == 0);
	assert(bit_test(P, flagC) == 0);
	assert(bit_test(P, flagZ) == 1);
	assert(result == 0b01001010);

	printf("Test ASL passed!\n");
}

void test_JSR() {
	int cachedPC = PC;
	int cachedSP = SP;
	int cachedCyclesThisSec = cyclesThisSec;

	byte param[2] = {0x69, 0x69};

	wmem_const(BYTE, PC, 0x20);
	wmem(WORD, PC + 1, param);
	cpu_cycle();

	peek(WORD, param);
	assert(to_mem_addr(param) == cachedPC - 1);
	assert(PC == 0x6969);
	assert(cachedCyclesThisSec + 6 == cyclesThisSec);
	assert(cachedSP - 2 == SP);
	printf("Test JSR passed!\n");
}

