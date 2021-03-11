#include "utils.h"

bool getBit(const uint8_t byte, const int position) {
	return (byte >> position) & 0x1;
}

void setBit(uint8_t& byte, const int position, const bool bitVal) {
	byte = (byte & ~(1UL << position)) | (bitVal << position);
}