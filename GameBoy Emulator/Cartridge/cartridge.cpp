#include "cartridge.h"
#include <fstream>

Cartridge::Cartridge(string path) {
	romData = readRom(path);
}

//read binary file and convert to uint8_t
vector<uint8_t> Cartridge::readRom(string path) {
	
	//open file
	ifstream file(path.c_str(), ios::binary);

	//get size 
	file.seekg(0, ios::end);
	romSize = static_cast<long int>(file.tellg());
	file.seekg(0, ios::beg);

	//read
	vector<char> charData(romSize);//temporary char array to convert later
	file.read(&charData[0], romSize);
	file.close();

	//ifstream.read() only reads char, so need to recast to vector<uint8_t> with iterator range
	return vector<uint8_t>(romData.begin(), romData.end());
}