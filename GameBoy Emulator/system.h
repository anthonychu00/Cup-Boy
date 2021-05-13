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
	//const int CYCLE_REFRESH = 69905;
	const int CYCLE_REFRESH = 69905;
	Cartridge cartridge;
	CPU cpu;
	MemoryMap mm;
	Video video;
};