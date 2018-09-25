#include "TestCPU.h"

/**
 * Simply calls each function to test. It's going to get messy but what the hell!....
 */
void testOpcodes() {
	cpu_power_up(0);
	//// We did something wrong with the BRK opcode function, need to be fixed
	test_BRK();
	test_ORA();
	test_ASL();
	test_JSR();
	test_PHP();
	test_PLP();
	test_PLA();
	test_PHA();
	test_AND();
	test_BIT();
	test_FLAGS();
	test_REGISTERS();
	test_ADC();
	test_BRANCH();
	test_NOP();
	test_SBC();
	test_INCDECMEM();
	test_LOADREGISTER();
	test_STOREREGISTER();
	test_RTS();
	test_RTI();
	test_COMPAREREGISTER();
	test_LSR();
	test_ROTATE();
	test_EOR();
	test_JMP();

	//Now for the real deal. Test the NESTEST ROM!
	test_NESTEST();
}

/**
 * For this test to succeed, after executing BRK, PC has to increase just by one,
 * the B flag and the Z flag have both to be 1
 */
void test_BRK() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;
	bit_clear(&P, flagB);
	assert(bit_test(P, flagB) == 0);
	wmem_b(PC, 0x00); // breakpoint opcode injected
	cpu_instruction();
	assert(cachedPC + 1 == PC);
	assert(bit_test(P, flagB) == 1);
	assert(cachedCyclesThisSec + 7 == cpu_cyclesThisSec);
	printf("Test BRK passed!\n");
}

/**
 * For this test to succeed, PC has to increase by two or three and only Z and N flags can be modified
 */
void test_ORA() {
	// Testing ora_ind_x() through ora()
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;
	wmem_b(PC, 0x01); // ora_ind_x opcode injected
	wmem_b(PC + 1, 0x42); // value injected at the next PC position
	A = 0x80; //Inject a value in the acumulator to do the "OR" with
	word addr = indirectx_addr(0x42);//ora_x will use this addr to get the value. So put it there
	wmem_b(addr, 0x58);
	cpu_instruction();
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 6 == cpu_cyclesThisSec);
	assert(bit_test(P, flagN) == 1);
	assert(A == 0xD8);

	//testing ora_absolute_x
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	wmem_b(PC, 0x19);
	word param = 0x6959;
	wmem_w(PC + 1, 0x6959);
	Y = 0x10;
	addr = absolutey_addr(param);
	wmem_w(addr, 0x0058);
	A = 0x80;
	cpu_instruction();
	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 4 == cpu_cyclesThisSec);
	assert(bit_test(P, flagN) == 1);
	assert(A == 0xD8);

	printf("Test ORA passed!\n");
}

void test_ASL() {
	// Testing asl_accumulator()
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;
	P = 0;
	wmem_b(PC, 0x0A); // asl_accumulator opcode injected
	A = 0b01000101;
	cpu_instruction();
	assert(cachedPC + 1 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);
	assert(bit_test(P, flagN) == 1);
	assert(bit_test(P, flagC) == 0);
	assert(bit_test(P, flagZ) == 0);

	// Testing asl_zpage()
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	P = 0;
	wmem_b(PC, 0x06); // asl_zage opcode injected
	wmem_b(PC + 1, 0x56); // where the data is save
	word addr = zpage_addr((word) 0x56);
	wmem_b(addr, 0b10000101);
	cpu_instruction();
	byte result = rmem_b(addr);
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 5 == cpu_cyclesThisSec);
	assert(bit_test(P, flagN) == 0);
	assert(bit_test(P, flagC) == 1);
	assert(bit_test(P, flagZ) == 0);
	assert(result == 0b00001010);

	// Testing asl_absolute()
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	P = 0;
	A = 0;
	word param = 0x5959;
	wmem_b(PC, 0x0E); // asl_absolute opcode injected
	wmem_w(PC + 1, param);
	addr = absolute_addr(param);
	wmem_b(addr, 0b00100101);
	cpu_instruction();
	result = rmem_b(addr);

	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 6 == cpu_cyclesThisSec);
	assert(bit_test(P, flagN) == 0);
	assert(bit_test(P, flagC) == 0);
	assert(bit_test(P, flagZ) == 1);
	assert(result == 0b01001010);

	printf("Test ASL passed!\n");
}

void test_JSR() {
	int cachedPC = PC;
	int cachedSP = SP;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	word param = 0x6969;

	wmem_b(PC, 0x20);
	wmem_w(PC + 1, param);
	cpu_instruction();

	assert(peek_w() == cachedPC + 2);
	assert(PC == 0x6969);
	assert(cachedCyclesThisSec + 6 == cpu_cyclesThisSec);
	assert(cachedSP - 2 == SP);
	printf("Test JSR passed!\n");
}

void test_PHP() { //Hehehe.... PHP..... :'D
	int cachedPC = PC;
	int cachedSP = SP;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	P = 0b01101010;//Push a random status vector on to the register
	wmem_b(PC, 0x08);//Inject the opcode
	cpu_instruction();
	assert(peek_b() == P);
	assert(cachedPC + 1 == PC);
	assert(cachedSP - 1 == SP);
	assert(cachedCyclesThisSec + 3 == cpu_cyclesThisSec);
	printf("Test PHP passed!\n");
}

void test_PLP() {
	int cachedPC = PC;
	int cachedSP;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	byte testValue = P = 0b01101111;
	push_b(P);
	P = 0b11111111;//Now change it to prove that it does it's job correctly
	wmem_b(PC, 0x28);// Inject the opcode
	cachedSP = SP;
	cpu_instruction();

	assert(P == testValue);
	assert(cachedPC + 1 == PC);
	assert(cachedSP + 1 == SP);
	assert(cachedCyclesThisSec + 4 == cpu_cyclesThisSec);
	printf("Test PLP passed!\n");
}

void test_PLA() {
	int cachedPC = PC;
	int cachedSP;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	//Test Zero flag
	byte testValue = A = 0b00000000;
	push_b(A);
	A = 0x05;//Overwrite with a random value. It doesn't matter
	wmem_b(PC, 0x68);//Inject the opcode
	cachedSP = SP;
	cpu_instruction();

	assert(A == testValue);
	assert(bit_test(P, flagZ) == 1);
	assert(bit_test(P, flagN) == 0);
	assert(cachedPC + 1 == PC);
	assert(cachedSP + 1 == SP);
	assert(cachedCyclesThisSec + 4 == cpu_cyclesThisSec);

	//Test negative flag
	P = 0;
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	testValue = A = 0b11001001;
	push_b(A);
	A = 0x05;//Overwrite with a random value. It doesn't matter
	wmem_b(PC, 0x68);
	cachedSP = SP;
	cpu_instruction();
	assert(A == testValue);
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagN) == 1);
	assert(cachedPC + 1 == PC);
	assert(cachedSP + 1 == SP);
	assert(cachedCyclesThisSec + 4 == cpu_cyclesThisSec);

	printf("Test PLA passed!\n");
}

void test_PHA() {
	int cachedPC = PC;
	int cachedSP = SP;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	A = 0x69;
	wmem_b(PC, 0x48);//Inject the opcode
	cpu_instruction();
	assert(peek_b() == A);
	assert(cachedPC + 1 == PC);
	assert(cachedSP - 1 == SP);
	assert(cachedCyclesThisSec + 3 == cpu_cyclesThisSec);

	printf("Test PHA passed!\n");
}

void test_AND() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	//Immediate test
	A = 0x0B;
	wmem_b(PC, 0x29);
	wmem_b(PC + 1, 0x0A);
	cpu_instruction();
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);
	assert(A == 0x0A);

	//Zero page Test
	A = 0b00000111;
	wmem_b(PC, 0x25);
	wmem_b(PC + 1, 0xFD);
	wmem_b(0xFD, 0b01111000);
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	P = 0;
	cpu_instruction();
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 3 == cpu_cyclesThisSec);
	assert(A == 0);
	assert(bit_test(P, flagZ)); //In this case A should be zero so we're testing the flag

	//Absolute test
	A = 0b10000111;
	wmem_b(PC, 0x2D);
	wmem_w(PC + 1, 0x1234);
	wmem_b(0x1234, 0b10000000);
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	P = 0;
	cpu_instruction();
	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 4 == cpu_cyclesThisSec);
	assert(A == 0b10000000);
	assert(bit_test(P, flagN));
	assert(bit_test(P, flagC));

	printf("Test AND passed!\n");
}

void test_BIT() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	A = 0x05;
	wmem_b(PC, 0x24);
	wmem_b(PC + 1, 0xFD);
	wmem_b(0xFD, 0x72);
	cpu_instruction();
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 3 == cpu_cyclesThisSec);
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagV));
	assert(bit_test(P, flagN) == 0);

	printf("Test BIT passed!\n");
}

void test_FLAGS() {
	//CLC
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	bit_set(&P, flagC);
	wmem_b(PC, 0x18);
	cpu_instruction();
	assert(bit_test(P, flagC) == 0);
	assert(cachedPC + 1 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	//PC and cycles are constant. Only test once. More than enough

	//CLD
	bit_set(&P, flagD);
	wmem_b(PC, 0xD8);
	cpu_instruction();
	assert(bit_test(P, flagD) == 0);

	//CLI
	bit_set(&P, flagI);
	wmem_b(PC, 0x58);
	cpu_instruction();
	assert(bit_test(P, flagI) == 0);

	//CLV
	bit_set(&P, flagV);
	wmem_b(PC, 0xB8);
	cpu_instruction();
	assert(bit_test(P, flagV) == 0);

	//SEC
	bit_clear(&P, flagC);
	wmem_b(PC, 0x38);
	cpu_instruction();
	assert(bit_test(P, flagC));

	//SEI
	bit_clear(&P, flagI);
	wmem_b(PC, 0x78);
	cpu_instruction();
	assert(bit_test(P, flagI));

	//SED
	bit_clear(&P, flagD);
	wmem_b(PC, 0xF8);
	cpu_instruction();
	assert(bit_test(P, flagD));

	printf("Test FLAGS passed!\n");
}

void test_REGISTERS() {
	//TAX
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	A = 0x05;
	wmem_b(PC, 0xAA);
	cpu_instruction();
	assert(X == A);
	assert(cachedPC + 1 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	//PC and cycles are constant. Only test once. More than enough

	//TXA
	X = 0x06;
	wmem_b(PC, 0x8A);
	cpu_instruction();
	assert(X == A);

	//DEX
	X = 0x06;
	wmem_b(PC, 0xCA);
	cpu_instruction();
	assert(X == 0x05);

	//INX
	X = 0x06;
	wmem_b(PC, 0xE8);
	cpu_instruction();
	assert(X == 0x07);

	//TAY
	A = 0x06;
	wmem_b(PC, 0xA8);
	cpu_instruction();
	assert(A == Y);

	//TYA
	Y = 0x07;
	wmem_b(PC, 0x98);
	cpu_instruction();
	assert(Y == A);

	//DEY
	Y = 0x06;
	wmem_b(PC, 0x88);
	cpu_instruction();
	assert(Y == 0x05);

	//INY
	Y = 0x06;
	wmem_b(PC, 0xC8);
	cpu_instruction();
	assert(X == 0x07);

	printf("Test REGISTERS passed!\n");
}

void test_ADC() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	// adc_inmediate
	A = 0x05;
	bit_set(&P, flagC);
	wmem_b(PC, 0x69);
	wmem_b(PC + 1, 0x06);
	cpu_instruction();
	assert(A == 0x0C);
	assert(bit_test(P, flagC) == 0);
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagV) == 0);
	assert(bit_test(P, flagN) == 0);
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	// adc_inmediate, now with carry!
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	A = 0x05;
	bit_clear(&P, flagC);
	wmem_b(PC, 0x69);
	wmem_b(PC + 1, 0xFF);
	cpu_instruction();
	assert(A == 0x04);
	assert(bit_test(P, flagC));
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagV) == 0);
	assert(bit_test(P, flagN) == 0);
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	// adc_inmediate, now with overflow!
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	A = 0xB0;
	bit_clear(&P, flagC);
	wmem_b(PC, 0x69);
	wmem_b(PC + 1, 0xB0);
	cpu_instruction();
	assert(A == 0x60);
	assert(bit_test(P, flagC));
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagV));
	assert(bit_test(P, flagN) == 0);
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	printf("Test ADC passed!\n");
}

void test_BRANCH() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	// bpl
	wmem_b(PC, 0x10);
	wmem_b(PC + 1, 0x05);
	bit_set(&P, flagN);
	cpu_instruction();
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	//bmi
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;

	wmem_b(PC, 0x30);
	wmem_b(PC + 1, 0x06);
	cpu_instruction();
	assert(cachedPC + 0x08 == PC);
	assert(cachedCyclesThisSec + 3 == cpu_cyclesThisSec);

	printf("Test BRANCH passed!\n");
}

void test_NOP() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	wmem_b(PC, 0xEA);
	cpu_instruction();
	assert(cachedPC + 1 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);
}

void test_SBC() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	// sbc_inmediate
	A = 0x05;
	bit_clear(&P, flagC);
	wmem_b(PC, 0xE9);
	wmem_b(PC + 1, 0x04);
	cpu_instruction();
	assert(A == 0x00);
	assert(bit_test(P, flagC) == 0);
	assert(bit_test(P, flagZ));
	assert(bit_test(P, flagV));
	assert(bit_test(P, flagN) == 0);
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	// sbc_inmediate, now with carry!
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	A = 0x05;
	bit_clear(&P, flagC);
	wmem_b(PC, 0xE9);
	wmem_b(PC + 1, 0x06);
	cpu_instruction();
	assert(A == 0xFe);
	assert(bit_test(P, flagC));
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagV) == 0);
	assert(bit_test(P, flagN));
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	printf("Test SBC passed!\n");
}

void test_INCDECMEM() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	//Test inc zpage
	wmem_b(PC, 0xE6);
	wmem_b(PC + 1, 0x69);
	wmem_b(0x69, 0x05);
	cpu_instruction();
	assert(rmem_b(0x69) == 0x06);
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagN) == 0);
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 5 == cpu_cyclesThisSec);

	//test inc absolute
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	wmem_b(PC, 0xEE);
	wmem_w(PC + 1, 0x6969);
	wmem_b(0x6969, 0x15);
	cpu_instruction();
	assert(rmem_b(0x6969) == 0x16);
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagN) == 0);
	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 6 == cpu_cyclesThisSec);

	//Test dec zpage
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	wmem_b(PC, 0xC6);
	wmem_w(PC + 1, 0x69);
	wmem_b(0x69, 0x01);
	cpu_instruction();
	assert(rmem_b(0x69) == 0x00);
	assert(bit_test(P, flagZ));
	assert(bit_test(P, flagN) == 0);
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 5 == cpu_cyclesThisSec);

	//Test dec absolute
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	wmem_b(PC, 0xCE);
	wmem_w(PC + 1, 0x6969);
	wmem_b(0x6969, 0x00);
	cpu_instruction();
	assert(rmem_b(0x6969) == 0xFF);
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagN));
	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 6 == cpu_cyclesThisSec);

	printf("Test INCDECMEM passed!\n");
}

void test_LOADREGISTER() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	// lda_inmediate
	wmem_b(PC, 0xA9);
	wmem_b(PC + 1, 0x42);
	cpu_instruction();
	assert(A == 0x42);
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	// lda_absolute_y
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;
	wmem_b(PC, 0xB9);
	wmem_w(PC + 1, 0x6969);
	Y = 0x01;
	wmem_b(0x696A, 0xF5);
	cpu_instruction();
	assert(A == 0xF5);
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagN));
	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 4 == cpu_cyclesThisSec);

	printf("Test LOADREGISTER passed!\n");
}

void test_STOREREGISTER() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	// sta_indirect_y
	wmem_b(PC, 0x91);
	wmem_b(PC + 1, 0x69);
	Y = 0x42;
	word addr = indirecty_addr(0x69);
	A = 0x23;
	cpu_instruction();
	assert(rmem_b(addr) == A);
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 6 == cpu_cyclesThisSec);

	printf("Test STOREREGISTER passed!\n");
}

void test_RTS() {
	int cachedPC = PC;
	int cachedCyclesThisSec;

	// Let's run a JSR
	wmem_b(PC, 0x20);
	wmem_w(PC + 1, 0x6969);
	cpu_instruction();
	assert(PC == 0x6969);

	// And now, the RTI
	cachedCyclesThisSec = cpu_cyclesThisSec;
	wmem_b(PC, 0x60);
	cpu_instruction();
	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 6 == cpu_cyclesThisSec);

	printf("Test RTS passed!\n");
}

void test_RTI() {
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	push_w(0x6969);
	push_b(0x23);
	wmem_b(PC, 0x40);
	cpu_instruction();

	assert(PC == 0x6969);
	assert(P == 0x23);
	assert(cachedCyclesThisSec + 6 == cpu_cyclesThisSec);

	printf("Test RTI passed!\n");
}

void test_COMPAREREGISTER() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	// cmp_inmediate zero flag set
	wmem_b(PC, 0xC9);
	wmem_b(PC + 1, 0x23);
	A = 0x23;
	cpu_instruction();

	assert(bit_test(P, flagC));
	assert(bit_test(P, flagN) == 0);
	assert(bit_test(P, flagZ));
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	// cmp_absolute carry flag set & negative set
	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;

	wmem_b(PC, 0xCD);
	wmem_w(PC + 1, 0x2323);
	wmem_b(0x2323, 0x20);
	A = 0x23;
	cpu_instruction();

	assert(bit_test(P, flagC));
	assert(bit_test(P, flagN));
	assert(bit_test(P, flagZ) == 0);
	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 4 == cpu_cyclesThisSec);

	printf("Test COMPAREREGISTER passed!\n");
}

void test_LSR() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	//lsr_zpage_x
	wmem_b(PC, 0x56);
	wmem_b(PC + 1, 0x42);
	X = 0x05;
	word addr = zpagex_addr(0x42);
	wmem_b(addr, 0x11);
	cpu_instruction();

	assert(bit_test(P, flagC));
	assert(bit_test(P, flagZ) == 0);
	assert(rmem_b(addr) == 0x08); // Because 0x08 is the result of the lsr opcode
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 6 == cpu_cyclesThisSec);

	printf("Test LSR passed!\n");
}

void test_ROTATE() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	// rol_accumulator
	wmem_b(PC, 0x2A);
	A = 0x42;
	bit_set(&P, flagC);
	cpu_instruction();

	assert(bit_test(P, flagC) == 0);
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagN));
	assert(A == 0x85);
	assert(cachedPC + 1 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;

	// ror_absolute
	wmem_b(PC, 0x6E);
	wmem_w(PC + 1, 0x6942);
	word addr = absolute_addr(0x6942);
	wmem_b(addr, 0x23);
	cpu_instruction();

	assert(bit_test(P, flagC));
	assert(bit_test(P, flagZ) == 0);
	assert(bit_test(P, flagN) == 0);
	assert(rmem_b(addr) == 0x11);
	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 6 == cpu_cyclesThisSec);

	printf("Test ROTATE passed!\n");
}

void test_EOR() {
	int cachedPC = PC;
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	// eor_immediate
	wmem_b(PC, 0x49);
	wmem_b(PC + 1, 0x32);
	A = 0xFF;
	cpu_instruction();

	assert(A == 0xCD);
	assert(bit_test(P, flagN) == 1);
	assert(cachedPC + 2 == PC);
	assert(cachedCyclesThisSec + 2 == cpu_cyclesThisSec);

	cachedPC = PC;
	cachedCyclesThisSec = cpu_cyclesThisSec;

	// eor_absolute
	wmem_b(PC, 0x4D);
	wmem_w(PC + 1, 0x6942);
	word addr = absolute_addr(0x6942);
	wmem_b(addr, 0xFF);
	A = 0xFF;
	cpu_instruction();

	assert(bit_test(P, flagZ) == 1);
	assert(A == 0x00);
	assert(cachedPC + 3 == PC);
	assert(cachedCyclesThisSec + 4 == cpu_cyclesThisSec);

	printf("Test EOR passed!\n");
}

void test_JMP() {
	int cachedCyclesThisSec = cpu_cyclesThisSec;

	wmem_b(PC, 0x6C);
	wmem_w(PC + 1, 0x6942);
	word addr = absolute_addr(0x6942);
	wmem_b(addr, 0x23);
	addr = absolute_addr(0x6943);
	wmem_b(addr, 0x32);
	cpu_instruction();

	assert(PC == 0x3223);
	assert(cachedCyclesThisSec + 5 == cpu_cyclesThisSec);

	printf("Test JMP passed!\n");
}

/**
 * At this moment, this won't assert anything, just run it and spit out the logs. We'll have a fresh new instance of
 * the console, we'll enable the logs and run it from scratch
 */
void test_NESTEST() {
	set_console_log_level(ConsoleError);
	set_file_log_level(FileDebug);
	set_clear_log_file();
	set_log_path("../../logs/nesulator.log");

	cpu_power_up(0);
	//If we need to initialize anything, it should go here

	//Read the ROM, that we're going to execute and all that stuff
	ROM *rom = insertCartridge("../../rom/nestest.nes");
	load_ROM(rom);

	int isRunning = 1;

	//For this ROM to work in non-GUI mode we need to setup the PC to $C000 manually
	PC = 0xC000;

	//Main loop. Keeps the emulator running forever more. In the future we'll be able to
	//control this with a debugger, or an UI. But for now, it simply runs forever
	while (isRunning) {
		cpu_instruction();

		//Stop the emulation once the PC reaches $FFFF
		if (PC >= 0xFFFF) {
			isRunning = 0;
		}
	}

	printf("NESTEST test passed? Or not :P Have a look at the logs!");
	log_info("Error code: $%02X", rmem_b(0x0002));

	ejectCartridge();

}
