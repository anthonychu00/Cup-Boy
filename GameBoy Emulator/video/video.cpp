#include "video.h"
#include <cmath>
Video::Video(CPU& newcpu, MemoryMap& newmm):
	cpu(newcpu), 
	mm(newmm),
	fetcher(*this)
{
	createSDLWindow();
}

void Video::createSDLWindow() {
	SDL_Init(SDL_INIT_VIDEO);
	gbWindow = SDL_CreateWindow(
		"CupBoy",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		160,
		144,
		SDL_WINDOW_SHOWN
	);
	gbSurface = SDL_GetWindowSurface(gbWindow);
	SDL_FillRect(gbSurface, NULL, SDL_MapRGB(gbSurface->format, 0xFF, 0xFF, 0xFF));

	//Update the surface
	SDL_UpdateWindowSurface(gbWindow);

	SDL_Delay(2000);

	//Destroy window
	SDL_DestroyWindow(gbWindow);

	//Quit SDL subsystems
	SDL_Quit();
}
bool Video::isLCDEnabled() {
	return getBit(mm.readAddress(LCDControl), 7);
}

bool Video::currentWindowTilemap() {
	return getBit(mm.readAddress(LCDControl), 6);
}

bool Video::isWindowEnabled() {
	return getBit(mm.readAddress(LCDControl), 5);
}

bool Video::currentAddressMode() {
	return getBit(mm.readAddress(LCDControl), 4);
}

bool Video::currentBGTilemap() {
	return getBit(mm.readAddress(LCDControl), 3);
}

bool Video::currentOBJSize() {
	return getBit(mm.readAddress(LCDControl), 2);
}

bool Video::isOBJEnabled() {
	return getBit(mm.readAddress(LCDControl), 1);
}

bool Video::currentWindowPriority() {
	return getBit(mm.readAddress(LCDControl), 0);
}

uint8_t Video::currentPPUMode() {
	return mm.readAddress(LCDStatus) & 0x03;
}

void Video::renderScreen() {
	for (int i = 0; i < screenHeight; i++) {
		uint8_t currentLY = mm.readAddress(LY);
		renderScanline(currentLY);
		mm.writeAddress(LY, currentLY + 1);
	}
	//enter v blank afterwards
} 

void Video::renderScanline(uint8_t currentLY) {
	uint8_t SCX = mm.readAddress(scrollX);
	uint8_t SCY = mm.readAddress(scrollY);
	uint8_t windowX = mm.readAddress(WX);
	uint8_t windowY = mm.readAddress(WY);
	bool windowInRow = false;
	bool windowTriggered = false;//short circuit boolean to ensure window conditions aren't checked every iteration
	
	if (windowY <= currentLY) {
		windowInRow = true;
	}
	fetcher.setX(SCX / 8);
	fetcher.setY(currentLY + SCY & 255);

	int cycleNumber = 0;
	int pixelsToDiscard = SCX % 8;
	//infinite loop until current  scanline is done
	while (nextLCDPosition < 160) {
		if (backgroundPixelFIFO.size() > 8) {
			pushPixelToLCD(pixelsToDiscard);
		}

		if (!windowTriggered && windowInRow && nextLCDPosition + 7 == windowX) {//check if current x coordinate is within window
			windowTriggered = true;
			fetcher.setX(0);
			fetcher.setY(currentLY - windowY);
			fetcher.setWindow(1);
			queue<int> empty;
			swap(backgroundPixelFIFO, empty);
		}

		if (cycleNumber % 2 == 1) {//fetcher steps take 2 cycles
			fetcher.tick(currentLY);
		}
		
		cycleNumber++;
	}
	fetcher.setWindow(0);//disable window for fetcher
	

}

void Video::pushPixelToLCD(int discardedPixels) {
	if (nextLCDPosition == 0) {//discard pixels based on scx at scanline beginning
		for (int i = 0; i < discardedPixels; i++) {
			backgroundPixelFIFO.pop();
		}
	}
	currentScanline.at(nextLCDPosition) = backgroundPixelFIFO.front();
	backgroundPixelFIFO.pop();
	nextLCDPosition++;
	
}

void Video::Fetcher::tick(uint8_t currentLY) {
	uint8_t baseTilemapAddress = 0, tilePos, vramIndex, vramBaseAddress;
	bool pixelsPushed = false;
	//3 reads, idle
	switch (currentState) {
	case 1: //get tile index
		if (!windowEnabled) {		
			baseTilemapAddress = ppu.currentBGTilemap() ? 0x9800 : 0x9C00;		
		}
		else {//access from window tilemap
			baseTilemapAddress = ppu.currentWindowTilemap() ? 0x9800 : 0x9C00;
		}
		tilePos = (fetcherY / 8) * 32 + fetcherX;
		vramIndex = ppu.mm.readAddress(baseTilemapAddress + tilePos);

		vramBaseAddress = ppu.currentAddressMode() ? 0x8000 : 0x9000;
		nextTileAddress = vramBaseAddress + vramIndex * 16;
		nextTileRow = fetcherY % 8;

		fetcherX = (fetcherX + 1) & 0x1F;

		break;
	case 2: //get first byte of tile data
		tileLow = ppu.mm.readAddress(nextTileAddress + 2 * nextTileRow);
		break;
	case 3: //get second byte of tile data, and push if possible
		tileHigh = ppu.mm.readAddress(nextTileAddress + 2 * nextTileRow + 1);

		for (int i = 0; i < 8; i++) {
			bool lowerBit = getBit(tileLow, i);
			bool upperBit = getBit(tileHigh, i);

			nextPixels.at(7 - i) = 2 * upperBit + lowerBit;
		}

		pixelsPushed = attemptFIFOPush();
		if (pixelsPushed) {
			currentState = 0;
		}
		break;
	case 4: //attempt to push again, if previous attempt was unsuccessful
		pixelsPushed = attemptFIFOPush();
		if (pixelsPushed) {
			currentState = 0;
		}
		else {
			currentState--;
		}
		break;
	} 

	currentState++;
}

void Video::Fetcher::setX(int n) {
	fetcherX = n;
}

void Video::Fetcher::setY(int n) {
	fetcherY = n;
}

void Video::Fetcher::setWindow(bool status) {
	windowEnabled = status;
}

bool Video::Fetcher::attemptFIFOPush() {
	
	if (ppu.backgroundPixelFIFO.size() <= 8) {
		for (int pixel : nextPixels) {
			ppu.backgroundPixelFIFO.push(pixel);
		}
		return true;
	}
	return false;
}

queue<pair<int, int>> Video::findScanlineSprites(uint8_t currentLY) {
	queue<pair<int, int>> locations;
	int spriteSize = currentOBJSize() ? 8 : 16;

	//add support for 8 x 16 later
	for (int i = 0; i < 40; i++) {
		int spriteYPos = static_cast<int>(mm.readAddress(0xFE00 + 4 * i)) - 16;
		int spriteRow = currentLY - spriteYPos;

		if (spriteRow >= 0 && spriteRow < spriteSize) {
			uint8_t spriteXPos = mm.readAddress(0xFE00 + 4 * i + 1);
			pair<int, int> spriteCoords(spriteXPos, spriteRow);
			locations.push(spriteCoords);
		}

		if (locations.size() >= 10) {
			break;
		}
	}
	return locations;//copy ellision
}

void Video::getSpritePixels(uint8_t yIndex, uint8_t xIndex) {

}

array<int, 8> Video::getPixels(uint8_t vramIndex, uint8_t yIndex) {
	array<int, 8> pixelValues;
	uint16_t vramBaseAddress = currentAddressMode() ? 0x8000 : 0x9000;
	uint16_t tileStartingAddress = vramBaseAddress + vramIndex * 16;

	uint8_t firstByte = mm.readAddress(tileStartingAddress + 2 * yIndex);
	//push pixels
	uint8_t secondByte = mm.readAddress(tileStartingAddress + 2 * yIndex + 1);
	//push pixels

	for (int i = 0; i < 8; i++) {
		bool lowerBit = getBit(firstByte, i);
		bool upperBit = getBit(secondByte, i);

		pixelValues.at(7 - i) = 2 * upperBit + lowerBit;
	}
	
	return pixelValues;
}

/*
void Video::renderBackgroundLine() {
	uint16_t bgTileMapAddress = currentBGTilemap() ? 0x9800 : 0x9C00;
	uint8_t yOrigin = mm.readAddress(scrollY);
	for (int i = 0; i < screenWidth; i++) {//160
	}
}*/

/*
void Video::renderTile(int tileIndex) {
	array<int, 8> pixelValues;
	uint16_t vramBaseAddress = currentAddressMode() ? 0x8000 : 0x9000;
	
	
	for (int i = 0; i < 16; i += 2) {
		uint8_t firstByte = mm.readAddress(vramBaseAddress + (16 * tileIndex) + i);
		uint8_t secondByte = mm.readAddress(vramBaseAddress + (16 * tileIndex) + i + 1);
		readTileRow(firstByte, secondByte, pixelValues);
	}
	
}
*/

/*
void Video::readTileRow(uint8_t firstByte, uint8_t secondByte, array<int, 8>& pixelValues) {
	//bit 0 is rightmost pixel, bit 7 leftmost
	for (int i = 0; i < 8; i++) {
		bool lowerBit = getBit(firstByte, i);
		bool upperBit = getBit(secondByte, i);
		
		//pixel values arranged from left to right
		pixelValues.at(7 - i) = 2 * static_cast<int>(upperBit) + 1 * static_cast<int>(lowerBit);
	}

}*/

/*void Video::renderScanline(uint8_t currentLY) {
	queue<pair<int, int>> spriteLocations = findScanlineSprites(currentLY);
	pair<int, int> nextSpritePos = spriteLocations.front();
	if (nextSpritePos.first == 0) { //sprite is hidden, but still affects scanline
		spriteLocations.pop();
		nextSpritePos = spriteLocations.front();
	}
	bool windowTriggered = false;

	uint8_t fetcherY = currentLY + mm.readAddress(scrollY) & 255;
	uint8_t SCX = mm.readAddress(scrollX);

	uint16_t baseTilemapAddress = currentBGTilemap() ? 0x9800 : 0x9C00;
	uint8_t windowY = mm.readAddress(WY);
	uint8_t windowX = mm.readAddress(WX);

	for (int i = 0; i < screenWidth; i++) {
		/*if (nextSpritePos.first == i) {
			getSpritePixels(nextSpritePos.second, nextSpritePos.first);//always 8
		}*/

		/*uint8_t fetcherX = SCX / 8 + i & 0x1F;
		uint8_t tilePos = (fetcherY / 8) * 32 + fetcherX;

		/*if (!windowTriggered && windowY <= currentLY && windowX <= i + 7) {
			baseTilemapAddress = currentWindowTilemap() ? 0x9800 : 0x9C00;
			windowTriggered = true;
		}*/

		/*if (backgroundPixelFIFO.size() >= 8) {
			pushPixelToLCD();
			pushPixelToLCD();
		}*/
		//background/window tile to fetch pixels from
		/*uint8_t vramIndex = mm.readAddress(baseTilemapAddress + tilePos);

		int yIndex = fetcherY % 8;
		array<int, 8> pixels = getPixels(vramIndex, yIndex);

		if (backgroundPixelFIFO.size() <= 8) {
			for (int pixel : pixels) {
				backgroundPixelFIFO.push(pixel);
			}
		}

		//discard first few pixels depending on scroll X
		if (i == 0) {
			for (int j = 0; j < SCX % 8; j++) {
				backgroundPixelFIFO.pop();
			}
		}

	}
}*/
