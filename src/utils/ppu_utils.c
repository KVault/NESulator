
#include "ppu_utils.h"
#include "../memory.h"

word c_vram;
word t_vram;
bool w;
bool nmi_output;
bool nmi_occurred;
int fine_x;

///////////////////////////////////////////////////////////////////////////
/////////////////////////////REGISTERS/////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void write_PPUADDR(byte value) {
	if (!w) {
		/**
         * t: .FEDCBA ........ = d: ..FEDCBA
	 	 * t: X...... ........ = 0
         * w:                  = 1
	 	*/
		word val = (word) ((value & 0b00111111) << 8);// extract FEDCBA
		t_vram = mask_word(t_vram, 0b011111100000000, val);
		t_vram &= 0b0111111111111111;// for the X
		w = true;
	} else {
		/**
		 * t: ....... HGFEDCBA = d: HGFEDCBA
		 * v                   = t
		 * w:                  = 0
		 */
		t_vram = (word) ((t_vram & ~0b0000000011111111) | (value));
		c_vram = t_vram;
		w = false;
	}
}


/**
 * 2005 first write:
 * t: ....... ...HGFED = d: HGFED...
 * x:              CBA = d: .....CBA
 * w:                  = 1
 *
 * 2005 second write:
 *      t: CBA..HG FED..... = d: HGFEDCBA
 *      w:                  = 0
 */
void write_PPUSCROLL(byte d){
	static word val;
	if(!w){
		val = (word) (d & 0xF8); // extract HGFED...
		t_vram = mask_word(t_vram, 0x0001F, val);
		fine_x = (byte) (d & 0x07);
		w = true;
	}else{
		//We have to do it in two steps. One for CBA and the other one for HGFED
		w = false;
		val = (word) (d & 0x7) << 8;// extract CBA
		t_vram = mask_word(t_vram, 0x7000, val);

		val = d >> 3;// extract HGFED
		t_vram = mask_word(t_vram, 0x0001F, val);
	}
}

byte read_PPUSTATUS(){
	//Need to access the bank directly here. Otherwise it'll be stuck in a recursive loop
	byte status = ram_bank[PPUSTATUS];
	bit_val(&status, 7, nmi_occurred);
	nmi_occurred = false;
	wmem_b(PPUSTATUS, status);
	w = 0; //Reading from STATUS resets the (shared) latch w
	return status;
}

void write_PPUDATA(byte value) {
	wmem_b_vram(c_vram, value);
	c_vram += bit_test(rmem_b(PPUCTRL), 2) ? 32 : 1;
}

byte read_PPUDATA() {
	c_vram += bit_test(rmem_b(PPUCTRL), 2) ? 32 : 1;
	return rmem_b_vram(c_vram);
}

void write_PPUCTRL(byte value) {
	nmi_output = bit_test(value, 7);

	// t: ... AB.. .... .... = value: .... ..AB
	word val = (word) (value & 0x03) << 8;
	t_vram = mask_word(t_vram, 0b0001100000000000, val);
}
