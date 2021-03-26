#include <cstdint>
#include <cstdlib>
#include "CPU.h"

//load byte
int CPU::opcodeLoadByte(SingleRegister& r) {
	uint8_t newValue = PCFetchByte();
	r.set(newValue);
	return 8;
}

int CPU::opcodeLoadByte(SingleRegister& r, const SingleRegister& newReg) {
	r.set(newReg.getByte());
	return 4;
}

//get byte from program counter
int CPU::opcodeLoadByte(const uint16_t address) {
	uint8_t toWrite = PCFetchByte();
	mm.writeAddress(address, toWrite);
	return 12;
}

//0x66
int CPU::opcodeLoadByte(SingleRegister& r, const uint16_t address) {
	uint8_t toWrite = mm.readAddress(address);
	r.set(toWrite);
	return 8;
}

int CPU::opcodeLoadByte(const uint16_t address, const SingleRegister& r) {
	mm.writeAddress(address, r.getByte());
	return 8;
}

//load words

int CPU::opcodeLoadWord(Word& r) {
	uint16_t newValue = PCFetchWord();//get 16 bytes from PC
	r.set(newValue);
	return 12;
}

//0x08
int CPU::opcodeLoadSPToMemory() {
	/*Address is a uint16_t grabbed from program counter
	- write SP.low() to (u16) address
	- write SP.high() to (u16 + 1) address*/
	uint16_t address = PCFetchWord();
	mm.writeAddress(address, SP.getLow());
	mm.writeAddress(address + 1, SP.getHigh());
	
	return 20;
}

int CPU::opcodeLoadHLSP() { //0xF8
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

	return 12;
}

int CPU::opcodeLoadSPHL() {
	SP.set(HL.getValue());
	return 8;
}

//load accumulator to memory
int CPU::opcodeLoadAToMemory16() {
	uint16_t address = PCFetchWord();
	//put value of A into address pointed to by uint16_t address
	mm.writeAddress(address, AF.getHigh());
	return 16;
}

int CPU::opcodeLoadAToMemory() {
	uint8_t location = PCFetchByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));

	//load A into 16 bit memory address specified above
	mm.writeAddress(address, AF.getHigh());
	return 12;
}

int CPU::opcodeLoadAToMemory(const SingleRegister& r) {
	uint8_t location = r.getByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));

	//load A into memory address above
	mm.writeAddress(address, AF.getHigh());
	return 8;
}

int CPU::opcodeLoadAToMemory(const uint16_t address) {
	//loads memory address specified by SplitRegister value with accumulator value
	mm.writeAddress(address, AF.getHigh());

	return 8;
}

//0x22
int CPU::opcodeLoadAToMemoryInc(const uint16_t address) {
	int toReturn = opcodeLoadAToMemory(address);
	HL.increment();
	return toReturn;
}

//0x32
int CPU::opcodeLoadAToMemoryDec(const uint16_t address) {
	int toReturn = opcodeLoadAToMemory(address);
	HL.decrement();
	return toReturn;
}

//0xFA
int CPU::opcodeLoadMemoryToA16() {
	uint16_t address = PCFetchWord();

	
	//put 8-bit value of uint16_t memory address into A
	uint8_t toWrite = mm.readAddress(address);
	AF.setHigh(toWrite);

	return 16;
}

//0xF0
int CPU::opcodeLoadMemoryToA() {
	uint8_t location = PCFetchByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));
	
	uint8_t toWrite = mm.readAddress(address);
	
	AF.setHigh(toWrite);
	
	//load 16 bit address's value into A
	return 12;// 4 + 8 = 12
}

int CPU::opcodeLoadMemoryToA(const SingleRegister& r) {
	uint8_t location = r.getByte();
	uint16_t address = static_cast<uint16_t>(0xFF00 + static_cast<uint16_t>(location));

	uint8_t toWrite = mm.readAddress(address);
	AF.setHigh(toWrite);

	//load 16 bit address value in memory to A
	return 8;
}

//load byte in memory to accumulator (0x0A)
int CPU::opcodeLoadMemoryToA(const uint16_t address) {
	uint8_t toWrite = mm.readAddress(address);
	AF.setHigh(toWrite);

	return 8;
}

int CPU::opcodeLoadMemoryToAInc(const uint16_t address) {
	int toReturn = opcodeLoadMemoryToA(address);
	HL.increment();
	return toReturn;
}

int CPU::opcodeLoadMemoryToADec(const uint16_t address) {
	int toReturn = opcodeLoadMemoryToA(address);
	HL.decrement();
	return toReturn;
}

//pop 2 bytes from SP and load into 2 registers
int CPU::opcodeStackPop(Word& r, bool flagInvolved) {
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
	
	return 12;
}

int CPU::opcodeStackPush(const Word& r) {
	SP.decrement();
	mm.writeAddress(SP.getValue(), r.getHigh());
	SP.decrement();
	mm.writeAddress(SP.getValue(), r.getLow());

	return 16;
}