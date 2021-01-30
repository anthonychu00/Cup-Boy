#include <cstdint>
#include <cstdlib>
#include "CPU.h"

//rotates val to the left with bit 7 being moved to bit 0 and also stored into the carry
uint8_t CPU::opcodeRLCAux(uint8_t val) {
	F.setCarryFlag((val >> 7) & 0x1);//sets carry flag bit to the 7th bit of val
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
	//uint8_t toSet = opcodeRLCAux(read byte from 16 bit address);
	//set 16 bit address's value to toSet
	setClockPrevious(16);
}

//Rotates val register to the left with the carry's value put into bit 0 and bit 7 is put into the carry.
uint8_t CPU::opcodeRLAux(uint8_t val) {

}

void CPU::opcodeRLA() {
	uint8_t temp = AF.getHighRegister().getByte();
	bool carryValue = F.getCarryFlag();//gets carry flag value for later

	F.setCarryFlag((temp >> 7) & 0x1);//sets carry flag to bit 7 of A's value
	temp = ((temp << 1) | (temp >> 7));//rotate A left
	temp = (temp & ~(1UL << 0)) | (carryValue << 0);//set bit 0 of A to carry value
	AF.getHighRegister().set(temp);//update A value in register

	F.setZeroFlag(temp == 0);//********might have to override == operator here? 
	F.setHalfCarryFlag(0);
	F.setAddSubFlag(0);

	setClockPrevious(4);
}

//Rotates A to the right with bit 0 moved to bit 7 and also stored into the carry.
void CPU::opcodeRRCA() {
	uint8_t temp = AF.getHighRegister().getByte();//gets value of A register's byte
	F.setCarryFlag((temp >> 0) & 0x1);
	temp = ((temp >> 1) | (temp << 7));
	AF.getHighRegister().set(temp);

	F.setZeroFlag(temp == 0);//********might have to override == operator here? 
	F.setHalfCarryFlag(0);
	F.setAddSubFlag(0);

	setClockPrevious(4);
}

//Rotates A to the right with the carry put into bit 7 and bit 0 put into the carry flag.
void CPU::opcodeRRA() {
	uint8_t temp = AF.getHighRegister().getByte();
	bool carryValue = F.getCarryFlag();

	F.setCarryFlag((temp >> 0) & 0x1);
	temp = ((temp >> 1) | (temp << 7));
	temp = (temp & ~(1UL << 7)) | (carryValue << 7);//set bit 7 of A to carry value
	AF.getHighRegister().set(temp);

	F.setZeroFlag(temp == 0);//********might have to override == operator here? 
	F.setHalfCarryFlag(0);
	F.setAddSubFlag(0);

	setClockPrevious(4);
}