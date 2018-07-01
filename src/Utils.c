#include "Utils.h"

int bit_test(byte b, byte bitPos) {
	byte bitSet = (b >> (bitPos));
	return (bitSet & 1);
}
