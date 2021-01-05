#pragma once
#include <cstdint>
#include "single_register.h"
#include "word.h"

//16-bit register that can be split into two 8-bit registers
class SplitRegister : public Word{
public:
	SplitRegister(uint8_t h, uint8_t l);
	void set(uint16_t high_low) override;

	uint16_t getValue() const override; //const makes a function unable to change member variables
	uint8_t getHigh() const override; 
	uint8_t getLow() const override;
	SingleRegister& getHighRegister();
	SingleRegister& getLowRegister();

	void increment(); //+1 to the byte value
	void decrement();

	/*
	void incrementHigh();
	void decrementHigh();
	void incrementLow();
	void decrementLow();*/


private:
	SingleRegister high;
	SingleRegister low;
};