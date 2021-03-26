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

	uint8_t interruptFlag = mm.readAddress(IFRegister);
	uint8_t interruptEnable = mm.readAddress(IERegister);
	uint8_t serviceableInterrupts = interruptFlag & interruptEnable;

	if (!IME) {
		if (serviceableInterrupts) {
			printf("Clock me\n");
			halt = false;
			executeTwice = true;
		}
	}

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