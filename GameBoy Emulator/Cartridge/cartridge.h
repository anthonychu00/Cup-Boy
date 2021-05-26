#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
using namespace std;

class MBC0;

class Cartridge {
public:
	Cartridge() = default;
	Cartridge(string path);
	~Cartridge() = default;
	const vector<uint8_t>& getRomData();
	unique_ptr<Cartridge> MBCFactory();
	virtual uint8_t readAddress(const uint16_t address) const;
	virtual void writeAddress(uint16_t address, uint8_t byte);
	
private:
	vector<uint8_t> readRom(string path);

	vector<uint8_t> romData;
	int romSize = 0;
	
};