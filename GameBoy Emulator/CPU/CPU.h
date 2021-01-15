#pragma once
#include <cstdint>
#include "../registers/split_register.h"
#include "../registers/full_register.h"

enum class Condition {
	NZ, Z, NC, C
};//conditions for flag register in jump operations

class CPU {
public:
	
	CPU() = default;
	void executeOperations();
	int executeOpcode(uint8_t opcode, uint16_t PCValue);
	void setClockPrevious(int ticks);
	
	//fetch an op like uint8_t op = fetchOp(PC);
	//function to decode and execute ( lots of switch cases probably)

	//execution can either be between its own registers or performing an operation on the address bus

	// the CPU will use an address bus of 16 bits to communicate with memory outside its registers, it will only be able to read and write to these
	// this is basically how the CPU will affect the state of the actual emulator
private:

	struct Clock {
		//figure out best representation of time units for CPU
		int p;	//time to execute previous operation (need to time how long each operation takes)
		int t;  //total time CPU has run
	};

	//SplitRegisters can be used as a 16 bit address or 2 separate 8 bit ones
	SplitRegister AF;//A = high, F = low
	SplitRegister BC;
	SplitRegister DE;
	SplitRegister HL;

	//FullRegisters can only be used as a 16 bit address
	FullRegister SP;
	FullRegister PC;

	//separate FlagRegister object for ease of access
	FlagRegister F;

	Clock clock;

	uint8_t PCFetchByte();
	uint16_t PCFetchWord();
	bool checkCondition(Condition c);

	//opcodes
	void opcodeNOP();//0x00

	//load bytes
	void opcodeLoadByte(SingleRegister& r);

	//Load words
	void opcodeLoadWord(SplitRegister& r);//0x01, 0x11, 0x21
	void opcodeLoadWord(FullRegister& r);//0x31
	//void opcodeLoadWord(Address, SP);

	//load accumulator to memory
	void opcodeLoadAToMemory(SplitRegister& r);//0x02, 0x12, 0x22, 0x32
	void opcodeLoadMemoryToA(SplitRegister& r);

	void opcodeIncrement(SplitRegister& r);//0x03, 0x13, 0x23, 0x33
	void opcodeIncrement(FullRegister& r);
	void opcodeIncrement(SingleRegister& r);
	//void opcodeIncrement(Address value);

	void opcodeDecrement(SplitRegister& r);
	void opcodeDecrement(FullRegister& r);
	void opcodeDecrement(SingleRegister& r);
	//void opcodeDecrement(Address value);

	void opcodeRLCA();
	void opcodeRLA();
	void opcodeRRCA();
	void opcodeRRA();

	void opcodeDAA();
	void opcodeSCF();

	void opcodeJR();
	void opcodeJR(Condition c);

	void opcodeAddHL(SplitRegister& addedVal);
	void opcodeAddHL(FullRegister& addedVal);
};