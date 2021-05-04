#include "video.h"
#include <cmath>
Video::Video(CPU& newcpu, MemoryMap& newmm):
	cpu(newcpu), 
	mm(newmm),
	fetcher(*this)
{
	initializeSDL();
}

void Video::initializeSDL() {
	SDL_Init(SDL_INIT_VIDEO);
	gbWindow = SDL_CreateWindow(
		"CupBoy",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		160,
		144,
		SDL_WINDOW_SHOWN
	);
	renderer = SDL_CreateRenderer(gbWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	gbTexture = SDL_CreateTexture(
		renderer, 
		SDL_PIXELFORMAT_ARGB8888, 
		SDL_TEXTUREACCESS_STREAMING, 
		160, 
		144);

	SDL_UpdateWindowSurface(gbWindow);

	SDL_RenderCopy(renderer, gbTexture, NULL, NULL);

}
bool Video::isLCDEnabled() {
	uint8_t result = mm.readAddress(LCDControl);
	bool result2 = getBit(result, 7);
	//printf("%d\n", unsigned(mm.readAddress(LCDControl)));
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

bool Video::isSpritesEnabled() {
	return getBit(mm.readAddress(LCDControl), 1);
}

bool Video::currentWindowPriority() {
	return getBit(mm.readAddress(LCDControl), 0);
}

uint8_t Video::currentPPUMode() {
	return mm.readAddress(LCDStatus) & 0x03;
}

void Video::tick(int cpuCycles) {
	uint8_t currentLY = mm.readAddress(LY);
	uint8_t currentStatus = mm.readAddress(LCDStatus);
	cycles += cpuCycles;
	//printf("New ticks: %d\n", cpuCycles);
	switch (mode) {
	case Mode::OAM_SCAN:
		if (cycles >= OAMCycles) {
			cycles -= OAMCycles;
			//scan for sprites
			if (isSpritesEnabled()) {
				//do oam scan
			}

			if (!isLCDEnabled()) {
				currentFrameBlank = true;
			}
			//preset fetcher values for next mode
			uint8_t SCX = mm.readAddress(scrollX);
			uint8_t SCY = mm.readAddress(scrollY);
			uint8_t windowY = mm.readAddress(WY);

			pixelShift = SCX % 8;
			windowInLine = currentLY >= windowY;

			fetcher.setX(SCX / 8);
			fetcher.setY(currentLY + SCY & 255);
			fetcher.setWindow(0);
			
			clearFIFO(backgroundPixelFIFO);
			clearFIFO(spritePixelFIFO);

			setBit(currentStatus, 0, 1);
			setBit(currentStatus, 1, 1);
			mm.writeAddress(LCDStatus, currentStatus);
			mode = Mode::DRAW_LCD;
		}
		break;
	case Mode::DRAW_LCD:
		//printf("2\n");
		while (drawCycles < cycles) {
			//printf("Draw: %d\n", drawCycles);
			//printf("CPU: %d\n", cycles);
			//printf("Next LCD: %d\n", nextLCDPosition);
			drawCycles++;
			advanceMode3(currentLY, drawCycles);
			
			if (nextLCDPosition >= 160) {
				//printf("Trigger\n");
				nextLCDPosition = 0;
				HBlankCycles = totalScanlineCycles - OAMCycles - drawCycles;
				cycles -= drawCycles;
				drawCycles = 0;
				
				//hblank and ly = lyc interrupt
				if (getBit(mm.readAddress(LCDStatus), 3)) {
					cpu.setInterruptFlag(1);
				}
				bool LYCEqualsLY = mm.readAddress(LYC) == currentLY;
				if (getBit(mm.readAddress(LCDStatus), 6) && LYCEqualsLY) {
					cpu.setInterruptFlag(1);
				}

				setBit(currentStatus, 0, 0);
				setBit(currentStatus, 1, 0);
				setBit(currentStatus, 2, LYCEqualsLY);//LYC = LY
				mm.writeAddress(LCDStatus, currentStatus);
				mode = Mode::HBLANK;
				break;
			}
		}
		break;
	case Mode::HBLANK:
		//printf("HBLANK\n");
		if (cycles >= HBlankCycles) {
			cycles -= HBlankCycles;
			mm.writeAddress(LY, currentLY + 1);//increment LY
			if (currentLY + 1 == 144) {
				setBit(currentStatus, 0, 1);
				setBit(currentStatus, 1, 0);
				mm.writeAddress(LCDStatus, currentStatus);
				cpu.setInterruptFlag(0);//v blank intterupt
				mode = Mode::VBLANK;
			}
			else {
				setBit(currentStatus, 0, 0);
				setBit(currentStatus, 1, 1);
				mm.writeAddress(LCDStatus, currentStatus);
				mode = Mode::OAM_SCAN;
			}
		}
		break;
	case Mode::VBLANK:
		//printf("VBLANK\n");
		if (cycles >= totalScanlineCycles) {
			cycles -= totalScanlineCycles;
			if (currentLY + 1 == 154) {
				mm.writeAddress(LY, 0);//reset LY, start new frame
				setBit(currentStatus, 0, 0);
				setBit(currentStatus, 1, 1);
				mm.writeAddress(LCDStatus, currentStatus);
				mode = Mode::OAM_SCAN;

				//render
				if (currentFrameBlank) {//if LCD was disabled and enabled mid frame
					frameBuffer.fill(0);
					currentFrameBlank = false;
				}
				renderFrameBuffer();
				//printf("New frame\n");
				frameBuffer.fill(0);
			}
			else {
				mm.writeAddress(LY, currentLY + 1);
			}
		}
		break;
	}
}

void Video::advanceMode3(uint8_t currentLY, int drawCycles) {

	/*if (!currentWindowPriority()) {//background and window are blank, sprites still ok
		return;//sprite mixing
	}*/
	if (backgroundPixelFIFO.size() > 8) {
		pushPixelToLCD(currentLY);
	}
	//start fetching from window instead, if enabled
	if (isWindowEnabled() && !fetcher.isFetchingWindow() && windowInLine && nextLCDPosition + 7 == mm.readAddress(WX)) {//check if current x coordinate is within window
		fetcher.setX(0);
		fetcher.setY(currentLY - mm.readAddress(WY));
		fetcher.setWindow(1);
		clearFIFO(backgroundPixelFIFO);
		pixelShift = 0;
	}

	if (drawCycles % 2 == 0) {//attempted every cycle if in push mode?
		fetcher.tick(currentLY);
	}
}

void Video::pushPixelToLCD(uint8_t currentLY) {
	if (nextLCDPosition == 0 && pixelShift > 0) {//discard pixels based on scx at scanline beginning, lengthens mode 3
		pixelShift--;
		backgroundPixelFIFO.pop();
		return;
	}
	frameBuffer.at(currentLY * 160 + nextLCDPosition) = backgroundPixelFIFO.front();
	backgroundPixelFIFO.pop();
	nextLCDPosition++;
}

void Video::renderFrameBuffer() {
	//process other window events first?
	//printf("New frame getting rendered?\n");
	SDL_RenderClear(renderer);

	void* lockedPixels;
	int pitch;
	SDL_LockTexture(gbTexture, NULL, &lockedPixels, &pitch);

	uint32_t* newPixels = static_cast<uint32_t*>(lockedPixels);
	writeFrameBufferData(newPixels);
	SDL_UnlockTexture(gbTexture);
	
	SDL_RenderCopy(renderer, gbTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void Video::writeFrameBufferData(uint32_t* newPixels) {
	for (int y = 0; y < 144; y++) {
		for (int x = 0; x < 160; x++) {
			//printf("%d ", frameBuffer.at(160 * y + x));
			newPixels[160 * y + x] = decipherPixelColor(frameBuffer.at(160 * y + x));
		}
		//printf("\n");
	}
}

uint32_t Video::decipherPixelColor(int pixel) {
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	
	uint32_t color = -1;
	switch (pixel) {
	case 0: r = g = b = 255; break;//white
	case 1: r = g = b = 170; break;//white-gray
	case 2: r = g = b = 85; break;//gray
	case 3: r = g = b = 0; break;//black
	}

	return (r << 16) | (g << 8) | b;
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
			pushPixelToLCD(currentLY);
		}

		if (!windowTriggered && windowInRow && nextLCDPosition + 7 == windowX) {//check if current x coordinate is within window
			windowTriggered = true;
			fetcher.setX(0);
			fetcher.setY(currentLY - windowY);
			fetcher.setWindow(1);
			clearFIFO(backgroundPixelFIFO);
		}

		if (cycleNumber % 2 == 1) {//fetcher steps take 2 cycles
			fetcher.tick(currentLY);
		}
		
		cycleNumber++;
	}
	fetcher.setWindow(0);//disable window for fetcher
}

void Video::clearFIFO(queue<int>& FIFO) {
	queue<int> empty;
	swap(FIFO, empty);
}

void Video::Fetcher::tick(uint8_t currentLY) {
	uint8_t baseTilemapAddress = 0, vramIndex, vramBaseAddress;
	int tilePos;
	bool pixelsPushed = false;
	//3 reads, idle
	switch (currentState) {
	case 1: //get tile index
		if (!fetchingWindow) {		
			baseTilemapAddress = ppu.currentBGTilemap() ? 0x9800 : 0x9C00;		
		}
		else {//access from window tilemap
			baseTilemapAddress = ppu.currentWindowTilemap() ? 0x9800 : 0x9C00;
		}
		tilePos = (fetcherY / 8) * 32 + fetcherX;
		vramIndex = ppu.mm.readAddress(baseTilemapAddress + tilePos);

		//printf("%d\n", ppu.currentAddressMode());
		vramBaseAddress = ppu.currentAddressMode() ? 0x8000 : 0x8800;//change 9000 to 8800?
		nextTileAddress = vramBaseAddress + vramIndex * 16;
		nextTileRow = fetcherY % 8;
		//printf("FetcherX: %d\n", fetcherX);
		//printf("FetcherY: %d\n", fetcherY);
		fetcherX = (fetcherX + 1) & 0x1F;//increment fetcher

		break;
	case 2: //get first byte of tile data
		tileLow = ppu.mm.readAddress(nextTileAddress + 2 * nextTileRow);
		break;
	case 3: //get second byte of tile data, and push if possible
		tileHigh = ppu.mm.readAddress(nextTileAddress + 2 * nextTileRow + 1);
		//printf("Lower: %d\n", tileLow);
		//printf("Upper: %d\n", tileHigh);
		for (int i = 0; i < 8; i++) {
			bool lowerBit = getBit(tileLow, i);
			bool upperBit = getBit(tileHigh, i);
			//printf("Lower: %d\n", lowerBit);
			//printf("Upper: %d\n", upperBit);
			//printf("Pixel color: %d\n", 2 * upperBit + lowerBit);
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
	fetchingWindow = status;
}

bool Video::Fetcher::isFetchingWindow() {
	return fetchingWindow;
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
/*
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
}*/

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
