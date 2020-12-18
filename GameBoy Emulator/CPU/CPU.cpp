#include "CPU.h"

void CPU::executeOperations() {
	for (int ticks = 0; ticks <= 69905;) {
		uint16_t PCValue = PC.getValue();
		//uint8_t opcode = fetchOperations(PCValue);
		//int ticksToExecute = executeOpcode(opcode, PCValue);
		//ticks += ticksToExecute;
	}
}

uint8_t CPU::fetchOperations(uint16_t op) {
	//uint8_t opcode = memory fetcher stuff
	PC.increment();

}