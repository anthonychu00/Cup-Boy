#include <cstdint>
#include <cstdlib>
#include "CPU.h"

//load byte
void CPU::opcodeLoadByte(SingleRegister& r) {
	uint8_t newValue = PCFetchByte();
	r.set(newValue);
	setClockPrevious(8);
}

void CPU::opcodeLoadByte(SingleRegister& r, const SingleRegister& newReg) {
	r.set(newReg.getByte());
	setClockPrevious(4);
}

void CPU::opcodeLoadByte(SingleRegister& r, uint16_t address) {
	//load byte stored in address (from HL) in memory to register r
	setClockPrevious(8);
}

void CPU::opcodeLoadByte(uint16_t address, SingleRegister& r) {
	//load byte in register r to memory address pointed to by HL
	setClockPrevious(8);
}

//load words

void CPU::opcodeLoadWord(Word& r) {
	uint16_t newValue = PCFetchWord();//get 16 bytes from PC
	r.set(newValue);
	setClockPrevious(12);
}

//0x08
/*void CPU::opcodeLoadWord(Address, SP) {
	Address is a uint16_t grabbed from program counter
	- write SP.low() to (u16) address
	- write SP.high() to (u16 + 1) address
	setClockPrevious(20);
}*/

void CPU::opcodeLoadHLSP() {
	int8_t addedVal = static_cast<int8_t>(PCFetchByte());
	HL.set(SP.getValue() + static_cast<uint16_t>(addedVal));

	F.setZeroFlag(0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((SP.getValue() & 0x0F) + (addedVal & 0x0F) > 0x0F);
	F.setCarryFlag((SP.getValue() & 0xFF) + (addedVal & 0xFF) > 0xFF);

	setClockPrevious(12);
}

void CPU::opcodeLoadSPHL() {
	SP.set(HL.getValue());
	setClockPrevious(8);
}

//load accumulator to memory
void CPU::opcodeLoadAToMemory16() {
	uint16_t address = PCFetchWord();
	//put value of A into address pointed to by uint16_t address
	setClockPrevious(16);
}

void CPU::opcodeLoadAToMemory() {
	uint8_t location = PCFetchByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));

	//load A into 16 bit memory address specified above
	setClockPrevious(12);
}

void CPU::opcodeLoadAToMemory(SingleRegister& r) {
	uint8_t location = r.getByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));

	//load A into memory address above
	setClockPrevious(8);
}

//0x02, 0x12, 0x22, 0x32
void CPU::opcodeLoadAToMemory(SplitRegister& r) {
	//loads memory address specified by SplitRegister value with accumulator value
	//remember memory has 2 byte addresses, but 1 byte data
	setClockPrevious(8);
}

void CPU::opcodeLoadMemoryToA16() {
	uint16_t address = PCFetchWord();
	//put 8-bit value of uint16_t memory address into A
	setClockPrevious(16);
}

void CPU::opcodeLoadMemoryToA() {
	uint8_t location = PCFetchByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));

	//load 16 bit address's value into A
	setClockPrevious(12);
}

void CPU::opcodeLoadMemoryToA(SingleRegister& r) {
	uint8_t location = r.getByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));

	//load 16 bit address value in memory to A
	setClockPrevious(8);
}

//load byte in memory to accumulator
void CPU::opcodeLoadMemoryToA(SplitRegister& r) {
	//opposite of above, implement once memory is done
	setClockPrevious(8);
}

//pop 2 bytes from SP and load into 2 registers
void CPU::stackPop(Word& r) {
	//get lower byte memory from address in memory pointed to by SP
	//SP.increment();
	//get higher byte memory from address
	//SP.increment();

	//word = ((uint16_t)low.getByte() << 8) | high.getByte();
	//set register to popped val (jumps if PC)
}

void CPU::stackPush(const Word& r) {
	//decrement SP
	//write r -> high to address pointed to by SP
	//decrement
	//write r-> low to new address pointed by SP
}