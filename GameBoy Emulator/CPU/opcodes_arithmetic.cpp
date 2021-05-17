#include <cstdint>
#include <cstdlib>
#include "CPU.h"

//increment registers
int CPU::opcodeIncrement(Word& r) {
	r.increment();
	return 8;
}

int CPU::opcodeIncrement(SingleRegister& r) {
	r.increment();

	F.setZeroFlag(r.getByte() == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((r.getByte() & 0x0F) == 0x00);//checks if bit 3 overflows (second hex is F)

	return 4;
}

int CPU::opcodeIncrement(uint16_t address) {
	uint8_t data = mm.readAddress(address);
	uint8_t toWrite = data + 1;
	
	F.setZeroFlag(toWrite == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((toWrite & 0x0F) == 0x00);//checks if bit 3 overflows (second hex is F)

	mm.writeAddress(address, toWrite);
	return 12;
}

//decrement registers
int CPU::opcodeDecrement(Word& r) {
	r.decrement();
	return 8;
}

int CPU::opcodeDecrement(SingleRegister& r) {
	r.decrement();

	F.setZeroFlag(r.getByte() == 0);
	F.setAddSubFlag(1);
	F.setHalfCarryFlag((r.getByte() & 0x0F) == 0x0F);//checks if bit 3 overflows (second hex is F)

	return 4;
}

int CPU::opcodeDecrement(uint16_t address) {
	uint8_t data = mm.readAddress(address);
	//printf("Data: %d\n ", data);
	uint8_t toWrite = data - 1;
	//printf("After: %d\n ", toWrite);

	F.setZeroFlag(toWrite == 0);
	F.setAddSubFlag(1);
	F.setHalfCarryFlag((toWrite & 0x0F) == 0x0F);//checks if bit 3 overflows (second hex is F)

	mm.writeAddress(address, toWrite);

	return 12;
}

//Decimal adjust register A
int CPU::opcodeDAA() {
	uint8_t a = AF.getHigh();
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

	AF.setHigh(static_cast<uint8_t>(a));//set A to adjusted value
	return 4;
	
}

//set carry flag and reset other flags
int CPU::opcodeSCF() {
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);
	F.setCarryFlag(1);

	return 4;
}

int CPU::opcodeAddSP() {//0xE8
	int8_t addedVal = PCFetchSignedByte();
	//SP value type is uint16_t
	F.setZeroFlag(0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((SP.getValue() & 0x0F) + (addedVal & 0x0F) > 0x0F);
	F.setCarryFlag((SP.getValue() & 0xFF) + (addedVal & 0xFF) > 0xFF);

	SP.set(SP.getValue() + static_cast<uint16_t>(addedVal));

	return 16;
}

//add
void CPU::opcodeAddAAux(uint8_t addedVal) {
	uint8_t a = AF.getHigh();
	uint16_t toSet = static_cast<uint16_t>(a + addedVal);//uint16_t in case the bytes added together >255

	AF.setHigh(static_cast<uint8_t>(toSet));//recast to uint8_t for truncation

	F.setZeroFlag(AF.getHigh() == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((a & 0xF) + (addedVal & 0xF) > 0xF);//check first 4 bits only
	F.setCarryFlag(toSet > 0xFF);// added value > 255
}

int CPU::opcodeAddA() {//getting value from program counter
	opcodeAddAAux(PCFetchByte());
	return 8;
}

int CPU::opcodeAddA(const SingleRegister& r) {
	opcodeAddAAux(r.getByte());
	return 4;
}

int CPU::opcodeAddA(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeAddAAux(byte);
	return 8;
}

void CPU::opcodeADCAux(uint8_t addedVal) {
	uint8_t a = AF.getHigh();
	uint8_t c = static_cast<uint8_t>(F.getCarryFlag());//********casting bool to uint8_t?
	uint16_t toSet = static_cast<uint16_t>(a + addedVal + c);

	AF.setHigh(static_cast<uint8_t>(toSet));//recasting to uint8 to truncate 9th bit

	F.setZeroFlag(AF.getHigh() == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((a & 0xF) + (addedVal & 0xF) + c > 0xF);//check first 4 bits
	F.setCarryFlag(toSet > 0xFF);
}

int CPU::opcodeADC() {//get from PC
	opcodeADCAux(PCFetchByte());
	return 8;
}

int CPU::opcodeADC(const SingleRegister& r) {
	opcodeADCAux(r.getByte());
	return 4;
}

int CPU::opcodeADC(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeADCAux(byte);
	return 8;
}

int CPU::opcodeAddHL(uint16_t addedVal) {
	uint16_t HLVal = HL.getValue();
	uint32_t toSet = static_cast<uint32_t>(HLVal + addedVal);
	HL.set(static_cast<uint16_t>(toSet));

	F.setAddSubFlag(0);
	F.setHalfCarryFlag((HLVal & 0xFFF) + (addedVal & 0xFFF) > 0x0FFF);//set if carried from bit 11
	F.setCarryFlag(toSet > 0xFFFF);//set if carried from bit 15

	return 8;
}

//sub
void CPU::opcodeSubAAux(uint8_t subbedVal) {
	uint8_t a = AF.getHigh();
	uint8_t toSet = static_cast<uint8_t>(a - subbedVal);//operations between 2 uint8_t result in an int, have to recast

	AF.setHigh(toSet);

	F.setZeroFlag(AF.getHigh() == 0);
	F.setAddSubFlag(1);
	F.setHalfCarryFlag((a & 0xF) - (subbedVal & 0xF) < 0);//check first 4 bits, set if borrow from bit 4
	F.setCarryFlag(a < subbedVal);//negative value, borrowing from
}

int CPU::opcodeSubA() {
	opcodeSubAAux(PCFetchByte());
	return 8;
}

int CPU::opcodeSubA(const SingleRegister& r) {
	opcodeSubAAux(r.getByte());
	return 4;
}

int CPU::opcodeSubA(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeSubAAux(byte);
	return 8;
}

void CPU::opcodeSBCAux(uint8_t subbedVal) {
	uint8_t a = AF.getHigh();
	uint8_t c = static_cast<uint8_t>(F.getCarryFlag());

	uint8_t toSet = static_cast<uint8_t>(a - subbedVal - c);

	AF.setHigh(toSet);

	F.setZeroFlag(toSet == 0);
	F.setAddSubFlag(1);
	F.setHalfCarryFlag((a & 0xF) - (subbedVal & 0xF) - c < 0);
	F.setCarryFlag(a < subbedVal + c);
	//F.setCarryFlag(toSet < 0);
}

int CPU::opcodeSBC() {
	opcodeSBCAux(PCFetchByte());
	return 8;
}

int CPU::opcodeSBC(const SingleRegister& r) {
	opcodeSBCAux(r.getByte());
	return 4;
}

int CPU::opcodeSBC(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeSBCAux(byte);
	return 8;
}

//and
void CPU::opcodeAndAux(uint8_t val) {
	uint8_t a = AF.getHigh();
	uint8_t result = a & val;

	AF.setHigh(result);

	F.setZeroFlag(result == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(1);
	F.setCarryFlag(0);
}

int CPU::opcodeAnd() {
	opcodeAndAux(PCFetchByte());
	return 8;
}

int CPU::opcodeAnd(const SingleRegister& r) {
	opcodeAndAux(r.getByte());
	return 4;
}

int CPU::opcodeAnd(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeAndAux(byte);
	return 8;
}

void CPU::opcodeXORAux(uint8_t val) {
	uint8_t a = AF.getHigh();
	//printf("a: %d\n", a);
	uint8_t result = a ^ val;
	AF.setHigh(result);

	F.setZeroFlag(result == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);
	F.setCarryFlag(0);
}

int CPU::opcodeXOR() {
	opcodeXORAux(PCFetchByte());
	return 8;
}

int CPU::opcodeXOR(const SingleRegister& r) {
	opcodeXORAux(r.getByte());
	return 4;
}

int CPU::opcodeXOR(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeXORAux(byte);
	return 8;
}

void CPU::opcodeOrAux(uint8_t val) {
	uint8_t a = AF.getHigh();
	uint8_t result = a | val;

	AF.setHigh(result);

	F.setZeroFlag(result == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);
	F.setCarryFlag(0);
}

int CPU::opcodeOr() {
	opcodeOrAux(PCFetchByte());
	return 8;
}

int CPU::opcodeOr(const SingleRegister& r) {
	opcodeOrAux(r.getByte());
	return 4;
}

int CPU::opcodeOr(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeOrAux(byte);
	return 8;
}

void CPU::opcodeCPAux(uint8_t val) {
	uint8_t a = AF.getHigh();

	F.setZeroFlag(a == val);
	F.setAddSubFlag(1);
	F.setHalfCarryFlag((a & 0xF) - (val & 0xF) < 0);
	F.setCarryFlag(a < val);//set if borrow
}

int CPU::opcodeCP() {
	opcodeCPAux(PCFetchByte());
	return 8;
}

int CPU::opcodeCP(const SingleRegister& r) {
	opcodeCPAux(r.getByte());
	return 4;
}

int CPU::opcodeCP(const uint16_t address) {
	uint8_t byte = mm.readAddress(address);
	opcodeCPAux(byte);
	return 8;
}

//complement A (Flip its bits)
int CPU::opcodeCPL() {
	uint8_t temp = AF.getHigh();
	temp = ~temp;//~ negates the bits and flips them
	AF.setHigh(temp);

	F.setAddSubFlag(1);
	F.setHalfCarryFlag(1);
	return 4;
}

//complement carry flag
int CPU::opcodeCCF() {
	F.setCarryFlag(!F.getCarryFlag());
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);
	return 4;
}