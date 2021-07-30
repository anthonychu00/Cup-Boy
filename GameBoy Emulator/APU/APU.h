#pragma once
#include <cstdint>
#include "../memory/memory_map.h"
class APU {
public:
	APU(MemoryMap& mm);
	~APU() = default;

private:
	MemoryMap& mm;
};