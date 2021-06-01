#include "mbc1.h"

MBC1::MBC1(vector<uint8_t>& newRomData) : romData(newRomData) 
{
	ramData = vector<uint8_t>(0x8000);
}

uint8_t MBC1::readAddress(const uint16_t address) const {
	if (address >= 0x0000 && address <= 0x3FFF) {
		return romData.at(address);
	}
	else if (address >= 0x4000 && address <= 0x7FFF) {
		uint64_t bankStartingAddress = selectedROMBank * 0x4000;
		uint16_t bankLocation = address - 0x4000;
		return romData.at(bankStartingAddress + bankLocation);
	}
	else if (address >= 0xA000 && address <= 0xBFFF) {
		if (!RAMEnabled) {
			return 0;
		}
		uint32_t bankStartingAddress = selectedRAMBank * 0x2000;
		uint16_t bankLocation = address - 0xA000;
		return ramData.at(bankStartingAddress + bankLocation);
	}
}

void MBC1::writeAddress(uint16_t address, uint8_t byte) {
	if (address >= 0x0000 && address <= 0x1FFF) {
		uint8_t lowerNibble = byte & 0x0F;
		if (lowerNibble == 0x0A) {
			RAMEnabled = true;
		}
		else {
			RAMEnabled = false;
		}
	}
	else if (address >= 0x2000 && address <= 0x3FFF) {
		switch (byte) {
		case 0x00: selectedROMBank = 0x1; return;
		case 0x20: selectedROMBank = 0x21; return;
		case 0x40: selectedROMBank = 0x41; return;
		case 0x60: selectedROMBank = 0x61; return;
		}
		
		selectedROMBank = byte & 0x1F;
	}
	else if (address >= 0x4000 && address <= 0x5FFF) {
		//RAM bank selector
	}
	else if (address >= 0x6000 && address <= 0x7FFF) {
		//banking mode select
	}
	else if (address >= 0xA000 && address <= 0xBFFF) {
		if (!RAMEnabled) {
			return;
		}
		uint32_t bankStartingAddress = selectedRAMBank * 0x2000;
		uint16_t bankLocation = address - 0xA000;
		ramData.at(bankStartingAddress + bankLocation) = byte;
	}
}