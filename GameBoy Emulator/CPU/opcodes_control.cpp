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
void CPU::opcodeJP(Condition c) {
	bool result = checkCondition(c);
	if (result) {
		opcodeJP();
	}
	else {
		uint16_t word = PCFetchWord();
		setClockPrevious(12);
	}
}

void CPU::opcodeJR() {
	//get a signed byte from program counter (increment PC as well)
	//get value of program counter (address its pointing to)
	//add the signed byte to said address
	//set PC to this added value, thereby jumping to it
	setClockPrevious(12);
}

void CPU::opcodeJR(Condition c) {
	bool result = checkCondition(c);
	if (result) { //we do branch
		opcodeJR();
	}
	else {
		//fetch signed byte from program coounter, but do nothing with it
		setClockPrevious(8);
	}
}

void CPU::opcodeRet() {
	stackPop(PC);
	setClockPrevious(16);
}

void CPU::opcodeRet(Condition c) {
	bool result = checkCondition(c);
	if (result) { //we do branch
		opcodeRet();
		setClockPrevious(20);
	}
	else {
		setClockPrevious(8);
	}
}

void CPU::opcodeRetI() {
	stackPop(PC);
	opcodeEI();
	setClockPrevious(16);
}

void CPU::opcodeCall() {
	uint16_t address = PCFetchWord();
	stackPush(PC);
	PC.set(address);
	setClockPrevious(24);
}

void CPU::opcodeCall(Condition c) {
	bool result = checkCondition(c);
	if (result) { //we do branch
		opcodeCall();
	}
	else {
		uint16_t word = PCFetchWord();
		setClockPrevious(12);
	}
}

void CPU::opcodeRST(uint8_t location) {
	stackPush(PC);
	uint16_t address = static_cast<uint16_t>(0x0000 + static_cast<uint16_t>(location));
	PC.set(address);
	setClockPrevious(16);
}