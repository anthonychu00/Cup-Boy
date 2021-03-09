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

//get byte from program counter
void CPU::opcodeLoadByte(const uint16_t address) {
	uint8_t toWrite = PCFetchByte();
	mm.writeAddress(address, toWrite);
	setClockPrevious(12);
}

//0x66
void CPU::opcodeLoadByte(SingleRegister& r, const uint16_t address) {
	uint8_t toWrite = mm.readAddress(address);
	r.set(toWrite);
	setClockPrevious(8);
}

void CPU::opcodeLoadByte(const uint16_t address, const SingleRegister& r) {
	mm.writeAddress(address, r.getByte());
	setClockPrevious(8);
}

//load words

void CPU::opcodeLoadWord(Word& r) {
	uint16_t newValue = PCFetchWord();//get 16 bytes from PC
	r.set(newValue);
	setClockPrevious(12);
}

//0x08
void CPU::opcodeLoadSPToMemory() {
	/*Address is a uint16_t grabbed from program counter
	- write SP.low() to (u16) address
	- write SP.high() to (u16 + 1) address*/
	uint16_t address = PCFetchWord();
	mm.writeAddress(address, SP.getLow());
	mm.writeAddress(address + 1, SP.getHigh());
	
	setClockPrevious(20);
}

void CPU::opcodeLoadHLSP() { //0xF8
	int8_t addedVal = PCFetchSignedByte();
	uint16_t sval = SP.getValue();
	uint16_t result = sval + addedVal;

	HL.set(result);


	F.setZeroFlag(0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((SP.getValue() & 0x0F) + (addedVal & 0x0F) > 0x0F);
	F.setCarryFlag((SP.getValue() & 0xFF) + (addedVal & 0xFF) > 0xFF);
	//F.setHalfCarryFlag(((sval ^ addedVal ^ result) & 0x10) == 0x10);
	//F.setCarryFlag(((sval ^ addedVal ^ result) & 0x100) == 0x100);
	


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
	mm.writeAddress(address, AF.getHigh());
	setClockPrevious(16);
}

void CPU::opcodeLoadAToMemory() {
	uint8_t location = PCFetchByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));

	//load A into 16 bit memory address specified above
	mm.writeAddress(address, AF.getHigh());
	setClockPrevious(12);
}

void CPU::opcodeLoadAToMemory(const SingleRegister& r) {
	uint8_t location = r.getByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));

	//load A into memory address above
	mm.writeAddress(address, AF.getHigh());
	setClockPrevious(8);
}

void CPU::opcodeLoadAToMemory(const uint16_t address) {
	//loads memory address specified by SplitRegister value with accumulator value
	mm.writeAddress(address, AF.getHigh());

	setClockPrevious(8);
}

//0x22
void CPU::opcodeLoadAToMemoryInc(const uint16_t address) {
	opcodeLoadAToMemory(address);
	HL.increment();
}

//0x32
void CPU::opcodeLoadAToMemoryDec(const uint16_t address) {
	opcodeLoadAToMemory(address);
	HL.decrement();
}

void CPU::opcodeLoadMemoryToA16() {
	uint16_t address = PCFetchWord();
	//put 8-bit value of uint16_t memory address into A
	uint8_t toWrite = mm.readAddress(address);
	AF.setHigh(toWrite);

	setClockPrevious(16);
}

void CPU::opcodeLoadMemoryToA() {
	uint8_t location = PCFetchByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));
	
	uint8_t toWrite = mm.readAddress(address);
	AF.setHigh(toWrite);

	//load 16 bit address's value into A
	setClockPrevious(12);
}

void CPU::opcodeLoadMemoryToA(const SingleRegister& r) {
	uint8_t location = r.getByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));

	uint8_t toWrite = mm.readAddress(address);
	AF.setHigh(toWrite);

	//load 16 bit address value in memory to A
	setClockPrevious(8);
}

//load byte in memory to accumulator (0x0A)
void CPU::opcodeLoadMemoryToA(const uint16_t address) {
	uint8_t toWrite = mm.readAddress(address);
	AF.setHigh(toWrite);

	setClockPrevious(8);
}

void CPU::opcodeLoadMemoryToAInc(const uint16_t address) {
	opcodeLoadMemoryToA(address);
	HL.increment();
}

void CPU::opcodeLoadMemoryToADec(const uint16_t address) {
	opcodeLoadMemoryToA(address);
	HL.decrement();
}

//pop 2 bytes from SP and load into 2 registers
void CPU::opcodeStackPop(Word& r, bool flagInvolved) {
	uint8_t low = mm.readAddress(SP.getValue());
	SP.increment();
	uint8_t high = mm.readAddress(SP.getValue());
	SP.increment();

	if (flagInvolved) {
		low = low & 0xF0;
		//printf("AF Low: %d, Flag: %d \n", unsigned(low), unsigned(F.getByte()));
	}

	uint16_t word = ((uint16_t)high << 8) | low;
	r.set(word);
	
}

void CPU::opcodeStackPush(const Word& r) {
	SP.decrement();
	mm.writeAddress(SP.getValue(), r.getHigh());
	SP.decrement();
	mm.writeAddress(SP.getValue(), r.getLow());
}