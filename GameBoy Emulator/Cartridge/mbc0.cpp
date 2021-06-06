#include "mbc0.h"

MBC0::MBC0(std::vector<uint8_t>& newRomData): romData(newRomData) 
{
}

uint8_t MBC0::readAddress(const uint16_t address) const {
	return romData.at(address);
}
void MBC0::writeAddress(uint16_t address, uint8_t byte) {
	//no mbc = blocked cartridge write
}