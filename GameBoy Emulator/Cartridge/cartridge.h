#pragma once
#include <string>
#include <vector>
#include <cstdint>
using namespace std;

class Cartridge {
public:
	Cartridge(string path);
	~Cartridge() = default;
	const vector<uint8_t>& getRomData();
	uint8_t readAddress(const uint16_t address) const;
	void writeAddress(uint16_t address, uint8_t byte);
private:
	vector<uint8_t> readRom(string path);
	vector<uint8_t> romData;
	int romSize;
	
};