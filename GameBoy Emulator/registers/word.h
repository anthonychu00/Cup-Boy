#pragma once
#include <cstdint>

//"interface" for the two types of 16-bit registers, FullRegisters and SplitRegisters
class Word {
	Word() = default;
	~Word() = default;

	virtual void set(uint16_t word) = 0;//pure virtual, subclasses need to implement
	virtual uint16_t getValue() const = 0;
	virtual uint8_t getHigh() const = 0;
	virtual uint8_t getLow() const = 0;
};