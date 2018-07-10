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
	test_PHP();
	test_PLP();
	test_PLA();
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
	wmem_b(PC, 0x00); // brk opcode injected
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
	wmem_b(PC, 0x01); // ora_ind_x opcode injected
	wmem_b(PC + 1, 0x42); // value injected at the next PC position
	A = 0x80; //Inject a value in the acumulator to do the "OR" with
	word addr = indirectx_addr(0x42);//ora_x will use this addr to get the value. So put it there
	wmem_b(addr, 0x58);
	cpu_cycle();
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 6 == cyclesThisSec);
	assert(bit_test(P, flagN) == 1);
	assert(A == 0xD8);

	//testing ora_absolute_x
	cachedPC = PC;
	cachedCyclesThisSec = cyclesThisSec;
	wmem_b(PC, 0x19);
	word param = 0x6959;
	wmem_w(PC + 1, 0x6959);
	Y = 0x10;
	addr = absolutey_addr(param);
	wmem_w(addr, 0x0058);
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
	wmem_b(PC, 0x0A); // asl_accumulator opcode injected
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
	wmem_b(PC, 0x06); // asl_zage opcode injected
	wmem_b(PC + 1, 0x56); // where the data is save
	word addr = zeropage_addr((word) 0x56);
	wmem_b(addr, 0b10000101);
	cpu_cycle();
	byte result = rmem_b(addr);
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
	word param = 0x5959;
	wmem_b(PC, 0x0E); // asl_absolute opcode injected
	wmem_w(PC + 1, param);
	addr = absolute_addr(param);
	wmem_b(addr, 0b00100101);
	cpu_cycle();
	result = rmem_b(addr);

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

	word param = 0x6969;

	wmem_b(PC, 0x20);
	wmem_w(PC + 1, param);
	cpu_cycle();

	assert(peek_w() == cachedPC - 1);
	assert(PC == 0x6969);
	assert(cachedCyclesThisSec + 6 == cyclesThisSec);
	assert(cachedSP - 2 == SP);
	printf("Test JSR passed!\n");
}

void test_PHP() { //Hehehe.... PHP..... :'D
	int cachedPC = PC;
	int cachedSP = SP;
	int cachedCyclesThisSec = cyclesThisSec;

	P = 0b01101010;//Push a random status vector on to the register
	wmem_b(PC, 0x08);//Inject the opcode
	cpu_cycle();
	assert(peek_b() == P);
	assert(cachedPC + 1 == PC);
	assert(cachedSP - 1 == SP);
	assert(cachedCyclesThisSec + 3 == cyclesThisSec);
	printf("Test PHP passed!\n");
}

void test_PLP() {
	int cachedPC = PC;
	int cachedSP;
	int cachedCyclesThisSec = cyclesThisSec;

	byte testValue = P = 0b01101111;
	push_b(P);
	P = 0b11111111;//Now change it to prove that it does it's job correctly
	wmem_b(PC, 0x28);// Inject the opcode
	cachedSP = SP;
	cpu_cycle();

	assert(P == testValue);
	assert(cachedPC + 1 == PC);
	assert(cachedSP + 1 == SP);
	assert(cachedCyclesThisSec + 4 == cyclesThisSec);
	printf("Test PLP passed!\n");
}

void test_PLA() {
	int cachedPC = PC;
	int cachedSP;
	int cachedCyclesThisSec = cyclesThisSec;

	//Test Zero flag
	byte testValue = A = 0b00000000;
	push_b(A);
	A = 0x05;//Overwrite with a random value. It doesn't matter
	wmem_b(PC, 0x68);//Inject the opcode
	cachedSP = SP;
	cpu_cycle();

	assert(A == testValue);
	assert(bit_test(P, flagZ) == 1);
	assert(bit_test(P, flagN) == 0);
	assert(cachedPC + 1 == PC);
	assert(cachedSP + 1 == SP);
	assert(cachedCyclesThisSec + 4 == cyclesThisSec);

	//Test negative flag
	P = 0;
	cachedPC = PC;
	cachedCyclesThisSec = cyclesThisSec;
	testValue = A = 0b11001001;
	push_b(A);
	A = 0x05;//Overwrite with a random value. It doesn't matter
	wmem_b(PC, 0x68);
	cachedSP = SP;
	cpu_cycle();
	assert(A == testValue);
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagN) == 1);
	assert(cachedPC + 1 == PC);
	assert(cachedSP + 1 == SP);
	assert(cachedCyclesThisSec + 4 == cyclesThisSec);

	printf("Test PLA passed!\n");
}

void test_PHA() {
	int cachedPC = PC;
	int cachedSP = SP;
	int cachedCyclesThisSec = cyclesThisSec;

	A = 0x69;
	wmem_b(PC, 0x48);//Inject the opcode
	cpu_cycle();
	assert(peek_b() == A);
	assert(cachedPC + 1 == PC);
	assert(cachedSP - 1 == SP);
	assert(cachedCyclesThisSec + 3 == cyclesThisSec);
}

void test_AND() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cyclesThisSec;

	//Immediate test
	A = 0x05;
	wmem_b(PC, 0x29);
	wmem_b(PC + 1, 0x0A);
	cpu_cycle();
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 2 == cyclesThisSec);
	assert(A == 0x0F);

	//Zero page Test
	A = 0b00000111;
	wmem_b(PC, 0x25);
	wmem_b(PC + 1, 0xFD);
	wmem_b(0xFD, 0b01111000);
	cpu_cycle();
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 3 == cyclesThisSec);
	assert(A == 0);
	assert(bit_test(P, flagZ)); //In this case A should be zero so we're testing the flag

	//Absolute test
	A = 0b10000111;
	wmem_b(PC, 0x2D);
	wmem_w(PC + 1, 0x1234);
	wmem_b(0x1234, 0b10000000);
	cpu_cycle();
	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 4 == cyclesThisSec);
	assert(A == 0b10000000);
	assert(bit_test(P, flagN));
}