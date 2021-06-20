#include "mbc3.h"

MBC3::MBC3(std::vector<uint8_t>& newRomData) : romData(newRomData)
{
	ramData = std::vector<uint8_t>(0x8000);
}

uint8_t MBC3::readAddress(const uint16_t address) const {
	if (address >= 0x0000 && address <= 0x3FFF) {
		return romData.at(address);
	}
	else if (address >= 0x4000 && address <= 0x7FFF) {
		uint64_t bankStartingAddress = selectedROMBank * 0x4000;
		uint16_t bankLocation = address - 0x4000;
		return romData.at(bankStartingAddress + bankLocation);
	}
	else if (address >= 0xA000 && address <= 0xBFFF) {
		if (!RAMRTCEnabled) {
			return 0;
		}
		uint32_t bankStartingAddress = selectedRAMBank * 0x2000;
		uint16_t bankLocation = address - 0xA000;
		return ramData.at(bankStartingAddress + bankLocation);
	}
}

void MBC3::writeAddress(uint16_t address, uint8_t byte) {
	if (address >= 0x0000 && address <= 0x1FFF) {
		uint8_t lowerNibble = byte & 0x0F;
		if (lowerNibble == 0x0A) {
			RAMRTCEnabled = true;
		}
		else {
			RAMRTCEnabled = false;
		}
	}
	else if (address >= 0x2000 && address <= 0x3FFF) {
		switch (byte) {
		case 0x00: selectedROMBank = 0x1; return;
		}

		selectedROMBank = byte & 0x7F;
	}
	else if (address >= 0x4000 && address <= 0x5FFF) {
		//RAM bank selector, or RTC
	}
	else if (address >= 0x6000 && address <= 0x7FFF) {
		//time written here gets "latched" to RTC
	}
	else if (address >= 0xA000 && address <= 0xBFFF) {
		if (!RAMRTCEnabled) {
			return;
		}
		uint32_t bankStartingAddress = selectedRAMBank * 0x2000;
		uint16_t bankLocation = address - 0xA000;
		ramData.at(bankStartingAddress + bankLocation) = byte;
	}
}