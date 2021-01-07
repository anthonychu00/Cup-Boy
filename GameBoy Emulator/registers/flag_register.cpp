#include "flag_register.h"

void FlagRegister::set(const uint8_t b){
	byte = b & 0xF0;//11110000
}

void FlagRegister::setZeroFlag(bool bit) {
	byte = (byte & ~(1UL << 7)) | (bit << 7);
}

void FlagRegister::setAddSubFlag(bool bit) {
	byte = (byte & ~(1UL << 6)) | (bit << 6);
}

void FlagRegister::setHalfCarryFlag(bool bit) {
	byte = (byte & ~(1UL << 5)) | (bit << 5);
}

void FlagRegister::setCarryFlag(bool bit) {
	byte = (byte & ~(1UL << 4)) | (bit << 4);
}

bool FlagRegister::getZeroFlag() const {
	return (byte >> 7) & 0x1;
}

bool FlagRegister::getAddSubFlag() const {
	return (byte >> 6) & 0x1;
}

bool FlagRegister::getHalfCarryFlag() const {
	return (byte >> 5) & 0x1;
}

bool FlagRegister::getCarryFlag() const {
	return (byte >> 4) & 0x1;
}