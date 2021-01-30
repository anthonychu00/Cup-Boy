#pragma once
#include <cstdint>

//class for an 8-bit register
class SingleRegister {
public:
	SingleRegister() = default;
	SingleRegister(uint8_t b);
	~SingleRegister() = default;

	virtual void set(uint8_t b);
	void setBit(int bitNum, bool bitVal);

	void increment(); //+1 to the byte value
	void decrement();

	uint8_t getByte() const;
protected:
	uint8_t byte = 0x00;
};