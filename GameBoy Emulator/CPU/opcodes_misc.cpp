#include <cstdint>
#include <cstdlib>
#include "CPU.h"

//0x00
void CPU::opcodeNOP() {
	//no operation
	setClockPrevious(4);
}

void CPU::opcodeHalt() {
	halt = true;
	setClockPrevious(4);
}

void CPU::opcodeEI() {
	interrupts = true;
	setClockPrevious(4);
}

void CPU::opcodeDI() {
	interrupts = false;
	setClockPrevious(4);
}