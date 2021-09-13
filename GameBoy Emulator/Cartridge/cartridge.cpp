#include <fstream>
#include <iostream>
#include <direct.h>
#include "cartridge.h"
#include "mbc0.h"
#include "mbc1.h"
#include "mbc3.h"
void Cartridge::loadRom(std::string path) {
	romData = readRom(path);
}

//read binary file and convert to uint8_t
std::vector<uint8_t> Cartridge::readRom(std::string path) {
	
	//open file
	std::ifstream file(path.c_str(), std::ios::binary);

	if (!file) {
		std::cout << "Error" << std::endl;

		char buff[FILENAME_MAX]; //create string buffer to hold path
		//_getcwd(buff, FILENAME_MAX);
		std::string current_working_dir(buff);

		std::cout << "Current dir: " << current_working_dir << std::endl;
		exit(EXIT_FAILURE);
	}

	//get size 
	file.seekg(0, std::ios::end);
	romSize = static_cast<int>(file.tellg());
	file.seekg(0, std::ios::beg);

	//cout << "Size: " << romSize << endl;

	//read
	std::vector<char> charData(romSize);//temporary char array to convert later
	file.read(&charData[0], romSize);
	file.close();

	//ifstream.read() only reads char, so need to recast to vector<uint8_t> with iterator range
	return std::vector<uint8_t>(charData.begin(), charData.end());
}

std::unique_ptr<Cartridge> Cartridge::MBCFactory() {
	uint8_t mbcType = readAddress(0x0147);
	//printf("mbctype: %d\n", mbcType);
	switch (mbcType) {
	case 0x0: return std::make_unique<MBC0>(romData);
		case 0x01:
		case 0x02:
		case 0x03: return std::make_unique<MBC1>(romData);
		case 0x0F:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13: return std::make_unique<MBC3>(romData);
	}
}

uint8_t Cartridge::readAddress(const uint16_t address) const {
	return romData.at(address);
}

void Cartridge::writeAddress(uint16_t address, uint8_t byte) {
	romData.at(address) = byte;
}

//return unmodifiable reference to rom
const std::vector<uint8_t>& Cartridge::getRomData() {
	return romData;
}

