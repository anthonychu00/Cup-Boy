#include <cstdint>
#include <cstdlib>
#include "CPU.h"

//jumps
void CPU::opcodeJP() {
	uint16_t address = PCFetchWord();
	PC.set(address);
	setClockPrevious(16);
}

void CPU::opcodeJPHL() {
	PC.set(HL.getValue());//jump to address pointed by HL, but do not actually access it
	setClockPrevious(4);
}
void CPU::opcodeJP(Flag fl) {
	bool result = checkCondition(fl);
	if (result) {
		opcodeJP();
	}
	else {
		uint16_t word = PCFetchWord();
		setClockPrevious(12);
	}
}

void CPU::opcodeJR() {
	int8_t val = PCFetchSignedByte();
	uint16_t PCVal= PC.getValue();

	PC.set(static_cast<uint16_t>(PCVal + val));
	
	setClockPrevious(12);
}

void CPU::opcodeJR(Flag fl) {
	bool result = checkCondition(fl);
	if (result) { //we do branch
		opcodeJR();
	}
	else {
		//fetch signed byte from program counter, but do nothing with it
		setClockPrevious(8);
	}
}

void CPU::opcodeRet() {
	opcodeStackPop(PC);
	setClockPrevious(16);
}

void CPU::opcodeRet(Flag fl) {
	bool result = checkCondition(fl);
	if (result) { //we do branch
		opcodeRet();
		setClockPrevious(20);
	}
	else {
		setClockPrevious(8);
	}
}

void CPU::opcodeRetI() {
	opcodeStackPop(PC);
	opcodeEI();
	setClockPrevious(16);
}

void CPU::opcodeCall() {
	uint16_t address = PCFetchWord();
	opcodeStackPush(PC);
	PC.set(address);
	setClockPrevious(24);
}

void CPU::opcodeCall(Flag fl) {
	bool result = checkCondition(fl);
	if (result) { //we do branch
		opcodeCall();
	}
	else {
		uint16_t word = PCFetchWord();
		setClockPrevious(12);
	}
}

void CPU::opcodeRST(uint8_t location) {
	opcodeStackPush(PC);
	uint16_t address = static_cast<uint16_t>(0x0000 + static_cast<uint16_t>(location));
	PC.set(address);
	setClockPrevious(16);
}

void CPU::opcodeStop() {
	halt = true;
}