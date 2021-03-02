#pragma once
#include "CPU/CPU.h"
#include "memory/memory_map.h"
#include "video/video.h"
#include "Cartridge/cartridge.h"

class System {
public:
	System(string path);
	~System() = default;

	void start();
private:
	Cartridge cartridge;
	CPU cpu;
	MemoryMap mm;
	Video video;
};