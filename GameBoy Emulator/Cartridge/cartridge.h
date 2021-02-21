#pragma once
#include <string>
#include <vector>
#include <cstdint>
using namespace std;

class Cartridge {
public:
	Cartridge(string path);
	~Cartridge() = default;
private:
	void readRom(string path);
	vector<uint8_t> romData;
	long int romSize;


	//how to represent rom data
};