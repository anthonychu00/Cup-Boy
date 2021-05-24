#include "CPU.h"
#include <iostream>
CPU::CPU(MemoryMap& newMM): 
	mm(newMM),
	AF(A, F),
	BC(B, C),
	DE(D, E),
	HL(H, L)
{
	PC.set(0x0100);//first instruction for roms, otherwise start at 0x0000
	divTime = 0;
	tima = 0;
	clockSpeed = 1024;
}

int CPU::executeOperations() {//69905
	
	if (halt) {
		//printf("Halt %d %d %d %d %d %d\n", PC.getValue(), IME, mm.readAddress(IERegister), mm.readAddress(IFRegister), mm.readAddress(timerCounter), getBit(mm.readAddress(timerControl), 2));
		if (IME == 0 && mm.readAddress(IERegister) == 0 && mm.readAddress(IFRegister) == 0 && getBit(mm.readAddress(timerControl), 2) == 0) {
			halt = false;
		}
		return 4;
	}

	uint16_t PCValue = PC.getValue();
	uint8_t opcode = PCFetchByte();

	if (executeTwice) {
		PC.decrement();
		executeTwice = false;
	}

	int ticksToExecute = executeOpcode(opcode, PCValue);

	//std::cout << unsigned(opcode) << endl;
	//std::cout << (char)unsigned(mm.readAddress(0xFF01)) << endl;
	//std::cout << ticks << endl;
	
	return ticksToExecute;
}

void CPU::checkInterruptRequests() {
	if (IME) {
		uint8_t interruptFlag = mm.readAddress(IFRegister);
		uint8_t interruptEnable = mm.readAddress(IERegister);
		uint8_t executableInterrupts = interruptFlag & interruptEnable;

		if (!executableInterrupts) {
			return;
		}
		//printf("Interrupts\n");
		halt = false;
		opcodeStackPush(PC);
		
		
		for (int position = 0; position < 5; position++) {
			if (getBit(interruptFlag, position) && getBit(interruptEnable, position)) {
				setBit(interruptFlag, position, 0); //reset bit in IF
				mm.writeAddress(IFRegister, interruptFlag);//corresponding bit is reset

				interruptExecute(position);//interrupt vector is called and executed

				IME = false;//IME flag reset

				return;
			}
		}
		
	}
	else if ((mm.readAddress(IFRegister) & mm.readAddress(IERegister)) && halt) { //CPU will still halt if there are no pending interrupts while ime = 0
		halt = false; //there are still pending interrupts, so we don't halt
		executeTwice = true;
	}
	
}

void CPU::interruptExecute(int vectorPosition) {
	//call interrupt vector
	if (vectorPosition == 0) {
		//printf("v-interrupt\n");
	}
	switch (vectorPosition) {
	case 0: PC.set(0x40); break; //v blank
	case 1:	PC.set(0x48); break; //LCD STAT
	case 2: PC.set(0x50); break; //Timer
	case 3: PC.set(0x58); break; //Serial
	case 4: PC.set(0x60); break; //Joypad/input
	}
	
}

void CPU::setInterruptFlag(int position) {
	uint8_t IFValue = mm.readAddress(IFRegister);
	setBit(IFValue, position, 1);
	mm.writeAddress(IFRegister, IFValue);
	//printf("%d", IFValue);
}

void CPU::updateTimer(int previousTicks) {
	bool timerEnable = getBit(mm.readAddress(timerControl), 2);

	//0xFF04
	divTime += previousTicks;
	if (divTime >= 256) { //divider increments every 256 CPU ticks
		//printf("div up\n");
		uint8_t currentDivTime = mm.readAddress(dividerRegister);
		mm.writeAddress(dividerRegister, currentDivTime + 1);
		divTime -= 256;
	}

	//0xFF05 & 0xFF06
	if (timerEnable) {
		//printf("Previous ticks: %d\n", previousTicks);
		tima += previousTicks;
		
		while (tima >= clockSpeed) { //tima increments every clockSpeed CPU ticks, can increment multiple times
			uint8_t currentTimerCounter = mm.readAddress(timerCounter);
			if (currentTimerCounter == 0xFF) { //timer overflow
				//printf("Overflow");
				mm.writeAddress(timerCounter, mm.readAddress(timerModulo));
				setInterruptFlag(2);

			}
			else {
				mm.writeAddress(timerCounter, currentTimerCounter + 1);
			}

			tima -= clockSpeed;		
		}
	}
}

void CPU::setClockSpeed(int newSpeed) {
	clockSpeed = newSpeed;
}
int CPU::getClockSpeed() {
	return clockSpeed;
}

int CPU::getDiv() {
	return divTime;
}
void CPU::resetDiv() {
	divTime = 0;
}

uint8_t CPU::PCFetchByte() {
	uint8_t byte = mm.readAddress(PC.getValue());
	PC.increment();
	return byte;
}

int8_t CPU::PCFetchSignedByte() {
	return static_cast<int8_t>(PCFetchByte());
}

uint16_t CPU::PCFetchWord() {
	//lower byte is first in memory from the PC according to timings
	uint8_t low = PCFetchByte();
	uint8_t high = PCFetchByte();

	return ((uint16_t)high << 8) | low;
}

void CPU::setClockPrevious(int ticks) {
	clock.p = ticks;
}

bool CPU::checkCondition(Flag fl) {
	bool result = 0;

	switch (fl) {
	case Flag::NZ:
		result = !F.getZeroFlag();
		break;
	case Flag::Z:
		result = F.getZeroFlag();
		break;
	case Flag::NC:
		result = !F.getCarryFlag();
		break;
	case Flag::C:
		result = F.getCarryFlag();
		break;

	}

	return result;
}

int CPU::executeOpcode(const uint8_t opcode, uint16_t PCValue) {
	if (opcode == 0xCB) {
		//cout << tracePrefixedOpcode(opcode) << endl;
		return executePrefixedOpcode(PCFetchByte(), PCValue);
	}
	//cout << traceOpcode(opcode) << endl;
	
	int ticks = 0;

	//perform action according to the opcode
	switch (opcode) {
		case 0x00: ticks = opcodeNOP(); break;
		case 0x01: ticks = opcodeLoadWord(BC); break;
		case 0x02: ticks = opcodeLoadAToMemory(BC.getValue()); break;
		case 0x03: ticks = opcodeIncrement(BC); break;
		case 0x04: ticks = opcodeIncrement(BC.getHighRegister()); break;
		case 0x05: ticks = opcodeDecrement(BC.getHighRegister()); break;
		case 0x06: ticks = opcodeLoadByte(BC.getHighRegister()); break;
		case 0x07: ticks = opcodeRLCA(); break;
		case 0x08: ticks = opcodeLoadSPToMemory(); break;
		case 0x09: ticks = opcodeAddHL(BC.getValue()); break;
		case 0x0A: ticks = opcodeLoadMemoryToA(BC.getValue()); break;
		case 0x0B: ticks = opcodeDecrement(BC); break;
		case 0x0C: ticks = opcodeIncrement(BC.getLowRegister()); break;
		case 0x0D: ticks = opcodeDecrement(BC.getLowRegister()); break;
		case 0x0E: ticks = opcodeLoadByte(BC.getLowRegister()); break;
		case 0x0F: ticks = opcodeRRCA(); break;
		case 0x10: ticks = opcodeStop(); break;
		case 0x11: ticks = opcodeLoadWord(DE); break;
		case 0x12: ticks = opcodeLoadAToMemory(DE.getValue()); break;
		case 0x13: ticks = opcodeIncrement(DE); break;
		case 0x14: ticks = opcodeIncrement(DE.getHighRegister()); break;
		case 0x15: ticks = opcodeDecrement(DE.getHighRegister()); break;
		case 0x16: ticks = opcodeLoadByte(DE.getHighRegister()); break;
		case 0x17: ticks = opcodeRLA(); break;
		case 0x18: ticks = opcodeJR(); break;
		case 0x19: ticks = opcodeAddHL(DE.getValue()); break;
		case 0x1A: ticks = opcodeLoadMemoryToA(DE.getValue()); break;
		case 0x1B: ticks = opcodeDecrement(DE); break;
		case 0x1C: ticks = opcodeIncrement(DE.getLowRegister()); break;
		case 0x1D: ticks = opcodeDecrement(DE.getLowRegister()); break;
		case 0x1E: ticks = opcodeLoadByte(DE.getLowRegister()); break;
		case 0x1F: ticks = opcodeRRA(); break;
		case 0x20: ticks = opcodeJR(Flag::NZ); break;
		case 0x21: ticks = opcodeLoadWord(HL); break;
		case 0x22: ticks = opcodeLoadAToMemoryInc(HL.getValue()); break;
		case 0x23: ticks = opcodeIncrement(HL); break;
		case 0x24: ticks = opcodeIncrement(HL.getHighRegister()); break;
		case 0x25: ticks = opcodeDecrement(HL.getHighRegister()); break;
		case 0x26: ticks = opcodeLoadByte(HL.getHighRegister()); break;
		case 0x27: ticks = opcodeDAA(); break;
		case 0x28: ticks = opcodeJR(Flag::Z); break;
		case 0x29: ticks = opcodeAddHL(HL.getValue()); break;
		case 0x2A: ticks = opcodeLoadMemoryToAInc(HL.getValue()); break;
		case 0x2B: ticks = opcodeDecrement(HL); break;
		case 0x2C: ticks = opcodeIncrement(HL.getLowRegister()); break;
		case 0x2D: ticks = opcodeDecrement(HL.getLowRegister()); break;
		case 0x2E: ticks = opcodeLoadByte(HL.getLowRegister()); break;
		case 0x2F: ticks = opcodeCPL(); break;
		case 0x30: ticks = opcodeJR(Flag::NC); break;
		case 0x31: ticks = opcodeLoadWord(SP); break;
		case 0x32: ticks = opcodeLoadAToMemoryDec(HL.getValue()); break;
		case 0x33: ticks = opcodeIncrement(SP); break;
		case 0x34: ticks = opcodeIncrement(HL.getValue()); break;
		case 0x35: ticks = opcodeDecrement(HL.getValue()); break;
		case 0x36: ticks = opcodeLoadByte(HL.getValue()); break;
		case 0x37: ticks = opcodeSCF(); break;
		case 0x38: ticks = opcodeJR(Flag::C); break;
		case 0x39: ticks = opcodeAddHL(SP.getValue()); break;
		case 0x3A: ticks = opcodeLoadMemoryToADec(HL.getValue()); break;
		case 0x3B: ticks = opcodeDecrement(SP); break;
		case 0x3C: ticks = opcodeIncrement(AF.getHighRegister()); break;
		case 0x3D: ticks = opcodeDecrement(AF.getHighRegister()); break;
		case 0x3E: ticks = opcodeLoadByte(AF.getHighRegister()); break;
		case 0x3F: ticks = opcodeCCF(); break;
		case 0x40: ticks = opcodeLoadByte(BC.getHighRegister(), BC.getHighRegister()); break;
		case 0x41: ticks = opcodeLoadByte(BC.getHighRegister(), BC.getLowRegister()); break;
		case 0x42: ticks = opcodeLoadByte(BC.getHighRegister(), DE.getHighRegister()); break;
		case 0x43: ticks = opcodeLoadByte(BC.getHighRegister(), DE.getLowRegister()); break;
		case 0x44: ticks = opcodeLoadByte(BC.getHighRegister(), HL.getHighRegister()); break;
		case 0x45: ticks = opcodeLoadByte(BC.getHighRegister(), HL.getLowRegister()); break;
		case 0x46: ticks = opcodeLoadByte(BC.getHighRegister(), HL.getValue()); break;
		case 0x47: ticks = opcodeLoadByte(BC.getHighRegister(), AF.getHighRegister()); break;
		case 0x48: ticks = opcodeLoadByte(BC.getLowRegister(), BC.getHighRegister()); break;
		case 0x49: ticks = opcodeLoadByte(BC.getLowRegister(), BC.getLowRegister()); break;
		case 0x4A: ticks = opcodeLoadByte(BC.getLowRegister(), DE.getHighRegister()); break;
		case 0x4B: ticks = opcodeLoadByte(BC.getLowRegister(), DE.getLowRegister()); break;
		case 0x4C: ticks = opcodeLoadByte(BC.getLowRegister(), HL.getHighRegister()); break;
		case 0x4D: ticks = opcodeLoadByte(BC.getLowRegister(), HL.getLowRegister()); break;
		case 0x4E: ticks = opcodeLoadByte(BC.getLowRegister(), HL.getValue()); break;
		case 0x4F: ticks = opcodeLoadByte(BC.getLowRegister(), AF.getHighRegister()); break;
		case 0x50: ticks = opcodeLoadByte(DE.getHighRegister(), BC.getHighRegister()); break;
		case 0x51: ticks = opcodeLoadByte(DE.getHighRegister(), BC.getLowRegister()); break;
		case 0x52: ticks = opcodeLoadByte(DE.getHighRegister(), DE.getHighRegister()); break;
		case 0x53: ticks = opcodeLoadByte(DE.getHighRegister(), DE.getLowRegister()); break;
		case 0x54: ticks = opcodeLoadByte(DE.getHighRegister(), HL.getHighRegister()); break;
		case 0x55: ticks = opcodeLoadByte(DE.getHighRegister(), HL.getLowRegister()); break;
		case 0x56: ticks = opcodeLoadByte(DE.getHighRegister(), HL.getValue()); break;
		case 0x57: ticks = opcodeLoadByte(DE.getHighRegister(), AF.getHighRegister()); break;
		case 0x58: ticks = opcodeLoadByte(DE.getLowRegister(), BC.getHighRegister()); break;
		case 0x59: ticks = opcodeLoadByte(DE.getLowRegister(), BC.getLowRegister()); break;
		case 0x5A: ticks = opcodeLoadByte(DE.getLowRegister(), DE.getHighRegister()); break;
		case 0x5B: ticks = opcodeLoadByte(DE.getLowRegister(), DE.getLowRegister()); break;
		case 0x5C: ticks = opcodeLoadByte(DE.getLowRegister(), HL.getHighRegister()); break;
		case 0x5D: ticks = opcodeLoadByte(DE.getLowRegister(), HL.getLowRegister()); break;
		case 0x5E: ticks = opcodeLoadByte(DE.getLowRegister(), HL.getValue()); break;
		case 0x5F: ticks = opcodeLoadByte(DE.getLowRegister(), AF.getHighRegister()); break;
		case 0x60: ticks = opcodeLoadByte(HL.getHighRegister(), BC.getHighRegister()); break;
		case 0x61: ticks = opcodeLoadByte(HL.getHighRegister(), BC.getLowRegister()); break;
		case 0x62: ticks = opcodeLoadByte(HL.getHighRegister(), DE.getHighRegister()); break;
		case 0x63: ticks = opcodeLoadByte(HL.getHighRegister(), DE.getLowRegister()); break;
		case 0x64: ticks = opcodeLoadByte(HL.getHighRegister(), HL.getHighRegister()); break;
		case 0x65: ticks = opcodeLoadByte(HL.getHighRegister(), HL.getLowRegister()); break;
		case 0x66: ticks = opcodeLoadByte(HL.getHighRegister(), HL.getValue()); break;
		case 0x67: ticks = opcodeLoadByte(HL.getHighRegister(), AF.getHighRegister()); break;
		case 0x68: ticks = opcodeLoadByte(HL.getLowRegister(), BC.getHighRegister()); break;
		case 0x69: ticks = opcodeLoadByte(HL.getLowRegister(), BC.getLowRegister()); break;
		case 0x6A: ticks = opcodeLoadByte(HL.getLowRegister(), DE.getHighRegister()); break;
		case 0x6B: ticks = opcodeLoadByte(HL.getLowRegister(), DE.getLowRegister()); break;
		case 0x6C: ticks = opcodeLoadByte(HL.getLowRegister(), HL.getHighRegister()); break;
		case 0x6D: ticks = opcodeLoadByte(HL.getLowRegister(), HL.getLowRegister()); break;
		case 0x6E: ticks = opcodeLoadByte(HL.getLowRegister(), HL.getValue()); break;
		case 0x6F: ticks = opcodeLoadByte(HL.getLowRegister(), AF.getHighRegister()); break;
		case 0x70: ticks = opcodeLoadByte(HL.getValue(), BC.getHighRegister()); break;
		case 0x71: ticks = opcodeLoadByte(HL.getValue(), BC.getLowRegister()); break;
		case 0x72: ticks = opcodeLoadByte(HL.getValue(), DE.getHighRegister()); break;
		case 0x73: ticks = opcodeLoadByte(HL.getValue(), DE.getLowRegister()); break;
		case 0x74: ticks = opcodeLoadByte(HL.getValue(), HL.getHighRegister()); break;
		case 0x75: ticks = opcodeLoadByte(HL.getValue(), HL.getLowRegister()); break;
		case 0x76: ticks = opcodeHalt(); break;
		case 0x77: ticks = opcodeLoadByte(HL.getValue(), AF.getHighRegister()); break;
		case 0x78: ticks = opcodeLoadByte(AF.getHighRegister(), BC.getHighRegister()); break;
		case 0x79: ticks = opcodeLoadByte(AF.getHighRegister(), BC.getLowRegister()); break;
		case 0x7A: ticks = opcodeLoadByte(AF.getHighRegister(), DE.getHighRegister()); break;
		case 0x7B: ticks = opcodeLoadByte(AF.getHighRegister(), DE.getLowRegister()); break;
		case 0x7C: ticks = opcodeLoadByte(AF.getHighRegister(), HL.getHighRegister()); break;
		case 0x7D: ticks = opcodeLoadByte(AF.getHighRegister(), HL.getLowRegister()); break;
		case 0x7E: ticks = opcodeLoadByte(AF.getHighRegister(), HL.getValue()); break;
		case 0x7F: ticks = opcodeLoadByte(AF.getHighRegister(), AF.getHighRegister()); break;
		case 0x80: ticks = opcodeAddA(BC.getHighRegister()); break;
		case 0x81: ticks = opcodeAddA(BC.getLowRegister()); break;
		case 0x82: ticks = opcodeAddA(DE.getHighRegister()); break;
		case 0x83: ticks = opcodeAddA(DE.getLowRegister()); break;
		case 0x84: ticks = opcodeAddA(HL.getHighRegister()); break;
		case 0x85: ticks = opcodeAddA(HL.getLowRegister()); break;
		case 0x86: ticks = opcodeAddA(HL.getValue()); break;
		case 0x87: ticks = opcodeAddA(AF.getHighRegister()); break;
		case 0x88: ticks = opcodeADC(BC.getHighRegister()); break;
		case 0x89: ticks = opcodeADC(BC.getLowRegister()); break;
		case 0x8A: ticks = opcodeADC(DE.getHighRegister()); break;
		case 0x8B: ticks = opcodeADC(DE.getLowRegister()); break;
		case 0x8C: ticks = opcodeADC(HL.getHighRegister()); break;
		case 0x8D: ticks = opcodeADC(HL.getLowRegister()); break;
		case 0x8E: ticks = opcodeADC(HL.getValue()); break;
		case 0x8F: ticks = opcodeADC(AF.getHighRegister()); break;
		case 0x90: ticks = opcodeSubA(BC.getHighRegister()); break;
		case 0x91: ticks = opcodeSubA(BC.getLowRegister()); break;
		case 0x92: ticks = opcodeSubA(DE.getHighRegister()); break;
		case 0x93: ticks = opcodeSubA(DE.getLowRegister()); break;
		case 0x94: ticks = opcodeSubA(HL.getHighRegister()); break;
		case 0x95: ticks = opcodeSubA(HL.getLowRegister()); break;
		case 0x96: ticks = opcodeSubA(HL.getValue()); break;
		case 0x97: ticks = opcodeSubA(AF.getHighRegister()); break;
		case 0x98: ticks = opcodeSBC(BC.getHighRegister()); break;
		case 0x99: ticks = opcodeSBC(BC.getLowRegister()); break;
		case 0x9A: ticks = opcodeSBC(DE.getHighRegister()); break;
		case 0x9B: ticks = opcodeSBC(DE.getLowRegister()); break;
		case 0x9C: ticks = opcodeSBC(HL.getHighRegister()); break;
		case 0x9D: ticks = opcodeSBC(HL.getLowRegister()); break;
		case 0x9E: ticks = opcodeSBC(HL.getValue()); break;
		case 0x9F: ticks = opcodeSBC(AF.getHighRegister()); break;
		case 0xA0: ticks = opcodeAnd(BC.getHighRegister()); break;
		case 0xA1: ticks = opcodeAnd(BC.getLowRegister()); break;
		case 0xA2: ticks = opcodeAnd(DE.getHighRegister()); break;
		case 0xA3: ticks = opcodeAnd(DE.getLowRegister()); break;
		case 0xA4: ticks = opcodeAnd(HL.getHighRegister()); break;
		case 0xA5: ticks = opcodeAnd(HL.getLowRegister()); break;
		case 0xA6: ticks = opcodeAnd(HL.getValue()); break;
		case 0xA7: ticks = opcodeAnd(AF.getHighRegister()); break;
		case 0xA8: ticks = opcodeXOR(BC.getHighRegister()); break;
		case 0xA9: ticks = opcodeXOR(BC.getLowRegister()); break;
		case 0xAA: ticks = opcodeXOR(DE.getHighRegister()); break;
		case 0xAB: ticks = opcodeXOR(DE.getLowRegister()); break;
		case 0xAC: ticks = opcodeXOR(HL.getHighRegister()); break;
		case 0xAD: ticks = opcodeXOR(HL.getLowRegister()); break;
		case 0xAE: ticks = opcodeXOR(HL.getValue()); break;
		case 0xAF: ticks = opcodeXOR(AF.getHighRegister()); break;
		case 0xB0: ticks = opcodeOr(BC.getHighRegister()); break;
		case 0xB1: ticks = opcodeOr(BC.getLowRegister()); break;
		case 0xB2: ticks = opcodeOr(DE.getHighRegister()); break;
		case 0xB3: ticks = opcodeOr(DE.getLowRegister()); break;
		case 0xB4: ticks = opcodeOr(HL.getHighRegister()); break;
		case 0xB5: ticks = opcodeOr(HL.getLowRegister()); break;
		case 0xB6: ticks = opcodeOr(HL.getValue()); break;
		case 0xB7: ticks = opcodeOr(AF.getHighRegister()); break;
		case 0xB8: ticks = opcodeCP(BC.getHighRegister()); break;
		case 0xB9: ticks = opcodeCP(BC.getLowRegister()); break;
		case 0xBA: ticks = opcodeCP(DE.getHighRegister()); break;
		case 0xBB: ticks = opcodeCP(DE.getLowRegister()); break;
		case 0xBC: ticks = opcodeCP(HL.getHighRegister()); break;
		case 0xBD: ticks = opcodeCP(HL.getLowRegister()); break;
		case 0xBE: ticks = opcodeCP(HL.getValue()); break;
		case 0xBF: ticks = opcodeCP(AF.getHighRegister()); break;
		case 0xC0: ticks = opcodeRet(Flag::NZ); break;
		case 0xC1: ticks = opcodeStackPop(BC); break;
		case 0xC2: ticks = opcodeJP(Flag::NZ); break;
		case 0xC3: ticks = opcodeJP(); break;
		case 0xC4: ticks = opcodeCall(Flag::NZ); break;
		case 0xC5: ticks = opcodeStackPush(BC); break;
		case 0xC6: ticks = opcodeAddA(); break;
		case 0xC7: ticks = opcodeRST(0x00); break;
		case 0xC8: ticks = opcodeRet(Flag::Z); break;
		case 0xC9: ticks = opcodeRet(); break;
		case 0xCA: ticks = opcodeJP(Flag::Z); break;
		case 0xCC: ticks = opcodeCall(Flag::Z); break;
		case 0xCD: ticks = opcodeCall(); break;
		case 0xCE: ticks = opcodeADC(); break;
		case 0xCF: ticks = opcodeRST(0x08); break;
		case 0xD0: ticks = opcodeRet(Flag::NC); break;
		case 0xD1: ticks = opcodeStackPop(DE); break;
		case 0xD2: ticks = opcodeJP(Flag::NC); break;
		case 0xD3: break; //no operation
		case 0xD4: ticks = opcodeCall(Flag::NC); break;
		case 0xD5: ticks = opcodeStackPush(DE); break;
		case 0xD6: ticks = opcodeSubA(); break;
		case 0xD7: ticks = opcodeRST(0x10); break;
		case 0xD8: ticks = opcodeRet(Flag::C); break;
		case 0xD9: ticks = opcodeRetI(); break;
		case 0xDA: ticks = opcodeJP(Flag::C); break;
		case 0xDB: break; //no operation
		case 0xDC: ticks = opcodeCall(Flag::C); break;
		case 0xDD: break; //no operation
		case 0xDE: ticks = opcodeSBC(); break;
		case 0xDF: ticks = opcodeRST(0x18); break;
		case 0xE0: ticks = opcodeLoadAToMemory(); break;
		case 0xE1: ticks = opcodeStackPop(HL); break;
		case 0xE2: ticks = opcodeLoadAToMemory(BC.getLowRegister()); break;
		case 0xE3: break; //no operation
		case 0xE4: break; //no operation
		case 0xE5: ticks = opcodeStackPush(HL); break;
		case 0xE6: ticks = opcodeAnd(); break;
		case 0xE7: ticks = opcodeRST(0x20); break;
		case 0xE8: ticks = opcodeAddSP(); break;
		case 0xE9: ticks = opcodeJPHL(); break;
		case 0xEA: ticks = opcodeLoadAToMemory16(); break;
		case 0xEB: break; //no operation
		case 0xEC: break; //no operation
		case 0xED: break; //no operation
		case 0xEE: ticks = opcodeXOR(); break;
		case 0xEF: ticks = opcodeRST(0x28); break;
		case 0xF0: ticks = opcodeLoadMemoryToA(); break;
		case 0xF1: ticks = opcodeStackPop(AF, true); break;
		case 0xF2: ticks = opcodeLoadMemoryToA(BC.getLowRegister()); break;
		case 0xF3: ticks = opcodeDI(); break;
		case 0xF4: break; //no operation
		case 0xF5: ticks = opcodeStackPush(AF); break;
		case 0xF6: ticks = opcodeOr(); break;
		case 0xF7: ticks = opcodeRST(0x30); break;
		case 0xF8: ticks = opcodeLoadHLSP(); break;
		case 0xF9: ticks = opcodeLoadSPHL(); break;
		case 0xFA: ticks = opcodeLoadMemoryToA16(); break;
		case 0xFB: ticks = opcodeEI(); break;
		case 0xFC: break; //no operation
		case 0xFD: break; //no operation
		case 0xFE: ticks = opcodeCP(); break;
		case 0xFF: ticks = opcodeRST(0x38); break;
	}

	return ticks;
	
}

int CPU::executePrefixedOpcode(const uint8_t opcode, uint16_t PCValue) {
	int ticks = 0;

	switch (opcode) {
	case 0x00: ticks = opcodeRLC(BC.getHighRegister()); break;
	case 0x01: ticks = opcodeRLC(BC.getLowRegister()); break;
	case 0x02: ticks = opcodeRLC(DE.getHighRegister()); break;
	case 0x03: ticks = opcodeRLC(DE.getLowRegister()); break;
	case 0x04: ticks = opcodeRLC(HL.getHighRegister()); break;
	case 0x05: ticks = opcodeRLC(HL.getLowRegister()); break;
	case 0x06: ticks = opcodeRLC(HL.getValue()); break;
	case 0x07: ticks = opcodeRLC(AF.getHighRegister()); break;
	case 0x08: ticks = opcodeRRC(BC.getHighRegister()); break;
	case 0x09: ticks = opcodeRRC(BC.getLowRegister()); break;
	case 0x0A: ticks = opcodeRRC(DE.getHighRegister()); break;
	case 0x0B: ticks = opcodeRRC(DE.getLowRegister()); break;
	case 0x0C: ticks = opcodeRRC(HL.getHighRegister()); break;
	case 0x0D: ticks = opcodeRRC(HL.getLowRegister()); break;
	case 0x0E: ticks = opcodeRRC(HL.getValue()); break;
	case 0x0F: ticks = opcodeRRC(AF.getHighRegister()); break;
	case 0x10: ticks = opcodeRL(BC.getHighRegister()); break;
	case 0x11: ticks = opcodeRL(BC.getLowRegister()); break;
	case 0x12: ticks = opcodeRL(DE.getHighRegister()); break;
	case 0x13: ticks = opcodeRL(DE.getLowRegister()); break;
	case 0x14: ticks = opcodeRL(HL.getHighRegister()); break;
	case 0x15: ticks = opcodeRL(HL.getLowRegister()); break;
	case 0x16: ticks = opcodeRL(HL.getValue()); break;
	case 0x17: ticks = opcodeRL(AF.getHighRegister()); break;
	case 0x18: ticks = opcodeRR(BC.getHighRegister()); break;
	case 0x19: ticks = opcodeRR(BC.getLowRegister()); break;
	case 0x1A: ticks = opcodeRR(DE.getHighRegister()); break;
	case 0x1B: ticks = opcodeRR(DE.getLowRegister()); break;
	case 0x1C: ticks = opcodeRR(HL.getHighRegister()); break;
	case 0x1D: ticks = opcodeRR(HL.getLowRegister()); break;
	case 0x1E: ticks = opcodeRR(HL.getValue()); break;
	case 0x1F: ticks = opcodeRR(AF.getHighRegister()); break;
	case 0x20: ticks = opcodeSLA(BC.getHighRegister()); break;
	case 0x21: ticks = opcodeSLA(BC.getLowRegister()); break;
	case 0x22: ticks = opcodeSLA(DE.getHighRegister()); break;
	case 0x23: ticks = opcodeSLA(DE.getLowRegister()); break;
	case 0x24: ticks = opcodeSLA(HL.getHighRegister()); break;
	case 0x25: ticks = opcodeSLA(HL.getLowRegister()); break;
	case 0x26: ticks = opcodeSLA(HL.getValue()); break;
	case 0x27: ticks = opcodeSLA(AF.getHighRegister()); break;
	case 0x28: ticks = opcodeSRA(BC.getHighRegister()); break;
	case 0x29: ticks = opcodeSRA(BC.getLowRegister()); break;
	case 0x2A: ticks = opcodeSRA(DE.getHighRegister()); break;
	case 0x2B: ticks = opcodeSRA(DE.getLowRegister()); break;
	case 0x2C: ticks = opcodeSRA(HL.getHighRegister()); break;
	case 0x2D: ticks = opcodeSRA(HL.getLowRegister()); break;
	case 0x2E: ticks = opcodeSRA(HL.getValue()); break;
	case 0x2F: ticks = opcodeSRA(AF.getHighRegister()); break;
	case 0x30: ticks = opcodeSwap(BC.getHighRegister()); break;
	case 0x31: ticks = opcodeSwap(BC.getLowRegister()); break;
	case 0x32: ticks = opcodeSwap(DE.getHighRegister()); break;
	case 0x33: ticks = opcodeSwap(DE.getLowRegister()); break;
	case 0x34: ticks = opcodeSwap(HL.getHighRegister()); break;
	case 0x35: ticks = opcodeSwap(HL.getLowRegister()); break;
	case 0x36: ticks = opcodeSwap(HL.getValue()); break;
	case 0x37: ticks = opcodeSwap(AF.getHighRegister()); break;
	case 0x38: ticks = opcodeSRL(BC.getHighRegister()); break;
	case 0x39: ticks = opcodeSRL(BC.getLowRegister()); break;
	case 0x3A: ticks = opcodeSRL(DE.getHighRegister()); break;
	case 0x3B: ticks = opcodeSRL(DE.getLowRegister()); break;
	case 0x3C: ticks = opcodeSRL(HL.getHighRegister()); break;
	case 0x3D: ticks = opcodeSRL(HL.getLowRegister()); break;
	case 0x3E: ticks = opcodeSRL(HL.getValue()); break;
	case 0x3F: ticks = opcodeSRL(AF.getHighRegister()); break;
	case 0x40: ticks = opcodeBit(0, BC.getHighRegister()); break;
	case 0x41: ticks = opcodeBit(0, BC.getLowRegister()); break;
	case 0x42: ticks = opcodeBit(0, DE.getHighRegister()); break;
	case 0x43: ticks = opcodeBit(0, DE.getLowRegister()); break;
	case 0x44: ticks = opcodeBit(0, HL.getHighRegister()); break;
	case 0x45: ticks = opcodeBit(0, HL.getLowRegister()); break;
	case 0x46: ticks = opcodeBit(0, HL.getValue()); break;
	case 0x47: ticks = opcodeBit(0, AF.getHighRegister()); break;
	case 0x48: ticks = opcodeBit(1, BC.getHighRegister()); break;
	case 0x49: ticks = opcodeBit(1, BC.getLowRegister()); break;
	case 0x4A: ticks = opcodeBit(1, DE.getHighRegister()); break;
	case 0x4B: ticks = opcodeBit(1, DE.getLowRegister()); break;
	case 0x4C: ticks = opcodeBit(1, HL.getHighRegister()); break;
	case 0x4D: ticks = opcodeBit(1, HL.getLowRegister()); break;
	case 0x4E: ticks = opcodeBit(1, HL.getValue()); break;
	case 0x4F: ticks = opcodeBit(1, AF.getHighRegister()); break;
	case 0x50: ticks = opcodeBit(2, BC.getHighRegister()); break;
	case 0x51: ticks = opcodeBit(2, BC.getLowRegister()); break;
	case 0x52: ticks = opcodeBit(2, DE.getHighRegister()); break;
	case 0x53: ticks = opcodeBit(2, DE.getLowRegister()); break;
	case 0x54: ticks = opcodeBit(2, HL.getHighRegister()); break;
	case 0x55: ticks = opcodeBit(2, HL.getLowRegister()); break;
	case 0x56: ticks = opcodeBit(2, HL.getValue()); break;
	case 0x57: ticks = opcodeBit(2, AF.getHighRegister()); break;
	case 0x58: ticks = opcodeBit(3, BC.getHighRegister()); break;
	case 0x59: ticks = opcodeBit(3, BC.getLowRegister()); break;
	case 0x5A: ticks = opcodeBit(3, DE.getHighRegister()); break;
	case 0x5B: ticks = opcodeBit(3, DE.getLowRegister()); break;
	case 0x5C: ticks = opcodeBit(3, HL.getHighRegister()); break;
	case 0x5D: ticks = opcodeBit(3, HL.getLowRegister()); break;
	case 0x5E: ticks = opcodeBit(3, HL.getValue()); break;
	case 0x5F: ticks = opcodeBit(3, AF.getHighRegister()); break;
	case 0x60: ticks = opcodeBit(4, BC.getHighRegister()); break;
	case 0x61: ticks = opcodeBit(4, BC.getLowRegister()); break;
	case 0x62: ticks = opcodeBit(4, DE.getHighRegister()); break;
	case 0x63: ticks = opcodeBit(4, DE.getLowRegister()); break;
	case 0x64: ticks = opcodeBit(4, HL.getHighRegister()); break;
	case 0x65: ticks = opcodeBit(4, HL.getLowRegister()); break;
	case 0x66: ticks = opcodeBit(4, HL.getValue()); break;
	case 0x67: ticks = opcodeBit(4, AF.getHighRegister()); break;
	case 0x68: ticks = opcodeBit(5, BC.getHighRegister()); break;
	case 0x69: ticks = opcodeBit(5, BC.getLowRegister()); break;
	case 0x6A: ticks = opcodeBit(5, DE.getHighRegister()); break;
	case 0x6B: ticks = opcodeBit(5, DE.getLowRegister()); break;
	case 0x6C: ticks = opcodeBit(5, HL.getHighRegister()); break;
	case 0x6D: ticks = opcodeBit(5, HL.getLowRegister()); break;
	case 0x6E: ticks = opcodeBit(5, HL.getValue()); break;
	case 0x6F: ticks = opcodeBit(5, AF.getHighRegister()); break;
	case 0x70: ticks = opcodeBit(6, BC.getHighRegister()); break;
	case 0x71: ticks = opcodeBit(6, BC.getLowRegister()); break;
	case 0x72: ticks = opcodeBit(6, DE.getHighRegister()); break;
	case 0x73: ticks = opcodeBit(6, DE.getLowRegister()); break;
	case 0x74: ticks = opcodeBit(6, HL.getHighRegister()); break;
	case 0x75: ticks = opcodeBit(6, HL.getLowRegister()); break;
	case 0x76: ticks = opcodeBit(6, HL.getValue()); break;
	case 0x77: ticks = opcodeBit(6, AF.getHighRegister()); break;
	case 0x78: ticks = opcodeBit(7, BC.getHighRegister()); break;
	case 0x79: ticks = opcodeBit(7, BC.getLowRegister()); break;
	case 0x7A: ticks = opcodeBit(7, DE.getHighRegister()); break;
	case 0x7B: ticks = opcodeBit(7, DE.getLowRegister()); break;
	case 0x7C: ticks = opcodeBit(7, HL.getHighRegister()); break;
	case 0x7D: ticks = opcodeBit(7, HL.getLowRegister()); break;
	case 0x7E: ticks = opcodeBit(7, HL.getValue()); break;
	case 0x7F: ticks = opcodeBit(7, AF.getHighRegister()); break;
	case 0x80: ticks = opcodeReset(0, BC.getHighRegister()); break;
	case 0x81: ticks = opcodeReset(0, BC.getLowRegister()); break;
	case 0x82: ticks = opcodeReset(0, DE.getHighRegister()); break;
	case 0x83: ticks = opcodeReset(0, DE.getLowRegister()); break;
	case 0x84: ticks = opcodeReset(0, HL.getHighRegister()); break;
	case 0x85: ticks = opcodeReset(0, HL.getLowRegister()); break;
	case 0x86: ticks = opcodeReset(0, HL.getValue()); break;
	case 0x87: ticks = opcodeReset(0, AF.getHighRegister()); break;
	case 0x88: ticks = opcodeReset(1, BC.getHighRegister()); break;
	case 0x89: ticks = opcodeReset(1, BC.getLowRegister()); break;
	case 0x8A: ticks = opcodeReset(1, DE.getHighRegister()); break;
	case 0x8B: ticks = opcodeReset(1, DE.getLowRegister()); break;
	case 0x8C: ticks = opcodeReset(1, HL.getHighRegister()); break;
	case 0x8D: ticks = opcodeReset(1, HL.getLowRegister()); break;
	case 0x8E: ticks = opcodeReset(1, HL.getValue()); break;
	case 0x8F: ticks = opcodeReset(1, AF.getHighRegister()); break;
	case 0x90: ticks = opcodeReset(2, BC.getHighRegister()); break;
	case 0x91: ticks = opcodeReset(2, BC.getLowRegister()); break;
	case 0x92: ticks = opcodeReset(2, DE.getHighRegister()); break;
	case 0x93: ticks = opcodeReset(2, DE.getLowRegister()); break;
	case 0x94: ticks = opcodeReset(2, HL.getHighRegister()); break;
	case 0x95: ticks = opcodeReset(2, HL.getLowRegister()); break;
	case 0x96: ticks = opcodeReset(2, HL.getValue()); break;
	case 0x97: ticks = opcodeReset(2, AF.getHighRegister()); break;
	case 0x98: ticks = opcodeReset(3, BC.getHighRegister()); break;
	case 0x99: ticks = opcodeReset(3, BC.getLowRegister()); break;
	case 0x9A: ticks = opcodeReset(3, DE.getHighRegister()); break;
	case 0x9B: ticks = opcodeReset(3, DE.getLowRegister()); break;
	case 0x9C: ticks = opcodeReset(3, HL.getHighRegister()); break;
	case 0x9D: ticks = opcodeReset(3, HL.getLowRegister()); break;
	case 0x9E: ticks = opcodeReset(3, HL.getValue()); break;
	case 0x9F: ticks = opcodeReset(3, AF.getHighRegister()); break;
	case 0xA0: ticks = opcodeReset(4, BC.getHighRegister()); break;
	case 0xA1: ticks = opcodeReset(4, BC.getLowRegister()); break;
	case 0xA2: ticks = opcodeReset(4, DE.getHighRegister()); break;
	case 0xA3: ticks = opcodeReset(4, DE.getLowRegister()); break;
	case 0xA4: ticks = opcodeReset(4, HL.getHighRegister()); break;
	case 0xA5: ticks = opcodeReset(4, HL.getLowRegister()); break;
	case 0xA6: ticks = opcodeReset(4, HL.getValue()); break;
	case 0xA7: ticks = opcodeReset(4, AF.getHighRegister()); break;
	case 0xA8: ticks = opcodeReset(5, BC.getHighRegister()); break;
	case 0xA9: ticks = opcodeReset(5, BC.getLowRegister()); break;
	case 0xAA: ticks = opcodeReset(5, DE.getHighRegister()); break;
	case 0xAB: ticks = opcodeReset(5, DE.getLowRegister()); break;
	case 0xAC: ticks = opcodeReset(5, HL.getHighRegister()); break;
	case 0xAD: ticks = opcodeReset(5, HL.getLowRegister()); break;
	case 0xAE: ticks = opcodeReset(5, HL.getValue()); break;
	case 0xAF: ticks = opcodeReset(5, AF.getHighRegister()); break;
	case 0xB0: ticks = opcodeReset(6, BC.getHighRegister()); break;
	case 0xB1: ticks = opcodeReset(6, BC.getLowRegister()); break;
	case 0xB2: ticks = opcodeReset(6, DE.getHighRegister()); break;
	case 0xB3: ticks = opcodeReset(6, DE.getLowRegister()); break;
	case 0xB4: ticks = opcodeReset(6, HL.getHighRegister()); break;
	case 0xB5: ticks = opcodeReset(6, HL.getLowRegister()); break;
	case 0xB6: ticks = opcodeReset(6, HL.getValue()); break;
	case 0xB7: ticks = opcodeReset(6, AF.getHighRegister()); break;
	case 0xB8: ticks = opcodeReset(7, BC.getHighRegister()); break;
	case 0xB9: ticks = opcodeReset(7, BC.getLowRegister()); break;
	case 0xBA: ticks = opcodeReset(7, DE.getHighRegister()); break;
	case 0xBB: ticks = opcodeReset(7, DE.getLowRegister()); break;
	case 0xBC: ticks = opcodeReset(7, HL.getHighRegister()); break;
	case 0xBD: ticks = opcodeReset(7, HL.getLowRegister()); break;
	case 0xBE: ticks = opcodeReset(7, HL.getValue()); break;
	case 0xBF: ticks = opcodeReset(7, AF.getHighRegister()); break;
	case 0xC0: ticks = opcodeSet(0, BC.getHighRegister()); break;
	case 0xC1: ticks = opcodeSet(0, BC.getLowRegister()); break;
	case 0xC2: ticks = opcodeSet(0, DE.getHighRegister()); break;
	case 0xC3: ticks = opcodeSet(0, DE.getLowRegister()); break;
	case 0xC4: ticks = opcodeSet(0, HL.getHighRegister()); break;
	case 0xC5: ticks = opcodeSet(0, HL.getLowRegister()); break;
	case 0xC6: ticks = opcodeSet(0, HL.getValue()); break;
	case 0xC7: ticks = opcodeSet(0, AF.getHighRegister()); break;
	case 0xC8: ticks = opcodeSet(1, BC.getHighRegister()); break;
	case 0xC9: ticks = opcodeSet(1, BC.getLowRegister()); break;
	case 0xCA: ticks = opcodeSet(1, DE.getHighRegister()); break;
	case 0xCB: ticks = opcodeSet(1, DE.getLowRegister()); break;
	case 0xCC: ticks = opcodeSet(1, HL.getHighRegister()); break;
	case 0xCD: ticks = opcodeSet(1, HL.getLowRegister()); break;
	case 0xCE: ticks = opcodeSet(1, HL.getValue()); break;
	case 0xCF: ticks = opcodeSet(1, AF.getHighRegister()); break;
	case 0xD0: ticks = opcodeSet(2, BC.getHighRegister()); break;
	case 0xD1: ticks = opcodeSet(2, BC.getLowRegister()); break;
	case 0xD2: ticks = opcodeSet(2, DE.getHighRegister()); break;
	case 0xD3: ticks = opcodeSet(2, DE.getLowRegister()); break;
	case 0xD4: ticks = opcodeSet(2, HL.getHighRegister()); break;
	case 0xD5: ticks = opcodeSet(2, HL.getLowRegister()); break;
	case 0xD6: ticks = opcodeSet(2, HL.getValue()); break;
	case 0xD7: ticks = opcodeSet(2, AF.getHighRegister()); break;
	case 0xD8: ticks = opcodeSet(3, BC.getHighRegister()); break;
	case 0xD9: ticks = opcodeSet(3, BC.getLowRegister()); break;
	case 0xDA: ticks = opcodeSet(3, DE.getHighRegister()); break;
	case 0xDB: ticks = opcodeSet(3, DE.getLowRegister()); break;
	case 0xDC: ticks = opcodeSet(3, HL.getHighRegister()); break;
	case 0xDD: ticks = opcodeSet(3, HL.getLowRegister()); break;
	case 0xDE: ticks = opcodeSet(3, HL.getValue()); break;
	case 0xDF: ticks = opcodeSet(3, AF.getHighRegister()); break;
	case 0xE0: ticks = opcodeSet(4, BC.getHighRegister()); break;
	case 0xE1: ticks = opcodeSet(4, BC.getLowRegister()); break;
	case 0xE2: ticks = opcodeSet(4, DE.getHighRegister()); break;
	case 0xE3: ticks = opcodeSet(4, DE.getLowRegister()); break;
	case 0xE4: ticks = opcodeSet(4, HL.getHighRegister()); break;
	case 0xE5: ticks = opcodeSet(4, HL.getLowRegister()); break;
	case 0xE6: ticks = opcodeSet(4, HL.getValue()); break;
	case 0xE7: ticks = opcodeSet(4, AF.getHighRegister()); break;
	case 0xE8: ticks = opcodeSet(5, BC.getHighRegister()); break;
	case 0xE9: ticks = opcodeSet(5, BC.getLowRegister()); break;
	case 0xEA: ticks = opcodeSet(5, DE.getHighRegister()); break;
	case 0xEB: ticks = opcodeSet(5, DE.getLowRegister()); break;
	case 0xEC: ticks = opcodeSet(5, HL.getHighRegister()); break;
	case 0xED: ticks = opcodeSet(5, HL.getLowRegister()); break;
	case 0xEE: ticks = opcodeSet(5, HL.getValue()); break;
	case 0xEF: ticks = opcodeSet(5, AF.getHighRegister()); break;
	case 0xF0: ticks = opcodeSet(6, BC.getHighRegister()); break;
	case 0xF1: ticks = opcodeSet(6, BC.getLowRegister()); break;
	case 0xF2: ticks = opcodeSet(6, DE.getHighRegister()); break;
	case 0xF3: ticks = opcodeSet(6, DE.getLowRegister()); break;
	case 0xF4: ticks = opcodeSet(6, HL.getHighRegister()); break;
	case 0xF5: ticks = opcodeSet(6, HL.getLowRegister()); break;
	case 0xF6: ticks = opcodeSet(6, HL.getValue()); break;
	case 0xF7: ticks = opcodeSet(6, AF.getHighRegister()); break;
	case 0xF8: ticks = opcodeSet(7, BC.getHighRegister()); break;
	case 0xF9: ticks = opcodeSet(7, BC.getLowRegister()); break;
	case 0xFA: ticks = opcodeSet(7, DE.getHighRegister()); break;
	case 0xFB: ticks = opcodeSet(7, DE.getLowRegister()); break;
	case 0xFC: ticks = opcodeSet(7, HL.getHighRegister()); break;
	case 0xFD: ticks = opcodeSet(7, HL.getLowRegister()); break;
	case 0xFE: ticks = opcodeSet(7, HL.getValue()); break;
	case 0xFF: ticks = opcodeSet(7, AF.getHighRegister()); break;
	}

	return ticks;
}

