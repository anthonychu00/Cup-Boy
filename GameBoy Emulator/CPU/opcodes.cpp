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

//byte to address


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

/*void CPU::opcodeIncrement(Address value) {
*	//set flags
	//stuff for memory address case like 0x34
	//increment byte value at 2 byte address by 1
	//read memory address to get value, increment value, rewrite to that memory address
	setClockPrevious(12);
}*/

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

/*void CPU::opcodeDecrement(Address value) {
	//stuff for memory address case like 0x34
	//decrement byte value at 2 byte address by 1
	setClockPrevious(12);
}*/


void CPU::opcodeRLCAux(uint8_t val) {
	F.setCarryFlag((val >> 7) & 0x1);//sets carry flag bit to the 7th bit of A
	val = ((val << 1) | (val >> 7));//rotate A left 
	AF.getHighRegister().set(val);

	F.setZeroFlag(val == 0);//********might have to override == operator here? 
	F.setHalfCarryFlag(0);
	F.setAddSubFlag(0);
}

//Rotates A to the left with bit 7 being moved to bit 0 and also stored into the carry.
void CPU::opcodeRLCA() {
	uint8_t temp = AF.getHighRegister().getByte();//gets value of A register's byte
	F.setCarryFlag((temp >> 7) & 0x1);//sets carry flag bit to the 7th bit of A
	temp = ((temp << 1) | (temp >> 7));//rotate A left 
	AF.getHighRegister().set(temp);

	F.setZeroFlag(temp == 0);//********might have to override == operator here? 
	F.setHalfCarryFlag(0);
	F.setAddSubFlag(0);

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

//jumps
void CPU::opcodeJP() {
	uint16_t address = PCFetchWord();
	PC.set(address);
	setClockPrevious(16);
}

void CPU::opcodeJPHL() {
	PC.set(HL.getValue());//jump to address pointed by HL, but do not actually access it
	setClockPrevious(4);
}
void CPU::opcodeJP(Condition c) {
	bool result = checkCondition(c);
	if (result) {
		opcodeJP();
	}
	else {
		uint16_t word = PCFetchWord();
		setClockPrevious(12);
	}
}

void CPU::opcodeJR() {
	//get a signed byte from program counter (increment PC as well)
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


void CPU::opcodeAddSP(){//********check if it works closely
	int8_t addedVal = static_cast<int8_t>(PCFetchByte());
	//SP value is uint16_t
	F.setZeroFlag(0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag((SP.getValue() & 0x0F) + (addedVal & 0x0F) > 0x0F);
	F.setCarryFlag((SP.getValue() & 0xFF) + (addedVal & 0xFF) > 0xFF);

	SP.set(SP.getValue() + static_cast<uint16_t>(addedVal));//replace 0x00 with byte value later

	setClockPrevious(16);
}

//add
void CPU::opcodeAddAAux(uint8_t addedVal) {
	uint8_t a = AF.getHighRegister().getByte();
	uint16_t toSet = static_cast<uint16_t>(a + addedVal);//uint16_t in case the bytes added together >255

	AF.getHighRegister().set(static_cast<uint8_t>(toSet));//recast to uint8_t for truncation

	F.setZeroFlag(AF.getHighRegister().getByte() == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag( (a & 0xF) + (addedVal & 0xF) > 0xF);//check first 4 bits only
	F.setCarryFlag(toSet > 0xFF);// added value > 255
}

void CPU::opcodeAddA() {//getting value from program counter
	opcodeAddAAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeAddA(SingleRegister& r) {
	opcodeAddAAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeAddA(uint16_t address) {
	//get byte from 16-bit address in memory
	setClockPrevious(8);
}

void CPU::opcodeADCAux(uint8_t addedVal) {
	uint8_t a = AF.getHighRegister().getByte();
	uint8_t c = static_cast<uint8_t>(F.getCarryFlag());//********casting bool to uint8_t?

	uint16_t toSet = static_cast<uint16_t>(a + addedVal + c);

	AF.getHighRegister().set(static_cast<uint8_t>(toSet));//recasting to uint8 to truncate 9th bit

	F.setZeroFlag(AF.getHighRegister().getByte() == 0);
	F.setAddSubFlag(0);
	F.setHalfCarryFlag( (a & 0xF) + (addedVal & 0xF) + c > 0xF);//check first 4 bits
	F.setCarryFlag(toSet > 0xFF);
}

void CPU::opcodeADC() {//get from PC
	opcodeADCAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeADC(SingleRegister& r) {
	opcodeADCAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeADC(uint16_t address) {
	//get byte from 16 bit memory address
	setClockPrevious(8);
}

void CPU::opcodeAddHLAux(uint16_t addedVal) { 
	uint16_t HLVal = HL.getValue();

	uint32_t toSet = static_cast<uint32_t>(HLVal + addedVal);
	HL.set(static_cast<uint16_t>(toSet));

	F.setAddSubFlag(0);
	F.setHalfCarryFlag((HLVal & 0xFFF) + (addedVal & 0xFFF) > 0xFFF);//set if carried from bit 11
	F.setCarryFlag(toSet > 0xFFFF);//set if carried from bit 15

	setClockPrevious(8);
}

void CPU::opcodeAddHL(SplitRegister& addedVal) {
	opcodeAddHLAux(addedVal.getValue());
}

void CPU::opcodeAddHL(FullRegister& addedVal) {
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

void CPU::opcodeSubA(SingleRegister& r) {
	opcodeSubAAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeSubA(uint16_t address) {
	//get byte from 16 bit memory address (HL)
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
}

void CPU::opcodeSBC() {
	opcodeSBCAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeSBC(SingleRegister& r) {
	opcodeSBCAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeSBC(uint16_t address) {
	//get byte from 16 bit memory address (HL)
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

void CPU::opcodeAnd(SingleRegister& r) {
	opcodeAndAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeAnd(uint16_t address) {
	//get byte from 16 bit address
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

void CPU::opcodeXOR(SingleRegister& r) {
	opcodeXORAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeXOR(uint16_t address) {
	//get byte from 16 bit address
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
	opcodeXORAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeOr(SingleRegister& r) {
	opcodeXORAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeOr(uint16_t address) {
	//get byte from 16 bit address
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
	opcodeXORAux(PCFetchByte());
	setClockPrevious(8);
}

void CPU::opcodeCP(SingleRegister& r) {
	opcodeXORAux(r.getByte());
	setClockPrevious(4);
}

void CPU::opcodeCP(uint16_t address) {
	//get byte from 16 bit address
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

//complement ccarry flag
void CPU::opcodeCCF() {
	F.setCarryFlag(!F.getCarryFlag());
	F.setAddSubFlag(0);
	F.setHalfCarryFlag(0);
	setClockPrevious(4);
}

void CPU::opcodeHalt() {
	halt = true;
	setClockPrevious(4);
}

void CPU::opcodeRet() {
	stackPop(PC);
	setClockPrevious(16);
}

void CPU::opcodeRet(Condition c) {
	bool result = checkCondition(c);
	if (result) { //we do branch
		opcodeRet();
		setClockPrevious(20);
	}
	else {
		setClockPrevious(8);
	}
}

void CPU::opcodeRetI() {
	stackPop(PC);
	opcodeEI();
	setClockPrevious(16);
}

void CPU::opcodeCall() {
	uint16_t address = PCFetchWord();
	stackPush(PC);
	PC.set(address);
	setClockPrevious(24);
}

void CPU::opcodeCall(Condition c) {
	bool result = checkCondition(c);
	if (result) { //we do branch
		opcodeCall();
	}
	else {
		uint16_t word = PCFetchWord();
		setClockPrevious(12);
	}
}

void CPU::opcodeRST(uint8_t location) {
	stackPush(PC);
	uint16_t address = static_cast<uint16_t>(0x0000 + static_cast<uint16_t>(location));
	PC.set(address);
	setClockPrevious(16);
}

void CPU::opcodeEI() {
	interrupts = true;
	setClockPrevious(4);
}

void CPU::opcodeDI() {
	interrupts = false;
	setClockPrevious(4);
}

void CPU::opcodePrefix() {
	/// <summary>
	/// 
	/// </summary>
	setClockPrevious(4);
}

