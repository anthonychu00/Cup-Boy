#include <cstdint>
#include <cstdlib>
#include "CPU.h"

//0x00
int CPU::opcodeNOP() {
	//no operation
	return 4;
}

int CPU::opcodeHalt() {
	halt = true;
	return 4;
}

int CPU::opcodeEI() {
	IME = true;
	return 4;
}

int CPU::opcodeDI() {
	IME = false;
	return 4;
}