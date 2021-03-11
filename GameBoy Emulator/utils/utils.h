#pragma once
#include <cstdint>

bool getBit(const uint8_t byte, const int position);
void setBit(uint8_t& byte, const int position, const bool bitVal);