#pragma once
#include <cstdint>

template<typename T>
void clearQueue(typename T& FIFO) {
	T empty;
	swap(FIFO, empty);
}

template<typename T>
bool getBit(const T byte, const int position) {
	return (byte >> position) & 0x1;
}

void setBit(uint8_t& byte, const int position, const bool bitVal);

void setLowerNibble(uint8_t& byte, uint8_t nibble);