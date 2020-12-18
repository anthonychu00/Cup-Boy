#include "flag_register.h"

void FlagRegister::set(const uint8_t b){
	byte = b & 0xF0;//11110000
}

void FlagRegister::setZeroFlag(bool bit) {

}