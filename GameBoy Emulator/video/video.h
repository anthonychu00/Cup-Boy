#pragma once
#include <cstdint>
#include <array>
#include <queue>
#include <utility>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include "../CPU/CPU.h"
#include "../memory/memory_map.h"
#include "../utils/utils.h"


//class controlling the video of the GameBoy

using namespace std;
class Video {
public:
	Video(CPU& cpu, MemoryMap& mm);
	~Video() = default;
	void tick(int cpuCycles);
	void viewTileData();
private:
	class Fetcher {
	public:
		Fetcher(Video& v):ppu(v){}
		~Fetcher() = default;
		void tick(uint8_t currentLY);
		void setX(int n);
		void setY(int n);
		void setWindow(bool status);
		bool isFetchingWindow();
	private:
		Video& ppu;// reference to access variables of enclosing class
		int currentState = 1;
		int fetcherX = 0;
		int fetcherY = 0;

		uint16_t nextTileAddress = 0;
		uint8_t nextTileRow = 0;
		uint8_t tileLow = 0;
		uint8_t tileHigh = 0;

		array<int, 8> nextPixels = {};

		bool fetchingWindow = false;

		bool attemptFIFOPush();
	};

	enum class Mode {
		HBLANK, VBLANK, OAM_SCAN, DRAW_LCD
	};
	CPU& cpu;
	MemoryMap& mm; 
	Fetcher fetcher;//default constructor called
	SDL_Window* gbWindow;
	SDL_Renderer* renderer;
	SDL_Texture* gbTexture;

	//testing tilemap
	bool tileViewerPresent = false;
	SDL_Window* tileWindow;
	SDL_Renderer* tileRenderer;
	SDL_Texture* tileTexture;
	array<int, 128 * 128> tileBuffer = {}; //16 tiles in each row/column
											//8 by 8 pixels in each tile


	const int screenWidth = 160;
	const int screenHeight = 144;

	Mode mode = Mode::OAM_SCAN;
	int cycles = 0;
	int drawCycles = 0;
	int HBlankCycles = 0;
	
	int nextLCDPosition = 0;
	int pixelShift = 0;
	bool windowInLine = false;
	bool currentFrameBlank = false;

	array<int, 160 * 144> frameBuffer = {};
	queue<int> backgroundPixelFIFO;
	queue<int> spritePixelFIFO;

	const int totalScanlineCycles = 456;
	const int OAMCycles = 80;//mode 2
	//const int LCDCycles = 172;//mode 3
	//const int HBlankCycles = 204;//mode 0
	

	const uint16_t LCDControl = 0xFF40;//PPU never locks it
	const uint16_t LCDStatus = 0xFF41;

	//window and background information
	const uint16_t scrollY = 0xFF42;//with FF43, specifies top left pixel area
	const uint16_t scrollX = 0xFF43;
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

	//testing tilemap
	void drawTile(int x, int y, uint32_t* newPixels, uint16_t address);

	//LCDControl bits (FF40)
	bool isLCDEnabled();
	bool currentWindowTilemap();//0 = $9800 tilemap, 1 = $9C00 tilemap
	bool isWindowEnabled();
	bool currentAddressMode();//0 = $8000 addressing, 1 = $8800 addressing
	bool currentBGTilemap();//0 = $9800 tilemap, 1 = $9C00 tilemap
	bool currentOBJSize();//0 = 8 x 8
	bool isSpritesEnabled();//are sprites enabled, set to 0 for sprites on textboxes
	bool currentWindowPriority();

	//LCDStatus bits (FF41)
	uint8_t currentPPUMode();

	void initializeSDL();
	void advanceMode3(uint8_t currentLY, int drawCycles);
	void pushPixelToLCD(uint8_t currentLY);
	void renderFrameBuffer();
	void writeFrameBufferData(uint32_t* newPixels);
	uint32_t decipherPixelColor(int pixel);
	void clearFIFO(queue<int>& FIFO);

	void renderScanline(uint8_t currentLY);

	queue<pair<int, int>> findScanlineSprites(uint8_t currentLY);
	void getSpritePixels(uint8_t yIndex, uint8_t xIndex);

	array<int, 8> getPixels(uint8_t vramIndex, uint8_t yIndex);
	
};