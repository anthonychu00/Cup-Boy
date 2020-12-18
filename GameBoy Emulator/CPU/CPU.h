#pragma once
#include <cstdint>
#include "../registers/split_register.h"
#include "../registers/full_register.h"
class CPU {
public:
	
	CPU() = default;
	uint8_t fetchOperations(uint16_t op);
	void executeOperations();


	struct Clock {
		//figure out best representation of time units for CPU
		int p;	//time to execute previous operation (need to time how long each operation takes)
		int t;  //total time CPU has run
	};


	//fetch an op like uint8_t op = fetchOp(PC);
	//function to decode and execute ( lots of switch cases probably)

	//execution can either be between its own registers or performing an operation on the address bus

	// the CPU will use an address bus of 16 bits to communicate with memory outside its registers, it will only be able to read and write to these
	// this is basically how the CPU will affect the state of the actual emulator
private:
	//SplitRegisters can be used as a 16 bit address or 2 separate 8 bit ones
	SplitRegister AF;//A = high, F = low
	SplitRegister BC;
	SplitRegister DE;
	SplitRegister HL;

	//FullRegisters can only be used as a 16 bit address
	FullRegister SP;
	FullRegister PC;

	Clock clock;

	//fetch



};