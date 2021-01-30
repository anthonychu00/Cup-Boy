#pragma once
#include <cstdint>
#include "../registers/split_register.h"
#include "../registers/full_register.h"



class CPU {
public:
	
	CPU() = default;
	void executeOperations();
	int executeOpcode(uint8_t opcode, uint16_t PCValue);
	int executePrefixedOpcode(uint8_t opcode, uint16_t PCValue);
	void setClockPrevious(int ticks);
	
	enum class Condition {
		NZ, Z, NC, C
	};//conditions for flag register in jump operations

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

	bool halt = false;
	bool interrupts = false;

	uint8_t PCFetchByte();
	uint16_t PCFetchWord();
	void stackPop(Word& r);
	void stackPush(const Word& r);
	bool checkCondition(Condition c);

	//opcodes
	void opcodeNOP();//0x00

	//load bytes
	void opcodeLoadByte(SingleRegister& r);//get byte from PC
	void opcodeLoadByte(SingleRegister& r, const SingleRegister& newReg);//get byte from other register
	void opcodeLoadByte(SingleRegister& r, uint16_t address); //address is HL's word value
	void opcodeLoadByte(uint16_t address, SingleRegister& r);

	//Load words
	void opcodeLoadWord(Word& r);
	//void opcodeLoadWord(Address, SP);

	void opcodeLoadHLSP();
	void opcodeLoadSPHL();

	//load accumulator to memory
	void opcodeLoadAToMemory16();
	void opcodeLoadAToMemory();
	void opcodeLoadAToMemory(SingleRegister& r);
	void opcodeLoadAToMemory(SplitRegister& r);//0x02, 0x12, 0x22, 0x32
	
	void opcodeLoadMemoryToA16();
	void opcodeLoadMemoryToA();
	void opcodeLoadMemoryToA(SingleRegister& r);
	void opcodeLoadMemoryToA(SplitRegister& r);

	void opcodeIncrement(Word& r);
	void opcodeIncrement(SingleRegister& r);
	//void opcodeIncrement(Address value);

	void opcodeDecrement(Word& r);
	void opcodeDecrement(SingleRegister& r);
	//void opcodeDecrement(Address value);

	uint8_t opcodeRLCAux(uint8_t val);
	void opcodeRLCA();
	void opcodeRLC(SingleRegister& r);
	void opcodeRLC(uint16_t address);

	uint8_t opcodeRLAux(uint8_t val);
	void opcodeRLA();
	void opcodeRL(SingleRegister& r);
	void opcodeRL(uint16_t address);

	void opcodeRRCA();
	void opcodeRRA();

	void opcodeDAA();
	void opcodeSCF();

	void opcodeJP();
	void opcodeJP(Condition c);
	void opcodeJPHL();

	void opcodeJR();
	void opcodeJR(Condition c);

	void opcodeAddSP();

	void opcodeAddAAux(uint8_t addedVal);
	void opcodeAddA();//get value from program counter
	void opcodeAddA(SingleRegister& r);
	void opcodeAddA(uint16_t address);

	void opcodeADCAux(uint8_t addedVal);
	void opcodeADC();
	void opcodeADC(SingleRegister& r);
	void opcodeADC(uint16_t address);

	void opcodeAddHLAux(uint16_t addedVal);
	void opcodeAddHL(SplitRegister& addedVal);
	void opcodeAddHL(FullRegister& addedVal);

	void opcodeSubAAux(uint8_t subbedVal);
	void opcodeSubA();
	void opcodeSubA(SingleRegister& r);
	void opcodeSubA(uint16_t address);

	void opcodeSBCAux(uint8_t subbedVal);
	void opcodeSBC();
	void opcodeSBC(SingleRegister& r);
	void opcodeSBC(uint16_t address);

	void opcodeAndAux(uint8_t val);
	void opcodeAnd();
	void opcodeAnd(SingleRegister& r);
	void opcodeAnd(uint16_t address);

	void opcodeXORAux(uint8_t val);
	void opcodeXOR();
	void opcodeXOR(SingleRegister& r);
	void opcodeXOR(uint16_t address);

	void opcodeOrAux(uint8_t val);
	void opcodeOr();
	void opcodeOr(SingleRegister& r);
	void opcodeOr(uint16_t address);

	void opcodeCPAux(uint8_t val);
	void opcodeCP();
	void opcodeCP(SingleRegister& r);
	void opcodeCP(uint16_t address);

	void opcodeCPL();
	void opcodeCCF();

	void opcodeHalt();

	void opcodeRet();
	void opcodeRet(Condition c);
	void opcodeRetI();

	void opcodeCall();
	void opcodeCall(Condition c);

	void opcodeRST(uint8_t location);//recheck

	void opcodeEI();
	void opcodeDI();

	void opcodePrefix();
};