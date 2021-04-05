#include "video.h"

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

void Video::readTileRow(uint16_t tileRow, array<int, 8>& pixelValues) {
	//bit 0 is rightmost pixel, bit 7 leftmost
	for (int i = 0; i < 8; i++) {
		bool lowerBit = getBit(tileRow, i);
		bool upperBit = getBit(tileRow, i + 8);
		
		//pixel values arranged from left to right
		pixelValues.at(7 - i) = 2 * static_cast<int>(upperBit) + 1 * static_cast<int>(lowerBit);
	}

}




