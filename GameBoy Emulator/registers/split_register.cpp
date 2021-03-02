#include "split_register.h"

void SplitRegister::set(const uint16_t high_low){
	high.set(static_cast<uint8_t>((high_low & 0xFF00) >> 8));//shift right 8 bits and lop off empty first byte
	low.set(static_cast<uint8_t>(high_low & 0x00FF));//cast lops off first byte
}

void SplitRegister::increment(){
	set(getValue() + 1);
}

void SplitRegister::decrement(){
	set(getValue() - 1);
}

uint16_t SplitRegister::getValue() const{
	//high is the leftmost 8 bits, low are the 8 bits afterwards
	return ((uint16_t)high.getByte() << 8) | low.getByte();
}

uint8_t SplitRegister::getHigh() const {
	return high.getByte();
}

uint8_t SplitRegister::getLow() const {
	return low.getByte();
}

SingleRegister& SplitRegister::getHighRegister() {
	return high;
}
SingleRegister& SplitRegister::getLowRegister() {
	return low;
}

