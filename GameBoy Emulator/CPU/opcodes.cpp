#include <cstdint>
#include <cstdlib>
#include "CPU.h"

void CPU::setClockPrevious(int ticks) {
	clock.p = ticks;
}

//0x00
void CPU::opcodeNOP() {
	//no operation
	setClockPrevious(4);
}

//load byte
void CPU::opcodeLoadByte(SingleRegister& r) {
	uint8_t newValue = PCFetchByte();
	r.set(newValue);
	setClockPrevious(8);
}

//load words
//0x01, 0x11, 0x21
void CPU::opcodeLoadWord(SplitRegister& r) {
	uint16_t newValue = PCFetchWord();//get 16 bytes from PC
	r.set(newValue);
	setClockPrevious(12);
}

//0x31
void CPU::opcodeLoadWord(FullRegister& r) {
	uint16_t newValue = PCFetchWord();//get 16 bytes from PC
	r.set(newValue);
	setClockPrevious(12);
}

//load accumulator to memory
//0x02, 0x12, 0x22, 0x32
void CPU::opcodeLoadAToMemory(SplitRegister& r) {
	//loads memory address specified by SplitRegister value with accumulator value
	//remember memory has 2 byte addresses, but 1 byte data
	setClockPrevious(8);
}

//increment registers
void CPU::opcodeIncrement(SplitRegister& r) {
	r.increment();
	setClockPrevious(8);
}

void CPU::opcodeIncrement(FullRegister& r) {
	r.increment();
	setClockPrevious(8);
}

void CPU::opcodeIncrement(SingleRegister& r) {
	r.increment();
	setClockPrevious(4);
}

/*void CPU::opcodeIncrement(Address value) {
	//stuff for memory address case like 0x34
	//increment byte value at 2 byte address by 1
	//read memory address to get value, increment value, rewrite to that memory address
	setClockPrevious(12);
}*/

//decrement registers
void CPU::opcodeDecrement(SplitRegister& r) {
	r.decrement();
	setClockPrevious(8);
}

void CPU::opcodeDecrement(FullRegister& r) {
	r.decrement();
	setClockPrevious(8);
}

void CPU::opcodeDecrement(SingleRegister& r) {
	r.decrement();
	setClockPrevious(4);
}

/*void CPU::opcodeDecrement(Address value) {
	//stuff for memory address case like 0x34
	//decrement byte value at 2 byte address by 1
	setClockPrevious(12);
}*/



