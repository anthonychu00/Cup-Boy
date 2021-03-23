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

	void executeOperations();
	int executeOpcode(const uint8_t opcode, uint16_t PCValue);
	int executePrefixedOpcode(const uint8_t opcode, uint16_t PCValue);
	string traceOpcode(const uint8_t opcode);
	string tracePrefixedOpcode(const uint8_t opcode);
	void setClockPrevious(int ticks);
	void setClockSpeed(int newSpeed);
	int getClockSpeed();
	
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
	bool IME = false;//interrupt master enable flag
	const uint16_t IERegister = 0xFFFF;//needs to be enabled with IME
	const uint16_t IFRegister = 0xFF0F;

	//timer register locations
	int divTime;// "ticks" of the divider register
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

	void checkInterruptRequests();
	void interruptExecute(int vectorPosition);
	void setInterruptFlag(int position);

	void updateTimer();
	

	bool checkCondition(Flag fl);


	//opcodes
	void opcodeNOP();//0x00

	//stack operations
	void opcodeStackPop(Word& r, bool flagInvolved = false);
	void opcodeStackPush(const Word& r);

	//load bytes
	void opcodeLoadByte(SingleRegister& r);//get byte from PC
	void opcodeLoadByte(SingleRegister& r, const SingleRegister& newReg);//get byte from other register
	void opcodeLoadByte(const uint16_t address);
	void opcodeLoadByte(SingleRegister& r, const uint16_t address); //address is HL's word value
	void opcodeLoadByte(const uint16_t address, const SingleRegister& r);
	
	//Load words
	void opcodeLoadSPToMemory();
	void opcodeLoadWord(Word& r);

	void opcodeLoadHLSP();
	void opcodeLoadSPHL();

	//load accumulator to memory
	void opcodeLoadAToMemory16();
	void opcodeLoadAToMemory();
	void opcodeLoadAToMemory(const SingleRegister& r);
	void opcodeLoadAToMemory(const uint16_t address);//0x02, 0x12, 0x22, 0x32
	void opcodeLoadAToMemoryInc(const uint16_t address);//for HL
	void opcodeLoadAToMemoryDec(const uint16_t address);
	
	void opcodeLoadMemoryToA16();
	void opcodeLoadMemoryToA();
	void opcodeLoadMemoryToA(const SingleRegister& r);
	void opcodeLoadMemoryToA(const uint16_t address);
	void opcodeLoadMemoryToAInc(const uint16_t address);
	void opcodeLoadMemoryToADec(const uint16_t address);

	void opcodeIncrement(Word& r);
	void opcodeIncrement(SingleRegister& r);
	void opcodeIncrement(uint16_t address);

	void opcodeDecrement(Word& r);
	void opcodeDecrement(SingleRegister& r);
	void opcodeDecrement(uint16_t address);

	uint8_t opcodeRLCAux(uint8_t val);
	void opcodeRLCA();
	void opcodeRLC(SingleRegister& r);
	void opcodeRLC(uint16_t address);

	uint8_t opcodeRLAux(uint8_t val);
	void opcodeRLA();
	void opcodeRL(SingleRegister& r);
	void opcodeRL(uint16_t address);

	uint8_t opcodeRRCAux(uint8_t val);
	void opcodeRRCA();
	void opcodeRRC(SingleRegister& r);
	void opcodeRRC(uint16_t address);

	uint8_t opcodeRRAux(uint8_t val);
	void opcodeRRA();
	void opcodeRR(SingleRegister& r);
	void opcodeRR(uint16_t address);

	uint8_t opcodeSLAAux(uint8_t val);
	void opcodeSLA(SingleRegister& r);
	void opcodeSLA(uint16_t address);

	uint8_t opcodeSRAAux(uint8_t val);
	void opcodeSRA(SingleRegister& r);
	void opcodeSRA(uint16_t address);

	uint8_t opcodeSRLAux(uint8_t val);
	void opcodeSRL(SingleRegister& r);
	void opcodeSRL(uint16_t address);

	uint8_t opcodeSwapAux(uint8_t val);
	void opcodeSwap(SingleRegister& r);
	void opcodeSwap(uint16_t address);

	void opcodeBit(const int position, SingleRegister& r);
	void opcodeBit(const int position, uint16_t address);

	void opcodeReset(const int position, SingleRegister& r);
	void opcodeReset(const int position, uint16_t address);

	void opcodeSet(const int position, SingleRegister& r);
	void opcodeSet(const int position, uint16_t address);

	void opcodeDAA();
	void opcodeSCF();

	void opcodeJP();
	void opcodeJP(Flag fl);
	void opcodeJPHL();

	void opcodeJR();
	void opcodeJR(Flag fl);

	void opcodeAddSP();

	void opcodeAddAAux(uint8_t addedVal);
	void opcodeAddA();//get value from program counter
	void opcodeAddA(const SingleRegister& r);
	void opcodeAddA(const uint16_t address);

	void opcodeADCAux(uint8_t addedVal);
	void opcodeADC();
	void opcodeADC(const SingleRegister& r);
	void opcodeADC(const uint16_t address);

	void opcodeAddHLAux(uint16_t addedVal);
	void opcodeAddHL(const SplitRegister& addedVal);
	void opcodeAddHL(const FullRegister& addedVal);

	void opcodeSubAAux(uint8_t subbedVal);
	void opcodeSubA();
	void opcodeSubA(const SingleRegister& r);
	void opcodeSubA(const uint16_t address);

	void opcodeSBCAux(uint8_t subbedVal);
	void opcodeSBC();
	void opcodeSBC(const SingleRegister& r);
	void opcodeSBC(const uint16_t address);

	void opcodeAndAux(uint8_t val);
	void opcodeAnd();
	void opcodeAnd(const SingleRegister& r);
	void opcodeAnd(const uint16_t address);

	void opcodeXORAux(uint8_t val);
	void opcodeXOR();
	void opcodeXOR(const SingleRegister& r);
	void opcodeXOR(const uint16_t address);

	void opcodeOrAux(uint8_t val);
	void opcodeOr();
	void opcodeOr(const SingleRegister& r);
	void opcodeOr(const uint16_t address);

	void opcodeCPAux(uint8_t val);
	void opcodeCP();
	void opcodeCP(const SingleRegister& r);
	void opcodeCP(const uint16_t address);

	void opcodeCPL();
	void opcodeCCF();

	void opcodeHalt();

	void opcodeRet();
	void opcodeRet(Flag fl);
	void opcodeRetI();

	void opcodeCall();
	void opcodeCall(Flag fl);

	void opcodeRST(uint8_t location);//recheck

	void opcodeEI();
	void opcodeDI();

	void opcodeStop();
};