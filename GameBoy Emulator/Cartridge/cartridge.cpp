#include "cartridge.h"
#include <fstream>
#include <iostream>
#include <direct.h>
Cartridge::Cartridge(string path) {
	romData = readRom(path);
}

//read binary file and convert to uint8_t
vector<uint8_t> Cartridge::readRom(string path) {
	
	//open file
	ifstream file(path.c_str(), ios::binary);

	if (!file) {
		cout << "Error" << endl;

		char buff[FILENAME_MAX]; //create string buffer to hold path
		//_getcwd(buff, FILENAME_MAX);
		string current_working_dir(buff);

		cout << "Current dir: " << current_working_dir << endl;
		exit(EXIT_FAILURE);
	}

	//get size 
	file.seekg(0, ios::end);
	romSize = static_cast<int>(file.tellg());
	file.seekg(0, ios::beg);

	//cout << "Size: " << romSize << endl;

	//read
	vector<char> charData(romSize);//temporary char array to convert later
	file.read(&charData[0], romSize);
	file.close();

	//ifstream.read() only reads char, so need to recast to vector<uint8_t> with iterator range
	return vector<uint8_t>(charData.begin(), charData.end());
}

uint8_t Cartridge::readAddress(const uint16_t address) const {
	return romData.at(address);
}

void Cartridge::writeAddress(uint16_t address, uint8_t byte) {
	romData.at(address) = byte;
}

//return unmodifiable reference to rom
const vector<uint8_t>& Cartridge::getRomData() {
	return romData;
}