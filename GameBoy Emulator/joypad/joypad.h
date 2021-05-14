#pragma once
#include <cstdint>
#include "../CPU/CPU.h"
#include "../memory/memory_map.h"

class Joypad {
public:
	Joypad(CPU& newCpu, MemoryMap& newMm);
	~Joypad() = default;
	uint8_t getInput();
private:
	uint8_t currentState = 0xFF;

	CPU& cpu;
	MemoryMap& mm;
};
