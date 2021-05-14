#include <cstdint>
#include <cstdlib>
#include "CPU.h"

//jumps
int CPU::opcodeJP() {
	uint16_t address = PCFetchWord();
	PC.set(address);
	return 16;
}

int CPU::opcodeJPHL() {
	PC.set(HL.getValue());//jump to address pointed by HL, but do not actually access it
	return 4;
}

int CPU::opcodeJP(Flag fl) {
	bool result = checkCondition(fl);
	if (result) {
		return opcodeJP();
	}
	else {
		uint16_t word = PCFetchWord();
		return 12;
	}
}

int CPU::opcodeJR() {
	int8_t val = PCFetchSignedByte();
	uint16_t PCVal= PC.getValue();

	PC.set(static_cast<uint16_t>(PCVal + val));
	
	return 12;
}

int CPU::opcodeJR(Flag fl) {
	bool result = checkCondition(fl);
	if (result) { //we do branch
		return opcodeJR();
	}
	else {
		PCFetchSignedByte();
		return 8;
	}
}

int CPU::opcodeRet() {
	opcodeStackPop(PC);
	return 16;
}

int CPU::opcodeRet(Flag fl) {
	bool result = checkCondition(fl);
	if (result) { //we do branch
		opcodeRet();
		return 20;
	}
	else {
		return 8;
	}
}

int CPU::opcodeRetI() {
	opcodeStackPop(PC);
	opcodeEI();
	return 16;
}

int CPU::opcodeCall() {
	uint16_t address = PCFetchWord();
	opcodeStackPush(PC);
	PC.set(address);
	return 24;
}

int CPU::opcodeCall(Flag fl) {
	bool result = checkCondition(fl);
	if (result) { //we do branch
		return opcodeCall();
	}
	else {
		uint16_t word = PCFetchWord();
		return 12;
	}
}

int CPU::opcodeRST(uint8_t location) {
	opcodeStackPush(PC);
	uint16_t address = static_cast<uint16_t>(0x0000 + static_cast<uint16_t>(location));
	PC.set(address);
	return 16;
}

int CPU::opcodeStop() {
	halt = true;
	return 4;
}