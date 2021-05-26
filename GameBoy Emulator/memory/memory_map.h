#pragma once
#include <cstdint>
#include <array>
#include <memory>
#include "../Cartridge/cartridge.h"
class CPU;
class Joypad;

class MemoryMap {
public:
	MemoryMap(CPU& newCpu, Joypad& newJoypad, unique_ptr<Cartridge> newCartridge);
	uint8_t readAddress(const uint16_t address);//checks what type of memory to access
	void writeAddress(const uint16_t address, const uint8_t byte);
private:
	std::array<uint8_t, 0x10000> memory = {};
	std::vector<uint8_t> bootroom;//size 0x100

	CPU& cpu;
	Joypad& joypad;
	unique_ptr<Cartridge> cartridge;
};