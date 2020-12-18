#pragma once
#include "word.h"

//16-bit register that is always treated as 16-bits
class FullRegister : public Word {
public:
	FullRegister() = default;
	void set(uint16_t w) override;

	uint16_t getValue() const override;
	uint8_t getHigh() const override;
	uint8_t getLow() const override;

	void increment();
	void decrement();
private:
	uint16_t word = 0x0;
};

