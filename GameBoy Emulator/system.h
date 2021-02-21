#pragma once
#include "CPU/CPU.h"
#include "memory/memory_map.h"
#include "video/video.h"

class System {
public:
	System();
	~System() = default;

	void start();
private:
	CPU& cpu;
	MemoryMap& mm;
	Video& video;
};