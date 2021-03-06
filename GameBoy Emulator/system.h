#pragma once
#include <memory>
#include "CPU/CPU.h"
#include "memory/memory_map.h"
#include "video/video.h"
#include "Cartridge/cartridge.h"
#include "joypad/joypad.h"

class System {
public:
	System(std::string path);
	~System() = default;

	void start();
private:
	const int CYCLE_REFRESH = 69905;

	Cartridge cartridge;
	CPU cpu;
	MemoryMap mm;
	Video video;
	Joypad joypad;
};