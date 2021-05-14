#pragma once
#include <cstdint>
#include <vector>
#include "../Cartridge/cartridge.h"
class CPU;
//class Joypad;

class MemoryMap {
public:
	//MemoryMap(CPU& newCpu, Cartridge& newCartridge, Joypad& newJoypad);
	MemoryMap(CPU& newCpu, Cartridge& newCartridge);
	uint8_t readAddress(const uint16_t address);//checks what type of memory to access
	void writeAddress(const uint16_t address, const uint8_t byte);
	
private:
	std::vector<uint8_t> memory;
	std::vector<uint8_t> bootroom;//size 0x100
	CPU& cpu;
	Cartridge& cartridge;
	//Joypad& joypad;
};