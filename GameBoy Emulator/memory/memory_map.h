#pragma once
#include <cstdint>
#include <vector>


class MemoryMap {
public:
	MemoryMap();
	uint8_t readAddress(uint16_t address);//checks what type of memory
	uint8_t fetchMemory(uint16_t address);//standard call to get data from a memory address


private:
	std::vector<uint8_t> memory;
};