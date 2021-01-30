#include "split_register.h"

SplitRegister::SplitRegister(uint8_t h, uint8_t l)
{	
	high.set(h);
	low.set(l);
}

void SplitRegister::set(const uint16_t high_low){
	high.set(static_cast<uint8_t>((high_low) >> 8));//shift right 8 bits and lop off empty first byte
	low.set(static_cast<uint8_t>(high_low));//cast lops off first byte
}

void SplitRegister::increment(){
	set(getValue() + 1);
}

void SplitRegister::decrement(){
	set(getValue() - 1);
}

uint16_t SplitRegister::getValue() const{
	return ((uint16_t)low.getByte() << 8) | high.getByte();
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

