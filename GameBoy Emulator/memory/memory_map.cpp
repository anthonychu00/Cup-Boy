#include "memory_map.h"
#include "../CPU/CPU.h"
#include "../joypad/joypad.h"
#include <iostream>

MemoryMap::MemoryMap(CPU& newCpu, Cartridge& newCartridge, Joypad& newJoypad) : 
	cpu(newCpu), 
	cartridge(newCartridge),
	joypad(newJoypad){//Add audio, controller, video, and CPU objects as params (and timer?)

	memory = std::vector<uint8_t>(0x10000);//preinitialize memory to initial size of 0xFFFF
	memory.at(0xFF50) = 0x1;//disable boot rom
}

uint8_t MemoryMap::readAddress(const uint16_t address) {

	//VRAM in between cartridge memory
	if (address >= 0x8000 && address <= 0x9FFF) {
		return memory.at(address);
	}
	//Cartridge memory
	else if (address <= 0xBFFF) {
		return cartridge.readAddress(address);
	}
	else if (address >= 0xE000 && address <= 0xFDFF) {
		return memory.at(address - 0x2000);//access prohibited, return mirror
	}
	else if (address >= 0xFEA0 && address <= 0xFEFF) {
		return 0x00;//access prohibited
	}
	else if (address == 0xFF00) {
		return joypad.getInput();
	}
	else {
		return memory.at(address);
	}
}

void MemoryMap::writeAddress(const uint16_t address, const uint8_t byte) {
	if (address >= 0x8000 && address <= 0x9FFF) {
		memory.at(address) = byte;
	}
	else if (address <= 0xBFFF) {
		cartridge.writeAddress(address, byte);
	}
	else if (address >= 0xE000 && address <= 0xFDFF) {
		memory.at(address - 0x2000) = byte;
		//access prohibited, write to mirror instead
	}
	else if (address == 0xFF00) {
		//write byte to controller
	}
	//serial? FF01 & FF02
	else if (address == 0xFF04) {
		memory.at(address) = 0x0000;
		//reset divider register in timer in CPU?
	}
	else if (address == 0xFF07) {
		memory.at(address) = byte;
		//printf("Clock speed changed: %d\n", byte & 0x03);
		switch (byte & 0x03) {
		case(0): cpu.setClockSpeed(1024); break;
		case(1): cpu.setClockSpeed(16); break;
		case(2): cpu.setClockSpeed(64); break;
		case(3): cpu.setClockSpeed(256); break;
		}
	}
	//sound at FF10 - FF3F?
	//{ }
	else if (address == 0xFF46) {
		//DMA transfer
		memory.at(address) = byte;
		uint16_t source = static_cast<uint16_t>(byte);

		source = source << 8;
		//copy source RAM/ROM to OAM (object attribute memory)
		for (int i = 0; i < 0x9F; i++) {
			memory.at(0xFE00 + i) = memory.at(source + i);
		}
			
	}
	else if (address >= 0xFEA0 && address <= 0xFEFF) {
		//acess prohibited
	}
	else {
		if (address == 0xFF01) {
			//printf("%c", byte);
		}
		
		//std::cout << "writing to " << unsigned(address) << endl;
		memory.at(address) = byte;
	}
}

