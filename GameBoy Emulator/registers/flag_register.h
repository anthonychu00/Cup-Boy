#pragma once
#include "single_register.h"
#include <cstdint>

//special class for the flag register in register AF
class FlagRegister : public SingleRegister {
public:
	FlagRegister() = default;

	//flag register only cares about its first 4 bits
	void set(uint8_t byte) override;
	void setZeroFlag(bool bit);//bit 7
	void setAddSubFlag(bool bit);//bit 6
	void setHalfCarryFlag(bool bit);//bit 5
	void setCarryFlag(bool bit);//bit 4

	bool getZeroFlag() const;
	bool getAddSubFlag() const;
	bool getHalfCarryFlag() const;
	bool getCarryFlag() const;
};