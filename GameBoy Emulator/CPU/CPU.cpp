#include "CPU.h"

void CPU::executeOperations() {
	for (int ticks = 0; ticks <= 69905;) {
		uint16_t PCValue = PC.getValue();
		//uint8_t opcode = PCFetchByte();
		//int ticksToExecute = executeOpcode(opcode, PCValue);
		//ticks += ticksToExecute;
	}
}

uint8_t CPU::PCFetchByte() {
	//uint8_t byte = memory fetcher stuff
	PC.increment();

	//return byte;
}

uint16_t CPU::PCFetchWord() {
	//lower byte is first in memory from the PC according to timings
	uint8_t low = PCFetchByte();
	uint8_t high = PCFetchByte();

	return ((uint16_t)low << 8) | high;
}

int CPU::executeOpcode(uint8_t opcode, uint16_t PCValue) {
	//perform action according to the opcode
	switch (opcode) {
		case 0x00: opcodeNOP(); break;
		case 0x01: opcodeLoadWord(BC); break;
		case 0x02: opcodeLoadAToMemory(BC); break;
		case 0x03: opcodeIncrement(BC); break;
		case 0x04: opcodeIncrement()
		case 0x11: opcodeLoadWord(DE); break;
		case 0x12: opcodeLoadAToMemory(DE); break;
		case 0x13: opcodeIncrement(DE); break;
		case 0x21: opcodeLoadWord(HL); break;
		case 0x22: opcodeLoadAToMemory(HL); break;//examine difference
		case 0x23: opcodeIncrement(HL); break;
		case 0x31: opcodeLoadWord(SP); break;
		case 0x32: opcodeLoadAToMemory(HL); break;//examine difference
		case 0x33: opcodeIncrement(SP); break;
	}

	return clock.p;
	
}

