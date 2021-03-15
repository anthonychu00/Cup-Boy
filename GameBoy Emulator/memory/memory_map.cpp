#include "memory_map.h"
#include <iostream>
MemoryMap::MemoryMap(CPU& newCpu, Cartridge& newCartridge) : 
	cpu(newCpu), 
	cartridge(newCartridge) {//Add audio, controller, video, and CPU objects as params (and timer?)

	memory = std::vector<uint8_t>(0x10000);//preinitialize memory to initial size of 0xFFFF
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
		return 0x00;
		//replace with getting input byte from controller
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
	//ff07
	else if (address == 0xFF07) {
		memory.at(address) = byte;
		//change timer control
	}
	else if (address == 0xFF0F) {
		//interrupt flag
	}
	//sound at FF10 - FF3F?
	//{ }
	//LCD control
	else if (address == 0xFF40) {

	}
	//LCD Status
	else if (address == 0xFF41) {

	}
	else if (address == 0xFF46) {
		//DMA transfer
	}
	else if (address >= 0xFEA0 && address <= 0xFEFF) {
		//acess prohibiterd
	}
	else {
		if (address == 0xFF01) {
			printf("%c", byte);
		}
		//std::cout << "writing to " << unsigned(address) << endl;
		memory.at(address) = byte;
	}
}

