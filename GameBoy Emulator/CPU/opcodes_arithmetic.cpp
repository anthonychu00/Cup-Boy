#include <cstdint>
#include <cstdlib>
#include "CPU.h"

//increment registers
void CPU::opcodeIncrement(Word& r) {
	r.increment();
	setClockPrevious(8);
}

void CPU::opcodeIncrement(SingleRegister& r) {
	r.increment();

	F.setZeroFlag(r.getByte() == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((r.getByte() & 0x0F) == 0x00);//checks if bit 3 overflows (second hex is F)

	setClockPrevious(4);
}

void CPU::opcodeIncrement(uint16_t address) {
	uint8_t data = mm.readAddress(address);
	uint8_t toWrite = data++;
	
	F.setZeroFlag(toWrite == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((toWrite & 0x0F) == 0x00);//checks if bit 3 overflows (second hex is F)

	mm.writeAddress(address, toWrite);
	setClockPrevious(12);
}

//decrement registers
void CPU::opcodeDecrement(Word& r) {
	r.decrement();
	setClockPrevious(8);
}

void CPU::opcodeDecrement(SingleRegister& r) {
	r.decrement();

	F.setZeroFlag(r.getByte() == 0);
	F.setAddSubFlag(1);
	F.setHalfCarryFlag((r.getByte() & 0x0F) == 0x0F);//checks if bit 3 overflows (second hex is F)

	setClockPrevious(4);
}

void CPU::opcodeDecrement(uint16_t address) {
	uint8_t data = mm.readAddress(address);
	uint8_t toWrite = data--;

	F.setZeroFlag(toWrite == 0);
	F.setAddSubFlag(1);
	F.setHalfCarryFlag((toWrite & 0x0F) == 0x0F);//checks if bit 3 overflows (second hex is F)

	mm.writeAddress(address, toWrite);

	setClockPrevious(12);
}

//Decimal adjust register A
void CPU::opcodeDAA() {
	uint8_t a = AF.getHighRegister().getByte();
	if (!F.getAddSubFlag()) { //addition was performed (AddSubFlag = 0)
		if (F.getCarryFlag() || a > 0x99) {
			a += 0x60;
			F.setCarryFlag(1);//used if a > 0x99 is true, but F.getCarryFlag() = 0;
		}
		if (F.getHalfCarryFlag() || (a & 0x0F) > 0x09) {
			a += 0x6;
		}
	}
	else { //subtraction was performed (AddSubFlag = 1)
		if (F.getCarryFlag()) {a -= 0x60; }
		if (F.getHalfCarryFlag()) { a -= 0x6; }
	}

	F.setZeroFlag(a == 0);//see if A is 0
	F.setHalfCarryFlag(0);//we reset half carry flag always

	AF.getHighRegister().set(static_cast<uint8_t>(a));//set A to adjusted value
	setClockPrevious(4);
	
}

//set carry flag and reset other flags
void CPU::opcodeSCF() {
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);
	F.setCarryFlag(1);

	setClockPrevious(4);
}

void CPU::opcodeAddSP() {//0xE8
	int8_t addedVal = PCFetchSignedByte();
	//SP value type is uint16_t
	F.setZeroFlag(0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((SP.getValue() & 0x0F) + (addedVal & 0x0F) > 0x0F);
	F.setCarryFlag((SP.getValue() & 0xFF) + (addedVal & 0xFF) > 0xFF);

	SP.set(SP.getValue() + static_cast<uint16_t>(addedVal));

	setClockPrevious(16);
}

//add
void CPU::opcodeAddAAux(uint8_t addedVal) {
	uint8_t a = AF.getHighRegister().getByte();
	uint16_t toSet = static_cast<uint16_t>(a + addedVal);//uint16_t in case the bytes added together >255

	AF.getHighRegister().set(static_cast<uint8_t>(toSet));//recast to uint8_t for truncation

	F.setZeroFlag(AF.getHighRegister().getByte() == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((a & 0xF) + (addedVal & 0xF) > 0xF);//check first 4 bits only
	F.setCarryFlag(toSet > 0xFF);// added value > 255
}

void CPU::opcodeAddA() {//getting value from program counter
	opcodeAddAAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeAddA(const SingleRegister& r) {
	opcodeAddAAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeAddA(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeAddAAux(byte);
	setClockPrevious(8);
}

void CPU::opcodeADCAux(uint8_t addedVal) {
	uint8_t a = AF.getHighRegister().getByte();
	uint8_t c = static_cast<uint8_t>(F.getCarryFlag());//********casting bool to uint8_t?

	uint16_t toSet = static_cast<uint16_t>(a + addedVal + c);

	AF.getHighRegister().set(static_cast<uint8_t>(toSet));//recasting to uint8 to truncate 9th bit

	F.setZeroFlag(AF.getHighRegister().getByte() == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((a & 0xF) + (addedVal & 0xF) + c > 0xF);//check first 4 bits
	F.setCarryFlag(toSet > 0xFF);
}

void CPU::opcodeADC() {//get from PC
	opcodeADCAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeADC(const SingleRegister& r) {
	opcodeADCAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeADC(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeADCAux(byte);
	setClockPrevious(8);
}

void CPU::opcodeAddHLAux(uint16_t addedVal) {
	uint16_t HLVal = HL.getValue();

	uint32_t toSet = static_cast<uint32_t>(HLVal + addedVal);
	HL.set(static_cast<uint16_t>(toSet));

	F.setAddSubFlag(0);
	F.setHalfCarryFlag((HLVal & 0xFFF) + (addedVal & 0xFFF) > 0x0FFF);//set if carried from bit 11
	F.setCarryFlag(toSet > 0xFFFF);//set if carried from bit 15

	setClockPrevious(8);
}

void CPU::opcodeAddHL(const SplitRegister& addedVal) {
	opcodeAddHLAux(addedVal.getValue());
}

void CPU::opcodeAddHL(const FullRegister& addedVal) {
	opcodeAddHLAux(addedVal.getValue());
}

//sub
void CPU::opcodeSubAAux(uint8_t subbedVal) {
	uint8_t a = AF.getHighRegister().getByte();
	uint8_t toSet = static_cast<uint8_t>(a - subbedVal);//operations between 2 uint8_t result in an int, have to recast

	AF.getHighRegister().set(toSet);

	F.setZeroFlag(AF.getHighRegister().getByte() == 0);
	F.setAddSubFlag(1);
	F.setHalfCarryFlag((a & 0xF) - (subbedVal & 0xF) < 0);//check first 4 bits, set if borrow from bit 4
	F.setCarryFlag(a < subbedVal);//negative value, borrowing from
}

void CPU::opcodeSubA() {
	opcodeSubAAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeSubA(const SingleRegister& r) {
	opcodeSubAAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeSubA(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeSubAAux(byte);
	setClockPrevious(8);
}

void CPU::opcodeSBCAux(uint8_t subbedVal) {
	uint8_t a = AF.getHighRegister().getByte();
	uint8_t c = static_cast<uint8_t>(F.getCarryFlag());

	uint8_t toSet = static_cast<uint8_t>(a - subbedVal - c);

	AF.getHighRegister().set(toSet);

	F.setZeroFlag(toSet == 0);
	F.setAddSubFlag(1);
	F.setHalfCarryFlag((a & 0xF) - (subbedVal & 0xF) - c < 0);
	F.setCarryFlag(a < subbedVal + c);
	//F.setCarryFlag(toSet < 0);
}

void CPU::opcodeSBC() {
	opcodeSBCAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeSBC(const SingleRegister& r) {
	opcodeSBCAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeSBC(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeSBCAux(byte);
	setClockPrevious(8);
}

//and
void CPU::opcodeAndAux(uint8_t val) {
	uint8_t a = AF.getHighRegister().getByte();
	uint8_t result = a & val;

	AF.getHighRegister().set(result);

	F.setZeroFlag(result == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(1);
	F.setCarryFlag(0);
}

void CPU::opcodeAnd() {
	opcodeAndAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeAnd(const SingleRegister& r) {
	opcodeAndAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeAnd(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeAndAux(byte);
	setClockPrevious(8);
}

void CPU::opcodeXORAux(uint8_t val) {
	uint8_t a = AF.getHighRegister().getByte();
	uint8_t result = a ^ val;

	AF.getHighRegister().set(result);

	F.setZeroFlag(result == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);
	F.setCarryFlag(0);
}

void CPU::opcodeXOR() {
	opcodeXORAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeXOR(const SingleRegister& r) {
	opcodeXORAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeXOR(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeXORAux(byte);
	setClockPrevious(8);
}

void CPU::opcodeOrAux(uint8_t val) {
	uint8_t a = AF.getHighRegister().getByte();
	uint8_t result = a | val;

	AF.getHighRegister().set(result);

	F.setZeroFlag(result == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);
	F.setCarryFlag(0);
}

void CPU::opcodeOr() {
	opcodeOrAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeOr(const SingleRegister& r) {
	opcodeOrAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeOr(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeOrAux(byte);
	setClockPrevious(8);
}

void CPU::opcodeCPAux(uint8_t val) {
	uint8_t a = AF.getHighRegister().getByte();

	F.setZeroFlag(a == val);
	F.setAddSubFlag(1);
	F.setHalfCarryFlag((a & 0xF) - (val & 0xF) < 0);
	F.setCarryFlag(a < val);//set if borrow
}

void CPU::opcodeCP() {
	opcodeCPAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeCP(const SingleRegister& r) {
	opcodeCPAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeCP(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeCPAux(byte);
	setClockPrevious(8);
}

//complement A (Flip its bits)
void CPU::opcodeCPL() {
	uint8_t temp = AF.getHighRegister().getByte();
	temp = ~temp;//~ negates the bits and flips them
	AF.getHighRegister().set(temp);

	F.setAddSubFlag(1);
	F.setHalfCarryFlag(1);
	setClockPrevious(4);
}

//complement carry flag
void CPU::opcodeCCF() {
	F.setCarryFlag(!F.getCarryFlag());
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);
	setClockPrevious(4);
}