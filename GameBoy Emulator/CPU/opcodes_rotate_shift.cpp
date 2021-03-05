#include <cstdint>
#include <cstdlib>
#include "CPU.h"

//rotates val to the left with bit 7 being moved to bit 0 and also stored into the carry
uint8_t CPU::opcodeRLCAux(uint8_t val) {
	F.setCarryFlag((val >> 7) & 0x1);//sets carry flag bit to the bit 7 of val
	val = ((val << 1) | (val >> 7));//rotate val left 

	F.setZeroFlag(val == 0);
	F.setHalfCarryFlag(0);
	F.setAddSubFlag(0);

	return val;
}

void CPU::opcodeRLCA() {
	uint8_t toSet = opcodeRLCAux(AF.getHighRegister().getByte());
	AF.getHighRegister().set(toSet);
	F.setZeroFlag(0);//RLCA  sets the zero flag to 0 instead
	setClockPrevious(4);
}

void CPU::opcodeRLC(SingleRegister& r) {
	uint8_t toSet = opcodeRLCAux(r.getByte());
	r.set(toSet);
	setClockPrevious(8);
}

void CPU::opcodeRLC(uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	uint8_t toSet = opcodeRLCAux(byte);
	mm.writeAddress(address, toSet);
	setClockPrevious(16);
}

//Rotates val register to the left with the carry's value put into bit 0 and bit 7 is put into the carry.
uint8_t CPU::opcodeRLAux(uint8_t val) {
	
	bool carryValue = F.getCarryFlag();//gets carry flag value for later

	F.setCarryFlag((val >> 7) & 0x1);//sets carry flag to bit 7 of val's value
	val = ((val << 1) | (val >> 7));//rotate val left
	val = (val & ~(1UL << 0)) | (carryValue << 0);//set bit 0 of val to carry value

	F.setZeroFlag(val == 0);
	F.setHalfCarryFlag(0);
	F.setAddSubFlag(0);

	return val;
}

void CPU::opcodeRLA() {
	uint8_t toSet = opcodeRLAux(AF.getHighRegister().getByte());
	AF.getHighRegister().set(toSet);
	F.setZeroFlag(0);
	setClockPrevious(4);
}

void CPU::opcodeRL(SingleRegister& r) {
	uint8_t toSet = opcodeRLAux(r.getByte());
	r.set(toSet);
	setClockPrevious(8);
}

void CPU::opcodeRL(uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	uint8_t toSet = opcodeRLAux(byte);
	mm.writeAddress(address, toSet);
	setClockPrevious(16);
}

//Rotates val to the right with bit 0 moved to bit 7 and also stored into the carry.
uint8_t CPU::opcodeRRCAux(uint8_t val) {
	
	F.setCarryFlag((val >> 0) & 0x1);
	val = ((val >> 1) | (val << 7));

	F.setZeroFlag(val == 0);
	F.setHalfCarryFlag(0);
	F.setAddSubFlag(0);

	return val;
}


void CPU::opcodeRRCA() {
	uint8_t toSet = opcodeRRCAux(AF.getHighRegister().getByte());
	AF.getHighRegister().set(toSet);
	F.setZeroFlag(0);
	setClockPrevious(4);
}

void CPU::opcodeRRC(SingleRegister& r) {
	uint8_t toSet = opcodeRRCAux(r.getByte());
	r.set(toSet);
	setClockPrevious(8);
}

void CPU::opcodeRRC(uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	uint8_t toSet = opcodeRRCAux(byte);
	mm.writeAddress(address, toSet);
	setClockPrevious(16);
}

//Rotates val to the right with the carry put into bit 7 and bit 0 put into the carry flag.
uint8_t CPU::opcodeRRAux(uint8_t val) {
	bool carryValue = F.getCarryFlag();

	F.setCarryFlag((val >> 0) & 0x1);
	val = ((val >> 1) | (val << 7));
	val = (val & ~(1UL << 7)) | (carryValue << 7);//set bit 7 of val to carry value

	F.setZeroFlag(val == 0); 
	F.setHalfCarryFlag(0);
	F.setAddSubFlag(0);

	return val;
}

void CPU::opcodeRRA() {
	uint8_t toSet = opcodeRRAux(AF.getHighRegister().getByte());
	AF.getHighRegister().set(toSet);
	F.setZeroFlag(0);
	setClockPrevious(4);
}

void CPU::opcodeRR(SingleRegister& r) {
	uint8_t toSet = opcodeRRAux(r.getByte());
	r.set(toSet);
	setClockPrevious(8);
}

void CPU::opcodeRR(uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	uint8_t toSet = opcodeRRAux(byte);
	mm.writeAddress(address, toSet);
	setClockPrevious(16);
}

//shift val left, with bit 7 stored into carry. Bit 0 is set to 0
uint8_t CPU::opcodeSLAAux(uint8_t val) {
	F.setCarryFlag((val >> 7) & 0x1);//sets carry flag bit to the 7th bit of val
	val = val << 1;

	F.setZeroFlag(val == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);

	return val;
}

void CPU::opcodeSLA(SingleRegister& r) {
	uint8_t toSet = opcodeSLAAux(r.getByte());
	r.set(toSet);
	setClockPrevious(8);
}

void CPU::opcodeSLA(uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	uint8_t toSet = opcodeSLAAux(byte);
	mm.writeAddress(address, toSet);
	setClockPrevious(16);
}

//shifts val right, bit 0 copied to carry, bit 7 is unchanged
uint8_t CPU::opcodeSRAAux(uint8_t val) {
	F.setCarryFlag((val >> 0) & 0x1);//sets carry flag bit to the 0th bit of val
	bool bit7 = (val >> 7) & 0x1;

	val = val >> 1;//shift val right
	val = (val & ~(1UL << 7)) | (bit7 << 7);//set bit 7 to its previous value

	F.setZeroFlag(val == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);

	return val;
}

void CPU::opcodeSRA(SingleRegister& r) {
	uint8_t toSet = opcodeSRAAux(r.getByte());
	r.set(toSet);
	setClockPrevious(8);
}

void CPU::opcodeSRA(uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	uint8_t toSet = opcodeSRAAux(byte);
	mm.writeAddress(address, toSet);
	setClockPrevious(16);
}

//SRA but bit 7 is set to 0
uint8_t CPU::opcodeSRLAux(uint8_t val) {

	F.setCarryFlag((val >> 0) & 0x1);//sets carry flag bit to the 0th bit of val

	val = val >> 1;//shift val right, also resets bit 7

	F.setZeroFlag(val == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);

	return val;
}

void CPU::opcodeSRL(SingleRegister& r) {
	uint8_t toSet = opcodeSRLAux(r.getByte());
	r.set(toSet);
	setClockPrevious(8);
}

void CPU::opcodeSRL(uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	uint8_t toSet = opcodeSRLAux(byte);
	mm.writeAddress(address, toSet);
	setClockPrevious(16);
}

//swap the lower and upper bytes
uint8_t CPU::opcodeSwapAux(uint8_t val) {

	val = static_cast<uint8_t>(((val & 0x0F) << 4 | (val & 0xF0) >> 4));
	F.setZeroFlag(val == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);
	F.setCarryFlag(0);

	return val;
}

void CPU::opcodeSwap(SingleRegister& r) {
	uint8_t toSet = opcodeSwapAux(r.getByte());
	r.set(toSet);
	setClockPrevious(8);
}

void CPU::opcodeSwap(uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	uint8_t toSet = opcodeSwapAux(byte);
	mm.writeAddress(address, toSet);
	setClockPrevious(16);
}

void CPU::opcodeBit(const int position, SingleRegister& r) {
	uint8_t val = r.getByte();
	bool test = (val >> position) & 0x1;

	F.setZeroFlag(test == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(1);
	//carry flag not affected
	setClockPrevious(8);
}

void CPU::opcodeBit(const int position, uint16_t address) {
	uint8_t val = mm.readAddress(address);
	bool test = (val >> position) & 0x1;

	F.setZeroFlag(test == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(1);
	//carry flag not affected*/
	setClockPrevious(12);
}

void CPU::opcodeReset(const int position, SingleRegister& r) {
	r.setBit(position, 0);
	setClockPrevious(8);
}

void CPU::opcodeReset(const int position, uint16_t address) {
	uint8_t val = mm.readAddress(address);
	val = (val & ~(1UL << position)) | (0 << position);
	mm.writeAddress(address, val);
	setClockPrevious(16);
}

void CPU::opcodeSet(const int position, SingleRegister& r) {
	r.setBit(position, 1);
	setClockPrevious(8);
}

void CPU::opcodeSet(const int position, uint16_t address) {
	uint8_t val = mm.readAddress(address);
	val = (val & ~(1UL << position)) | (1 << position);
	mm.writeAddress(address, val);
	setClockPrevious(16);
}