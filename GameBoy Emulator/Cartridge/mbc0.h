#pragma once
#include "cartridge.h"

class MBC0 : public Cartridge {
public:
	MBC0(std::vector<uint8_t>& newRomData);
	~MBC0() = default;

	uint8_t readAddress(const uint16_t address) const override;
	void writeAddress(uint16_t address, uint8_t byte) override;
private:
	std::vector<uint8_t>& romData;
};