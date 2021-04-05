#pragma once
#include <cstdint>
#include <array>
#include "../CPU/CPU.h"
#include "../memory/memory_map.h"
#include "../utils/utils.h"
//class controlling the video of the GameBoy

using namespace std;
class Video {
public:
	Video(CPU& cpu, MemoryMap& mm);
	~Video() = default;
private:
	CPU& cpu;
	MemoryMap& mm;

	const uint16_t LCDControl = 0xFF40;//PPU never locks it
	const uint16_t LCDStatus = 0xFF41;

	//window and background information
	const uint16_t ScrollY = 0xFF42;//with FF43, specifies top left pixel area
	const uint16_t ScrollX = 0xFF43;
	const uint16_t LY = 0xFF44;//current horizontal line of screen being drawn
	const uint16_t LYC = 0xFF45;//compare LYC to LY, if equal, can request STAT interrupt
	const uint16_t WY = 0xFF4A;//window Y position
	const uint16_t WX = 0xFF4B;//window X position

	//monochrome palettes
	const uint16_t BGP = 0xFF47;//background palette data
	const uint16_t OBP0 = 0xFF48;//object palette 0 data
	const uint16_t OBP1 = 0xFF49;//object palette 1 data

	//color palettes (GBC only)
	const uint16_t BCPS = 0xFF68;//background color palette specification
	const uint16_t BCPD = 0xFF69;//background color palette data
	const uint16_t OCPS = 0xFF6A;//object color palette specification
	const uint16_t OCPD = 0xFF6B;//object color palette data

	const uint16_t DMA = 0xFF46;

	//LCDControl bits (FF40)
	bool isLCDEnabled();
	bool currentWindowTilemap();//0 = $9800 tilemap, 1 = $9C00 tilemap
	bool isWindowEnabled();
	bool currentAddressMode();
	bool currentBGTilemap();//0 = $9800 tilemap, 1 = $9C00 tilemap
	bool currentOBJSize();
	bool isOBJEnabled();//are sprites enabled, set to 0 for sprites on textboxes
	bool currentWindowPriority();

	//LCDStatus bits (FF41)
	uint8_t currentPPUMode();

	void readTileRow(uint16_t tileRow, array<int, 8>& pixelValues);

	
};