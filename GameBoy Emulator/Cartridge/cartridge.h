#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <memory>

class MBC0;
class MBC1;
class MBC3;

class Cartridge {
public:
	Cartridge() = default;
	~Cartridge() = default;

	void loadRom(std::string path);
	const std::vector<uint8_t>& getRomData();
	std::unique_ptr<Cartridge> MBCFactory();
	virtual uint8_t readAddress(const uint16_t address) const;
	virtual void writeAddress(uint16_t address, uint8_t byte);

private:
	std::vector<uint8_t> readRom(std::string path);

	std::vector<uint8_t> romData;
	int romSize = 0;

};