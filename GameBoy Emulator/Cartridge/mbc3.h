#pragma once
#include "cartridge.h"

class MBC3 : public Cartridge {
public:
	MBC3(std::vector<uint8_t>& newRomData);
	~MBC3() = default;

	uint8_t readAddress(const uint16_t address) const override;
	void writeAddress(uint16_t address, uint8_t byte) override;
private:
	std::vector<uint8_t>& romData;
	std::vector<uint8_t> ramData;
	int selectedROMBank = 0x1;
	int selectedRAMBank = 0x0;
	bool RAMRTCEnabled = false;
	
};