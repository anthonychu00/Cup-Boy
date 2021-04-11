#include "video.h"
#include <cmath>
Video::Video(CPU& newcpu, MemoryMap& newmm):cpu(newcpu), mm(newmm) {

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
	queue<pair<int, int>> spriteLocations = findScanlineSprites(currentLY);
	pair<int, int> nextSpritePos = spriteLocations.front();
	if (nextSpritePos.first == 0) { //sprite is hidden, but still affects scanline
		spriteLocations.pop();
		nextSpritePos = spriteLocations.front();
	}
	bool windowTriggered = false;

	uint8_t yPos = currentLY + mm.readAddress(scrollY);
	uint8_t xPos = mm.readAddress(scrollX);

	//uint16_t vramBaseAddress = currentAddressMode() ? 0x8000 : 0x9000;
	uint16_t baseTilemapAddress = currentBGTilemap() ? 0x9800 : 0x9C00;
	uint8_t windowY = mm.readAddress(WY);
	uint8_t windowX = mm.readAddress(WX);

	for (int i = 0; i < screenWidth; i++) {
		/*if (nextSpritePos.first == i) {
			getSpritePixels(nextSpritePos.second, nextSpritePos.first);//always 8
		}*/

		int tilePos = (yPos / 8) * 32 + (xPos / 8);

		if (!windowTriggered && windowY <= currentLY && windowX <= i + 7) {
			baseTilemapAddress = currentWindowTilemap() ? 0x9800 : 0x9C00;
			windowTriggered = true;
		}

		uint8_t vramIndex = mm.readAddress(baseTilemapAddress + static_cast<uint16_t>(tilePos));
		
		int yIndex = yPos % 8;
		int xIndex = xPos % 8;

		int pixel = getPixel(vramIndex, yIndex, xIndex);
		backgroundPixelFIFO.push(pixel);
		//if greater than 8 pixels in queue, push

	}
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

int Video::getPixel(uint8_t vramIndex, uint8_t yIndex, uint8_t xIndex) {
	uint16_t vramBaseAddress = currentAddressMode() ? 0x8000 : 0x9000;
	uint16_t tileStartingAddress = vramBaseAddress + vramIndex * 16;

	uint8_t firstByte = mm.readAddress(tileStartingAddress + 2 * yIndex);
	uint8_t secondByte = mm.readAddress(tileStartingAddress + 2 * yIndex + 1);

	//bit 7 is the leftmost pixel, bit 0 the rightmost
	bool lowerBit = getBit(firstByte, 7 - xIndex);
	bool upperBit = getBit(secondByte, 7 - xIndex);

	return 2 * static_cast<int>(upperBit) + static_cast<int>(lowerBit);
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

void Video::readTileRow(uint8_t firstByte, uint8_t secondByte, array<int, 8>& pixelValues) {
	//bit 0 is rightmost pixel, bit 7 leftmost
	for (int i = 0; i < 8; i++) {
		bool lowerBit = getBit(firstByte, i);
		bool upperBit = getBit(secondByte, i);
		
		//pixel values arranged from left to right
		pixelValues.at(7 - i) = 2 * static_cast<int>(upperBit) + 1 * static_cast<int>(lowerBit);
	}

}




