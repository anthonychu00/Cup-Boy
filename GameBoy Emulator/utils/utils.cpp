#include "utils.h"

void setBit(uint8_t& byte, const int position, const bool bitVal) {
	byte = (byte & ~(1UL << position)) | (bitVal << position);
}