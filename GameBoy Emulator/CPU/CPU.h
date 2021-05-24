#pragma once
#include <cstdint>
#include "../registers/split_register.h"
#include "../registers/full_register.h"
#include "../memory/memory_map.h"
#include "../utils/utils.h"

class CPU {
public:
	
	CPU(MemoryMap& newMM);//registers default to 0x00 value
	~CPU() = default;

	int executeOperations();
	int executeOpcode(const uint8_t opcode, uint16_t PCValue);
	int executePrefixedOpcode(const uint8_t opcode, uint16_t PCValue);
	string traceOpcode(const uint8_t opcode);
	string tracePrefixedOpcode(const uint8_t opcode);
	void setClockPrevious(int ticks);
	void setClockSpeed(int newSpeed);
	int getClockSpeed();
	int getDiv();
	void resetDiv();
	void updateTimer(int previousTicks);
	void checkInterruptRequests();
	void setInterruptFlag(int position);
	
	enum class Flag {
		NZ, Z, NC, C
	};//conditions for flag register in jump operations

private:
	SingleRegister A, B, C, D, E, H, L; //accumulator
	FlagRegister F;

	//wrapper classese to treat our single registers as pairs or 16 bit ones
	//SplitRegisters can be used as a 16 bit address or 2 separate 8 bit ones
	SplitRegister AF;//A = high, F = low
	SplitRegister BC;
	SplitRegister DE;
	SplitRegister HL;

	//FullRegisters can only be used as a 16 bit address
	FullRegister SP;
	FullRegister PC;

	bool halt = false;
	bool executeTwice = false;
	bool IME = false;//interrupt master enable flag
	const uint16_t IERegister = 0xFFFF;//needs to be enabled with IME
	const uint16_t IFRegister = 0xFF0F;

	//timer register locations
	int divTime = 0;// "ticks" of the divider register
	int tima;
	int clockSpeed;
	const uint16_t dividerRegister = 0xFF04;
	const uint16_t timerCounter = 0xFF05;
	const uint16_t timerModulo = 0xFF06;
	const uint16_t timerControl = 0xFF07;

	MemoryMap& mm;

	struct Clock {
		//figure out best representation of time units for CPU
		int p;	//time to execute previous operation (need to time how long each operation takes)
		int t;  //total time CPU has run
	};

	Clock clock = { 0, 0 };

	uint8_t PCFetchByte();
	int8_t PCFetchSignedByte();
	uint16_t PCFetchWord();

	void interruptExecute(int vectorPosition);
	
	

	bool checkCondition(Flag fl);


	//opcodes
	int opcodeNOP();//0x00

	//stack operations
	int opcodeStackPop(Word& r, bool flagInvolved = false);
	int opcodeStackPush(const Word& r);

	//load bytes
	int opcodeLoadByte(SingleRegister& r);//get byte from PC
	int opcodeLoadByte(SingleRegister& r, const SingleRegister& newReg);//get byte from other register
	int opcodeLoadByte(const uint16_t address);
	int opcodeLoadByte(SingleRegister& r, const uint16_t address); //address is HL's word value
	int opcodeLoadByte(const uint16_t address, const SingleRegister& r);
	
	//Load words
	int opcodeLoadSPToMemory();
	int opcodeLoadWord(Word& r);

	int opcodeLoadHLSP();
	int opcodeLoadSPHL();

	//load accumulator to memory
	int opcodeLoadAToMemory16();
	int opcodeLoadAToMemory();
	int opcodeLoadAToMemory(const SingleRegister& r);
	int opcodeLoadAToMemory(const uint16_t address);//0x02, 0x12, 0x22, 0x32
	int opcodeLoadAToMemoryInc(const uint16_t address);//for HL
	int opcodeLoadAToMemoryDec(const uint16_t address);
	
	int opcodeLoadMemoryToA16();
	int opcodeLoadMemoryToA();
	int opcodeLoadMemoryToA(const SingleRegister& r);
	int opcodeLoadMemoryToA(const uint16_t address);
	int opcodeLoadMemoryToAInc(const uint16_t address);
	int opcodeLoadMemoryToADec(const uint16_t address);

	int opcodeIncrement(Word& r);
	int opcodeIncrement(SingleRegister& r);
	int opcodeIncrement(uint16_t address);

	int opcodeDecrement(Word& r);
	int opcodeDecrement(SingleRegister& r);
	int opcodeDecrement(uint16_t address);

	uint8_t opcodeRLCAux(uint8_t val);
	int opcodeRLCA();
	int opcodeRLC(SingleRegister& r);
	int opcodeRLC(uint16_t address);

	uint8_t opcodeRLAux(uint8_t val);
	int opcodeRLA();
	int opcodeRL(SingleRegister& r);
	int opcodeRL(uint16_t address);

	uint8_t opcodeRRCAux(uint8_t val);
	int opcodeRRCA();
	int opcodeRRC(SingleRegister& r);
	int opcodeRRC(uint16_t address);

	uint8_t opcodeRRAux(uint8_t val);
	int opcodeRRA();
	int opcodeRR(SingleRegister& r);
	int opcodeRR(uint16_t address);

	uint8_t opcodeSLAAux(uint8_t val);
	int opcodeSLA(SingleRegister& r);
	int opcodeSLA(uint16_t address);

	uint8_t opcodeSRAAux(uint8_t val);
	int opcodeSRA(SingleRegister& r);
	int opcodeSRA(uint16_t address);

	uint8_t opcodeSRLAux(uint8_t val);
	int opcodeSRL(SingleRegister& r);
	int opcodeSRL(uint16_t address);

	uint8_t opcodeSwapAux(uint8_t val);
	int opcodeSwap(SingleRegister& r);
	int opcodeSwap(uint16_t address);

	int opcodeBit(const int position, SingleRegister& r);
	int opcodeBit(const int position, uint16_t address);

	int opcodeReset(const int position, SingleRegister& r);
	int opcodeReset(const int position, uint16_t address);

	int opcodeSet(const int position, SingleRegister& r);
	int opcodeSet(const int position, uint16_t address);

	int opcodeDAA();
	int opcodeSCF();

	int opcodeJP();
	int opcodeJPHL();
	int opcodeJP(Flag fl);
	

	int opcodeJR();
	int opcodeJR(Flag fl);

	int opcodeAddSP();

	void opcodeAddAAux(uint8_t addedVal);
	int opcodeAddA();//get value from program counter
	int opcodeAddA(const SingleRegister& r);
	int opcodeAddA(const uint16_t address);

	int opcodeAddHL(uint16_t addedVal);

	void opcodeADCAux(uint8_t addedVal);
	int opcodeADC();
	int opcodeADC(const SingleRegister& r);
	int opcodeADC(const uint16_t address);

	void opcodeSubAAux(uint8_t subbedVal);
	int opcodeSubA();
	int opcodeSubA(const SingleRegister& r);
	int opcodeSubA(const uint16_t address);

	void opcodeSBCAux(uint8_t subbedVal);
	int opcodeSBC();
	int opcodeSBC(const SingleRegister& r);
	int opcodeSBC(const uint16_t address);

	void opcodeAndAux(uint8_t val);
	int opcodeAnd();
	int opcodeAnd(const SingleRegister& r);
	int opcodeAnd(const uint16_t address);

	void opcodeXORAux(uint8_t val);
	int opcodeXOR();
	int opcodeXOR(const SingleRegister& r);
	int opcodeXOR(const uint16_t address);

	void opcodeOrAux(uint8_t val);
	int opcodeOr();
	int opcodeOr(const SingleRegister& r);
	int opcodeOr(const uint16_t address);

	void opcodeCPAux(uint8_t val);
	int opcodeCP();
	int opcodeCP(const SingleRegister& r);
	int opcodeCP(const uint16_t address);

	int opcodeCPL();
	int opcodeCCF();

	int opcodeHalt();

	int opcodeRet();
	int opcodeRet(Flag fl);
	int opcodeRetI();

	int opcodeCall();
	int opcodeCall(Flag fl);

	int opcodeRST(uint8_t location);//recheck

	int opcodeEI();
	int opcodeDI();

	int opcodeStop();
};