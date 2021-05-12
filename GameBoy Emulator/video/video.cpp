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

void Video::viewTileData() {

	tileWindow = SDL_CreateWindow(
		"Tile Data",
		SDL_WINDOWPOS_UNDEFINED,
		300,
		128,
		128,
		SDL_WINDOW_SHOWN
	);
	tileRenderer = SDL_CreateRenderer(tileWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	tileTexture = SDL_CreateTexture(
		tileRenderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		128,
		128);

	SDL_UpdateWindowSurface(tileWindow);

	SDL_RenderCopy(tileRenderer, tileTexture, NULL, NULL);

	void* lockedPixels;
	int pitch;
	SDL_LockTexture(tileTexture, NULL, &lockedPixels, &pitch);
	uint32_t* newPixels = static_cast<uint32_t*>(lockedPixels);

	uint16_t vramBaseAddress = 0x8000;
	uint16_t vramIndex = 0;
	for (int y = 0; y < 16; y++) {
		for (int x = 0; x < 16; x++) {
			drawTile(x, y, newPixels, vramBaseAddress + vramIndex);
			vramIndex += 16;
		}
	}

	SDL_UnlockTexture(tileTexture);
	SDL_RenderCopy(tileRenderer, tileTexture, NULL, NULL);
	SDL_RenderPresent(tileRenderer);

	/*int counter = 0;
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			printf("%d ", mm.readAddress(0x9800 + counter));
			counter++;
		}
		printf("\n");
	}*/
}

void Video::drawTile(int x, int y, uint32_t* newPixels, uint16_t address) {
	uint8_t tileLow, tileHigh;

	for (int i = 0; i < 8; i++) {
		tileLow = mm.readAddress(address + 2 * i);
		tileHigh = mm.readAddress(address + 2 * i + 1);
		
		for (int j = 0; j < 8; j++) {
			bool lowerBit = getBit(tileLow, j);
			bool upperBit = getBit(tileHigh, j);
			int pixelValue = 2 * upperBit + lowerBit;
			//printf("%d ", pixelValue);
			int xCoord = x * 8 + (7 - j);
			int yCoord = y * 1024 + i * 128;
			newPixels[yCoord + xCoord] = decipherPixelColor(pixelValue);
		}
		//printf("\n");
	}
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

bool Video::isSpritesEnabled() {
	return getBit(mm.readAddress(LCDControl), 1);
}

bool Video::BGWindowEnabled() {
	return getBit(mm.readAddress(LCDControl), 0);
}

uint8_t Video::currentPPUMode() {
	return mm.readAddress(LCDStatus) & 0x03;
}

void Video::tick(int cpuCycles) {
	if (isLCDEnabled() && !tileViewerPresent) {
		viewTileData();
		tileViewerPresent = true;
	}

	uint8_t currentLY = mm.readAddress(LY);
	uint8_t currentStatus = mm.readAddress(LCDStatus);
	cycles += cpuCycles;
	//printf("New ticks: %d\n", cpuCycles);
	switch (mode) {
	case Mode::OAM_SCAN:
		if (cycles >= OAMCycles) {
			cycles -= OAMCycles;
			//scan for sprites
			spritesInLine = {};
			if (!isSpritesEnabled()) {
				printf("sprites disabled on line %d\n", currentLY);
			}
			if (isSpritesEnabled()) {
				//printf("sprite line: %d ", currentLY);
				findScanlineSprites(currentLY);//add locations to spritesInLine queue
				//printf("sprite queue size: %d\n", spritesInLine.size());
			}

			if (!isLCDEnabled()) {
				currentFrameBlank = true;
			}
			//preset fetcher values for next mode
			uint8_t SCX = mm.readAddress(scrollX);
			uint8_t SCY = mm.readAddress(scrollY);
			uint8_t windowY = mm.readAddress(WY);

			if (currentLY == 0x82) {
				printf("%d ********************************************\n", SCX);
			}

			pixelShift = SCX % 8;
			windowInLine = currentLY >= windowY;

			fetcher.setX((SCX / 8));
			fetcher.setY(currentLY + SCY & 255);
			if (fetcher.isFetchingWindow()) {
				fetcher.incrementWindowLine();
			}
			fetcher.setWindow(0);
			fetcher.resetState();
			
			clearFIFO(backgroundPixelFIFO);
			clearFIFO(spritePixelFIFO);
			
			//fire lyc=ly interrupt, if applicable
			bool LYCEqualsLY = mm.readAddress(LYC) == currentLY;
			if (getBit(mm.readAddress(LCDStatus), 6) && LYCEqualsLY) {
				cpu.setInterruptFlag(1);
			}
			setBit(currentStatus, 2, LYCEqualsLY);//LYC = LY

			setBit(currentStatus, 0, 1);
			setBit(currentStatus, 1, 1);
			mm.writeAddress(LCDStatus, currentStatus);
			mode = Mode::DRAW_LCD;
		}
		break;
	case Mode::DRAW_LCD:

		while (drawCycles < cycles) {
			drawCycles++;

			if (spritePixelFIFO.empty() && !spritesInLine.empty()) {
				//printf("current sprite queue size: %d\n", spritesInLine.size());
				tuple<int, int, int> nextSprite = spritesInLine.front();
				int spriteXPos = get<0>(nextSprite) - 8;
				int spriteScroll = nextLCDPosition - spriteXPos;//starting location in row for pixels
				
				//if two sprites completely overlap, need to get rid of the second one and look at thrid sprite instead
				if (spriteScroll >= 0 && spriteScroll < 8) {
					printf("Sprite found at line %d, x-position %d \n", currentLY, nextLCDPosition);
					getSpritePixels(spriteScroll, get<1>(nextSprite), get<2>(nextSprite));
				}
			}
			advanceMode3(currentLY, drawCycles);
			
			if (nextLCDPosition >= 160) {
				nextLCDPosition = 0;
				HBlankCycles = totalScanlineCycles - OAMCycles - drawCycles;
				cycles -= drawCycles;
				drawCycles = 0;
				
				//hblank interrupt
				if (getBit(mm.readAddress(LCDStatus), 3)) {
					cpu.setInterruptFlag(1);
				}

				setBit(currentStatus, 0, 0);
				setBit(currentStatus, 1, 0);
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
				if (currentLY == 103) {
					//printf("LCDC : %d\n", mm.readAddress(LCDControl));
				}
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

				fetcher.resetWindowLine();//reset internal window line counter
				mode = Mode::OAM_SCAN;

				//render
				if (currentFrameBlank) {//if LCD was disabled and enabled mid frame
					frameBuffer.fill(0);
					currentFrameBlank = false;
				}
				renderFrameBuffer();
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

	if (backgroundPixelFIFO.size() > 8) {
		pushPixelToLCD(currentLY);
	}
	//start fetching from window instead, if enabled
	if (isWindowEnabled() && !fetcher.isFetchingWindow() && windowInLine && nextLCDPosition + 7 == mm.readAddress(WX)) {//check if current x coordinate is within window
		fetcher.setX(0);
		fetcher.setY(fetcher.getWindowLine());
		fetcher.setWindow(1);
		fetcher.resetState();
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
	if (!spritePixelFIFO.empty()) {
		tuple<int, int, int> nextSprite = spritesInLine.front();
		uint8_t spriteAttr = mm.readAddress(0xFE00 + 4 * get<2>(nextSprite)  + 3);
		bool bgWindowCovered = getBit(spriteAttr, 7);

		//background and window cover sprite if not transparent
		if (bgWindowCovered && backgroundPixelFIFO.front() == 0) {
			frameBuffer.at(currentLY * 160 + nextLCDPosition) = spritePixelFIFO.front();
		}
		else if (!bgWindowCovered && spritePixelFIFO.front() != 0) {
			frameBuffer.at(currentLY * 160 + nextLCDPosition) = spritePixelFIFO.front();
		}
		spritePixelFIFO.pop();

		if (spritePixelFIFO.empty()) {
			//printf("Decrementing\n");
			spritesInLine.pop();
		}
	}
	backgroundPixelFIFO.pop();
	nextLCDPosition++;
}

void Video::renderFrameBuffer() {
	//process other window events first?
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

void Video::clearFIFO(queue<int>& FIFO) {
	queue<int> empty;
	swap(FIFO, empty);
	//FIFO = {};
}

void Video::Fetcher::tick(uint8_t currentLY) {
	uint16_t baseTilemapAddress = 0, vramIndex, vramBaseAddress;
	int tilePos;
	bool pixelsPushed = false;
	bool bgWindowOn;
	//3 reads, idle
	switch (currentState) {
	case 1: //get tile index
		if (!fetchingWindow) {		
			baseTilemapAddress = ppu.currentBGTilemap() ? 0x9C00 : 0x9800;		
		}
		else {//access from window tilemap
			baseTilemapAddress = ppu.currentWindowTilemap() ? 0x9C00 : 0x9800;
		}
		tilePos = (fetcherY / 8) * 32 + fetcherX;
		vramIndex = ppu.mm.readAddress(baseTilemapAddress + tilePos);
		
		vramBaseAddress = ppu.currentAddressMode() ? 0x8000 : 0x8800;

		//handles signed addressing in 0x8800 mode
		if (vramBaseAddress == 0x8800) {
			if (vramIndex <= 127) {
				vramIndex += 128;
			}
			else if (vramIndex >= 128) {
				vramIndex -= 128;
			}
		}

		nextTileAddress = vramBaseAddress + vramIndex * 16;
		nextTileRow = fetcherY % 8;
		fetcherX = (fetcherX + 1) & 0x1F;//increment fetcher

		break;
	case 2: //get first byte of tile data
		tileLow = ppu.mm.readAddress(nextTileAddress + 2 * nextTileRow);
		break;
	case 3: //get second byte of tile data, and push if possible
		tileHigh = ppu.mm.readAddress(nextTileAddress + 2 * nextTileRow + 1);
		bgWindowOn = ppu.BGWindowEnabled();//******************does window internal counter still go up?
		for (int i = 0; i < 8; i++) {
			bool lowerBit = getBit(tileLow, i);
			bool upperBit = getBit(tileHigh, i);
			if (!bgWindowOn) {
				nextPixels.at(7 - i) = 0;
			}
			else {
				nextPixels.at(7 - i) = 2 * upperBit + lowerBit;
			}
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

void::Video::Fetcher::resetState() {
	currentState = 1;
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

int Video::Fetcher::getWindowLine() {
	return internalWindowLine;
}

void Video::Fetcher::incrementWindowLine() {
	internalWindowLine++;
}

void Video::Fetcher::resetWindowLine() {
	internalWindowLine = 0;
}

void Video::findScanlineSprites(uint8_t currentLY) {
	int spriteSize = currentOBJSize() ? 16 : 8;

	//add support for 8 x 16 later
	for (int i = 0; i < 40; i++) {
		int spriteYPos = static_cast<int>(mm.readAddress(0xFE00 + 4 * i)) - 16;
		int spriteRow = currentLY - spriteYPos;

		if (spriteRow >= 0 && spriteRow < spriteSize) {
			uint8_t spriteXPos = mm.readAddress(0xFE00 + 4 * i + 1);
			printf("Sprite scanned at line %d, x-position %d\n", currentLY, spriteXPos);
			tuple<int, int, int> spriteCoords = make_tuple(spriteXPos, spriteRow, i);//last arg is index of sprite in memory
			spritesInLine.push(spriteCoords);
		}

		if (spritesInLine.size() >= 10) {
			break;
		}
	}
}

void Video::getSpritePixels(int xIndex, int yIndex, int OAMIndex) {
	//xIndex, starting location of pixel row (7 = last pixel only)
	//yIndex, which row in tile at index to get
	//OAMIndex, where to get sprite info in OAM, mm.readAddress(0xFE00 + 4 * OAMIndex)
	int spriteSize = currentOBJSize() ? 15 : 7;
	uint8_t tileIndex = mm.readAddress(0xFE00 + 4 * OAMIndex + 2);
	uint8_t spriteAttr = mm.readAddress(0xFE00 + 4 * OAMIndex + 3);
	uint8_t paletteValues = 0;

	if (currentOBJSize()) {//if sprites are 8 by 16, ignore bit 0 of tile index
		tileIndex = tileIndex & 0xFE;
	}

	uint16_t tileAddress = 0x8000 + tileIndex * 16;

	if (getBit(spriteAttr, 6)) { //is sprite mirrored vertically
		yIndex = spriteSize - yIndex;
	}

	if (getBit(spriteAttr, 4)) { //use OBJ palette 1
		paletteValues = mm.readAddress(0xFF49);
	}
	else {
		paletteValues = mm.readAddress(0xFF48);
	}

	uint8_t tileLow = mm.readAddress(tileAddress + 2 * yIndex);
	uint8_t tileHigh = mm.readAddress(tileAddress + 2 * yIndex + 1);

	
	for (int i = xIndex; i < 8; i++) {
		bool lowerBit = 0, upperBit = 0;

		if (getBit(spriteAttr, 5)) {//is sprite mirrored horizontally
			lowerBit = getBit(tileLow, i);
			upperBit = getBit(tileHigh, i);
		}
		else {
			lowerBit = getBit(tileLow, 7 - i);
			upperBit = getBit(tileHigh, 7 - i);
		}
		
		//do palette stuff here?
		int pixelIndex = 2 * upperBit + lowerBit;
		
		//printf("next pixel value: %d\n", pixelIndex);
		//spritePixelFIFO.push(2 * upperBit + lowerBit);
		spritePixelFIFO.push(applyPalette(pixelIndex, paletteValues));
	}

}

int Video::applyPalette(int pixelIndex, uint8_t paletteValues) {
	int paletteColor = 0;

	switch (pixelIndex) {
	case 0: paletteColor = paletteValues & 0x3; break;
	case 1: paletteColor = (paletteValues >> 2) & 0x3; break;
	case 2: paletteColor = (paletteValues >> 4) & 0x3; break;
	case 3: paletteColor = (paletteValues >> 6) & 0x3; break;
	}

	return paletteColor;
}