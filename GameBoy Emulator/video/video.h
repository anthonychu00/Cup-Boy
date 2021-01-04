#pragma once
#include <cstdint>
#include "../CPU/CPU.h"
#include "../memory/memory_map.h"
#include "../registers/single_register.h"
//class controlling the video of the GameBoy
class Video {
public:
	Video(CPU& cpu, MemoryMap& mm);
	~Video();
private:
	SingleRegister LCDControl;//FF40
	SingleRegister LCDStatus;//FF41
	SingleRegister ScrollY;//FF42
	SingleRegister ScrollX;//FF43
	SingleRegister LCDYCoord;
	SingleRegister LYCompare;
};