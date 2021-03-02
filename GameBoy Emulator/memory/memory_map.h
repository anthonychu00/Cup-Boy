#pragma once
#include <cstdint>
#include <vector>

class CPU;

class MemoryMap {
public:
	MemoryMap(CPU& newCpu);
	uint8_t readAddress(const uint16_t address);//checks what type of memory to access
	void writeAddress(const uint16_t address, const uint8_t byte);
	
	

private:
	std::vector<uint8_t> memory;
	std::vector<uint8_t> bootroom;//size 0x100
	CPU& cpu;
};