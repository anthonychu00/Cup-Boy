#pragma once
#include <cstdint>
#include "single_register.h"
#include "flag_register.h"
#include "word.h"

//16-bit register that can be split into two 8-bit registers
class SplitRegister : public Word{
public:
	SplitRegister(SingleRegister& newHigh, SingleRegister& newLow);
	void set(uint16_t high_low) override;

	void increment() override; //+1 to the byte value
	void decrement() override;

	uint16_t getValue() const override; //const makes a function unable to change member variables
	uint8_t getHigh() const override;
	uint8_t getLow() const override;

	void setHigh(uint8_t toSet);
	void setLow(uint8_t toSet);

	SingleRegister& getHighRegister();
	SingleRegister& getLowRegister();
private:
	SingleRegister& high;
	SingleRegister& low;
};