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

//0x08
/*void CPU::opcodeLoadWord(Address, SP) {
	Address is a uint16_t grabbed from program counter
	- write SP.low() to (u16) address
	- write SP.high() to (u16 + 1) address
	setClockPrevious(20);
}*/

//load accumulator to memory
//0x02, 0x12, 0x22, 0x32
void CPU::opcodeLoadAToMemory(SplitRegister& r) {
	//loads memory address specified by SplitRegister value with accumulator value
	//remember memory has 2 byte addresses, but 1 byte data
	setClockPrevious(8);
}

//load byte in memory to accumulator
void CPU::opcodeLoadMemoryToA(SplitRegister& r) {
	//opposite of above, implement once memory is done
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


//Rotates A to the left with bit 7 being moved to bit 0 and also stored into the carry.
void CPU::opcodeRLCA() {
	uint8_t temp = AF.getHighRegister().getByte();//gets value of A register's byte
	F.setCarryFlag((temp >> 7) & 0x1);//sets carry flag bit to the 7th bit of A
	temp = ((temp << 1) | (temp >> 7));//rotate A left 
	AF.getHighRegister().set(temp);

	setClockPrevious(4);
}

//Rotates A register to the left with the carry's value put into bit 0 and bit 7 is put into the carry.
void CPU::opcodeRLA() {
	uint8_t temp = AF.getHighRegister().getByte();
	bool carryValue = F.getCarryFlag();//gets carry flag value for later

	F.setCarryFlag((temp >> 7) & 0x1);//sets carry flag to bit 7 of A's value
	temp = ((temp << 1) | (temp >> 7));//rotate A left
	temp = (temp & ~(1UL << 0)) | (carryValue << 0);//set bit 0 of A to carry value
	AF.getHighRegister().set(temp);//update A value in register

	setClockPrevious(4);
}


void CPU::opcodeRRCA() {
	//*******************
	setClockPrevious(4);
}

void CPU::opcodeRRA() {
	//*************************
	setClockPrevious(4);
}




//Decimal adjust register A
void CPU::opcodeDAA() {
	uint8_t a = AF.getHighRegister().getByte();
	if(!F.getAddSubFlag()) { //addition was performed (AddSubFlag = 0)
		if (F.getCarryFlag() || a > 0x99) {
			a += 0x60;
			F.setCarryFlag(1);//used if a > 0x99 is true, but F.getCarryFlag() = 0;
		}
		if(F.getHalfCarryFlag() || (a & 0x0f) > 0x09) {
			a += 0x6;
		}
	}
	else { //subtraction was performed (AddSubFlag = 1)
		if (F.getCarryFlag()) {a -= 0x60;}
		if (F.getHalfCarryFlag()) { a -= 0x6; }
	}

	F.setZeroFlag(a == 0);//see if A is 0
	F.setHalfCarryFlag(0);//we reset half carry flag always

	AF.getHighRegister().set(a);//set A to adjusted value
	setClockPrevious(4);
}

//set carry flag and reset other flags
void CPU::opcodeSCF() {
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);
	F.setCarryFlag(1);
	setClockPrevious(4);
}

void CPU::opcodeJR() {
	//get a signed byte from program counter (increment)
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


void CPU::opcodeAddHL(SplitRegister& addedVal) {
	uint16_t HLVal = HL.getValue();

	HLVal += addedVal.getValue();

	F.setAddSubFlag(0);
	F.setHalfCarryFlag(1);//***********************TODO:set if carried rom bit 11
	F.setCarryFlag(1);//*******************TODO: set if carried from bit 15

	HL.set(HLVal);
	setClockPrevious(8);
}

void CPU::opcodeAddHL(FullRegister& addedVal) {
	uint16_t HLVal = HL.getValue();

	HLVal += addedVal.getValue();

	F.setAddSubFlag(0);
	F.setHalfCarryFlag(1);//***********************TODO:set if carried rom bit 11
	F.setCarryFlag(1);//*******************TODO: set if carried from bit 15

	HL.set(HLVal);
	setClockPrevious(8);
}

