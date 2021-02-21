#include "cartridge.h"

Cartridge::Cartridge(string path) {
	readRom(path);
}

void Cartridge::readRom(string path) {
	//put rom into a data type
	FILE* rom = fopen(path.c_str(), "rb");
	if (rom == NULL) {
		printf("No rom found");
		return;
	}

	fseek(rom, 0, SEEK_END);
	romSize = ftell(rom);
	rewind(rom);

	romData.resize(romSize);
	fread(romData, romSize, 1, rom);

	fclose(rom);
}