#include "Utils.h"

int bit_test(byte b, byte bitPos) {
    byte bitSet = (b >> (bitPos));
    return (bitSet & 1);
}

void bit_set(byte *b, byte bitPos) {
    *b |= 1UL << bitPos;
}

void bit_clear(byte *b, byte bitPos) {
    *b &= ~(1UL << bitPos);
}
