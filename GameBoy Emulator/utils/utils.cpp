#include "utils.h"

/*void setBit(uint8_t& byte, const int position, const bool bitVal) {
	byte = (byte & ~(1UL << position)) | (bitVal << position);
}*/

void setLowerNibble(uint8_t& byte, uint8_t nibble) {
	byte &= 0xF0;
	byte |= (nibble & 0x0F);
}