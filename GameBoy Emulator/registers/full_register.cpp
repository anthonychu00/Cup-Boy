#include "full_register.h"

void FullRegister::set(uint16_t w) {
	word = w;
}

void FullRegister::increment() {
	word++;
}

void FullRegister::decrement() {
	word--;
}

uint16_t FullRegister::getValue() const {
	return word;
}

uint8_t FullRegister::getHigh() const {
	return static_cast<uint8_t>((word) >> 8);
}

uint8_t FullRegister::getLow() const {
	return static_cast<uint8_t>(word);
}

