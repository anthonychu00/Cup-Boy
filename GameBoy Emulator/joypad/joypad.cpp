#include "joypad.h"

Joypad::Joypad(CPU& newCpu, MemoryMap& newMm) :
	cpu(newCpu),
	mm(newMm){
}

uint8_t Joypad::getInput() {
	return currentState;
}

