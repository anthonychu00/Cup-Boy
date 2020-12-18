#include "single_register.h"

SingleRegister::SingleRegister(uint8_t b){
	byte = b;
}

void SingleRegister::set(const uint8_t b){
	byte = b;
}

void SingleRegister::increment(){
	byte++;
}

void SingleRegister::decrement(){
	byte--;
}

uint8_t SingleRegister::getByte() const{
	return byte;
}