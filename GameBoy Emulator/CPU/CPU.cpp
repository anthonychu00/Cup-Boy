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

void CPU::executeOperations() {//69905
	for (int ticks = 0; ticks < 69905;) {
		
		
		uint16_t PCValue = PC.getValue();
		uint8_t opcode = PCFetchByte();
		int ticksToExecute = executeOpcode(opcode, PCValue);
		//ticks += ticksToExecute;

		//std::cout << unsigned(opcode) << endl;
		//std::cout << (char)unsigned(mm.readAddress(0xFF01)) << endl;
		//std::cout << ticks << endl;

		updateTimer();
		checkInterruptRequests();//CPU checks interrupts at the end of each instruction, updates accordingly
	
	}
}

void CPU::checkInterruptRequests() {
	if (IME) {
		uint8_t interruptFlag = mm.readAddress(IFRegister);
		uint8_t interruptEnable = mm.readAddress(IERegister);

		for (int position = 0; position < 5; position++) {
			if (getBit(interruptFlag, position) && getBit(interruptEnable, position)) {
		
				opcodeStackPush(PC);//save PC address to jump back to after interrupt is done
				halt = false;

				interruptExecute(position);//interrupt vector is called and executed

				setBit(interruptFlag, position, 0); //reset bit in IF
				mm.writeAddress(IFRegister, interruptFlag);//corresponding bit is reset

				IME = false;//IME flag reset

				return;
			}
		}
	}
	//halt = false;
	
}

void CPU::interruptExecute(int vectorPosition) {
	//call interrupt vector
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
}

void CPU::updateTimer() {
	bool timerEnable = getBit(mm.readAddress(timerControl), 2);

	//0xFF04
	divTime += clock.p;
	if (divTime >= 256) { //divider increments every 256 CPU ticks
		uint8_t currentDivTime = mm.readAddress(dividerRegister);
		mm.writeAddress(dividerRegister, currentDivTime + 1);
		divTime -= 256;
	}

	//0xFF05 & 0xFF06
	if (timerEnable) {
		tima += clock.p;
		
		if (tima >= clockSpeed) { //tima increments every clockSpeed CPU ticks
			uint8_t currentTimerCounter = mm.readAddress(timerCounter);
			if (currentTimerCounter == 0xFF) { //timer overflow
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

	//perform action according to the opcode
	switch (opcode) {
		case 0x00: opcodeNOP(); break;
		case 0x01: opcodeLoadWord(BC); break;
		case 0x02: opcodeLoadAToMemory(BC.getValue()); break;
		case 0x03: opcodeIncrement(BC); break;
		case 0x04: opcodeIncrement(BC.getHighRegister()); break;
		case 0x05: opcodeDecrement(BC.getHighRegister()); break;
		case 0x06: opcodeLoadByte(BC.getHighRegister()); break;
		case 0x07: opcodeRLCA(); break;
		case 0x08: opcodeLoadSPToMemory(); break;
		case 0x09: opcodeAddHL(BC); break;
		case 0x0A: opcodeLoadMemoryToA(BC.getValue()); break;
		case 0x0B: opcodeDecrement(BC); break;
		case 0x0C: opcodeIncrement(BC.getLowRegister()); break;
		case 0x0D: opcodeDecrement(BC.getLowRegister()); break;
		case 0x0E: opcodeLoadByte(BC.getLowRegister()); break;
		case 0x0F: opcodeRRCA(); break;
		case 0x10: opcodeStop(); break;
		case 0x11: opcodeLoadWord(DE); break;
		case 0x12: opcodeLoadAToMemory(DE.getValue()); break;
		case 0x13: opcodeIncrement(DE); break;
		case 0x14: opcodeIncrement(DE.getHighRegister()); break;
		case 0x15: opcodeDecrement(DE.getHighRegister()); break;
		case 0x16: opcodeLoadByte(DE.getHighRegister()); break;
		case 0x17: opcodeRLA(); break;
		case 0x18: opcodeJR(); break;
		case 0x19: opcodeAddHL(DE); break;
		case 0x1A: opcodeLoadMemoryToA(DE.getValue()); break;
		case 0x1B: opcodeDecrement(DE); break;
		case 0x1C: opcodeIncrement(DE.getLowRegister()); break;
		case 0x1D: opcodeDecrement(DE.getLowRegister()); break;
		case 0x1E: opcodeLoadByte(DE.getLowRegister()); break;
		case 0x1F: opcodeRRA(); break;
		case 0x20: opcodeJR(Flag::NZ); break;
		case 0x21: opcodeLoadWord(HL); break;
		case 0x22: opcodeLoadAToMemoryInc(HL.getValue()); break;
		case 0x23: opcodeIncrement(HL); break;
		case 0x24: opcodeIncrement(HL.getHighRegister()); break;
		case 0x25: opcodeDecrement(HL.getHighRegister()); break;
		case 0x26: opcodeLoadByte(HL.getHighRegister()); break;
		case 0x27: opcodeDAA(); break;
		case 0x28: opcodeJR(Flag::Z); break;
		case 0x29: opcodeAddHL(HL); break;
		case 0x2A: opcodeLoadMemoryToAInc(HL.getValue()); break;
		case 0x2B: opcodeDecrement(HL); break;
		case 0x2C: opcodeIncrement(HL.getLowRegister()); break;
		case 0x2D: opcodeDecrement(HL.getLowRegister()); break;
		case 0x2E: opcodeLoadByte(HL.getLowRegister()); break;
		case 0x2F: opcodeCPL(); break;
		case 0x30: opcodeJR(Flag::NC); break;
		case 0x31: opcodeLoadWord(SP); break;
		case 0x32: opcodeLoadAToMemoryDec(HL.getValue()); break;
		case 0x33: opcodeIncrement(SP); break;
		case 0x34: opcodeIncrement(HL.getValue()); break; 
		case 0x35: opcodeDecrement(HL.getValue()); break; 
		case 0x36: opcodeLoadByte(HL.getValue()); break;
		case 0x37: opcodeSCF(); break;
		case 0x38: opcodeJR(Flag::C); break;
		case 0x39: opcodeAddHL(SP); break;
		case 0x3A: opcodeLoadMemoryToADec(HL.getValue()); break;
		case 0x3B: opcodeDecrement(SP); break;
		case 0x3C: opcodeIncrement(AF.getHighRegister()); break;
		case 0x3D: opcodeDecrement(AF.getHighRegister()); break;
		case 0x3E: opcodeLoadByte(AF.getHighRegister()); break;
		case 0x3F: opcodeCCF(); break;
		case 0x40: opcodeLoadByte(BC.getHighRegister(), BC.getHighRegister()); break;
		case 0x41: opcodeLoadByte(BC.getHighRegister(), BC.getLowRegister()); break;
		case 0x42: opcodeLoadByte(BC.getHighRegister(), DE.getHighRegister()); break;
		case 0x43: opcodeLoadByte(BC.getHighRegister(), DE.getLowRegister()); break;
		case 0x44: opcodeLoadByte(BC.getHighRegister(), HL.getHighRegister()); break;
		case 0x45: opcodeLoadByte(BC.getHighRegister(), HL.getLowRegister()); break;
		case 0x46: opcodeLoadByte(BC.getHighRegister(), HL.getValue()); break;
		case 0x47: opcodeLoadByte(BC.getHighRegister(), AF.getHighRegister()); break;
		case 0x48: opcodeLoadByte(BC.getLowRegister(), BC.getHighRegister()); break;
		case 0x49: opcodeLoadByte(BC.getLowRegister(), BC.getLowRegister()); break;
		case 0x4A: opcodeLoadByte(BC.getLowRegister(), DE.getHighRegister()); break;
		case 0x4B: opcodeLoadByte(BC.getLowRegister(), DE.getLowRegister()); break;
		case 0x4C: opcodeLoadByte(BC.getLowRegister(), HL.getHighRegister()); break;
		case 0x4D: opcodeLoadByte(BC.getLowRegister(), HL.getLowRegister()); break;
		case 0x4E: opcodeLoadByte(BC.getLowRegister(), HL.getValue()); break;
		case 0x4F: opcodeLoadByte(BC.getLowRegister(), AF.getHighRegister()); break;
		case 0x50: opcodeLoadByte(DE.getHighRegister(), BC.getHighRegister()); break;
		case 0x51: opcodeLoadByte(DE.getHighRegister(), BC.getLowRegister()); break;
		case 0x52: opcodeLoadByte(DE.getHighRegister(), DE.getHighRegister()); break;
		case 0x53: opcodeLoadByte(DE.getHighRegister(), DE.getLowRegister()); break;
		case 0x54: opcodeLoadByte(DE.getHighRegister(), HL.getHighRegister()); break;
		case 0x55: opcodeLoadByte(DE.getHighRegister(), HL.getLowRegister()); break;
		case 0x56: opcodeLoadByte(DE.getHighRegister(), HL.getValue()); break;
		case 0x57: opcodeLoadByte(DE.getHighRegister(), AF.getHighRegister()); break;
		case 0x58: opcodeLoadByte(DE.getLowRegister(), BC.getHighRegister()); break;
		case 0x59: opcodeLoadByte(DE.getLowRegister(), BC.getLowRegister()); break;
		case 0x5A: opcodeLoadByte(DE.getLowRegister(), DE.getHighRegister()); break;
		case 0x5B: opcodeLoadByte(DE.getLowRegister(), DE.getLowRegister()); break;
		case 0x5C: opcodeLoadByte(DE.getLowRegister(), HL.getHighRegister()); break;
		case 0x5D: opcodeLoadByte(DE.getLowRegister(), HL.getLowRegister()); break;
		case 0x5E: opcodeLoadByte(DE.getLowRegister(), HL.getValue()); break;
		case 0x5F: opcodeLoadByte(DE.getLowRegister(), AF.getHighRegister()); break;
		case 0x60: opcodeLoadByte(HL.getHighRegister(), BC.getHighRegister()); break;
		case 0x61: opcodeLoadByte(HL.getHighRegister(), BC.getLowRegister()); break;
		case 0x62: opcodeLoadByte(HL.getHighRegister(), DE.getHighRegister()); break;
		case 0x63: opcodeLoadByte(HL.getHighRegister(), DE.getLowRegister()); break;
		case 0x64: opcodeLoadByte(HL.getHighRegister(), HL.getHighRegister()); break;
		case 0x65: opcodeLoadByte(HL.getHighRegister(), HL.getLowRegister()); break;
		case 0x66: opcodeLoadByte(HL.getHighRegister(), HL.getValue()); break;
		case 0x67: opcodeLoadByte(HL.getHighRegister(), AF.getHighRegister()); break;
		case 0x68: opcodeLoadByte(HL.getLowRegister(), BC.getHighRegister()); break;
		case 0x69: opcodeLoadByte(HL.getLowRegister(), BC.getLowRegister()); break;
		case 0x6A: opcodeLoadByte(HL.getLowRegister(), DE.getHighRegister()); break;
		case 0x6B: opcodeLoadByte(HL.getLowRegister(), DE.getLowRegister()); break;
		case 0x6C: opcodeLoadByte(HL.getLowRegister(), HL.getHighRegister()); break;
		case 0x6D: opcodeLoadByte(HL.getLowRegister(), HL.getLowRegister()); break;
		case 0x6E: opcodeLoadByte(HL.getLowRegister(), HL.getValue()); break;
		case 0x6F: opcodeLoadByte(HL.getLowRegister(), AF.getHighRegister()); break;
		case 0x70: opcodeLoadByte(HL.getValue(), BC.getHighRegister()); break;
		case 0x71: opcodeLoadByte(HL.getValue(), BC.getLowRegister()); break;
		case 0x72: opcodeLoadByte(HL.getValue(), DE.getHighRegister()); break;
		case 0x73: opcodeLoadByte(HL.getValue(), DE.getLowRegister()); break;
		case 0x74: opcodeLoadByte(HL.getValue(), HL.getHighRegister()); break;
		case 0x75: opcodeLoadByte(HL.getValue(), HL.getLowRegister()); break;
		case 0x76: opcodeHalt(); break;
		case 0x77: opcodeLoadByte(HL.getValue(), AF.getHighRegister()); break;
		case 0x78: opcodeLoadByte(AF.getHighRegister(), BC.getHighRegister()); break;
		case 0x79: opcodeLoadByte(AF.getHighRegister(), BC.getLowRegister()); break;
		case 0x7A: opcodeLoadByte(AF.getHighRegister(), DE.getHighRegister()); break;
		case 0x7B: opcodeLoadByte(AF.getHighRegister(), DE.getLowRegister()); break;
		case 0x7C: opcodeLoadByte(AF.getHighRegister(), HL.getHighRegister()); break;
		case 0x7D: opcodeLoadByte(AF.getHighRegister(), HL.getLowRegister()); break;
		case 0x7E: opcodeLoadByte(AF.getHighRegister(), HL.getValue()); break;
		case 0x7F: opcodeLoadByte(AF.getHighRegister(), AF.getHighRegister()); break;
		case 0x80: opcodeAddA(BC.getHighRegister()); break;
		case 0x81: opcodeAddA(BC.getLowRegister()); break;
		case 0x82: opcodeAddA(DE.getHighRegister()); break;
		case 0x83: opcodeAddA(DE.getLowRegister()); break;
		case 0x84: opcodeAddA(HL.getHighRegister()); break;
		case 0x85: opcodeAddA(HL.getLowRegister()); break;
		case 0x86: opcodeAddA(HL.getValue()); break;
		case 0x87: opcodeAddA(AF.getHighRegister()); break;
		case 0x88: opcodeADC(BC.getHighRegister()); break;
		case 0x89: opcodeADC(BC.getLowRegister()); break;
		case 0x8A: opcodeADC(DE.getHighRegister()); break;
		case 0x8B: opcodeADC(DE.getLowRegister()); break;
		case 0x8C: opcodeADC(HL.getHighRegister()); break;
		case 0x8D: opcodeADC(HL.getLowRegister()); break;
		case 0x8E: opcodeADC(HL.getValue()); break;
		case 0x8F: opcodeADC(AF.getHighRegister()); break;
		case 0x90: opcodeSubA(BC.getHighRegister()); break;
		case 0x91: opcodeSubA(BC.getLowRegister()); break;
		case 0x92: opcodeSubA(DE.getHighRegister()); break;
		case 0x93: opcodeSubA(DE.getLowRegister()); break;
		case 0x94: opcodeSubA(HL.getHighRegister()); break;
		case 0x95: opcodeSubA(HL.getLowRegister()); break;
		case 0x96: opcodeSubA(HL.getValue()); break;
		case 0x97: opcodeSubA(AF.getHighRegister()); break;
		case 0x98: opcodeSBC(BC.getHighRegister()); break;
		case 0x99: opcodeSBC(BC.getLowRegister()); break;
		case 0x9A: opcodeSBC(DE.getHighRegister()); break;
		case 0x9B: opcodeSBC(DE.getLowRegister()); break;
		case 0x9C: opcodeSBC(HL.getHighRegister()); break;
		case 0x9D: opcodeSBC(HL.getLowRegister()); break;
		case 0x9E: opcodeSBC(HL.getValue()); break;
		case 0x9F: opcodeSBC(AF.getHighRegister()); break;
		case 0xA0: opcodeAnd(BC.getHighRegister()); break;
		case 0xA1: opcodeAnd(BC.getLowRegister()); break;
		case 0xA2: opcodeAnd(DE.getHighRegister()); break;
		case 0xA3: opcodeAnd(DE.getLowRegister()); break;
		case 0xA4: opcodeAnd(HL.getHighRegister()); break;
		case 0xA5: opcodeAnd(HL.getLowRegister()); break;
		case 0xA6: opcodeAnd(HL.getValue()); break;
		case 0xA7: opcodeAnd(AF.getHighRegister()); break;
		case 0xA8: opcodeXOR(BC.getHighRegister()); break;
		case 0xA9: opcodeXOR(BC.getLowRegister()); break;
		case 0xAA: opcodeXOR(DE.getHighRegister()); break;
		case 0xAB: opcodeXOR(DE.getLowRegister()); break;
		case 0xAC: opcodeXOR(HL.getHighRegister()); break;
		case 0xAD: opcodeXOR(HL.getLowRegister()); break;
		case 0xAE: opcodeXOR(HL.getValue()); break;
		case 0xAF: opcodeXOR(AF.getHighRegister()); break;
		case 0xB0: opcodeOr(BC.getHighRegister()); break;
		case 0xB1: opcodeOr(BC.getLowRegister()); break;
		case 0xB2: opcodeOr(DE.getHighRegister()); break;
		case 0xB3: opcodeOr(DE.getLowRegister()); break;
		case 0xB4: opcodeOr(HL.getHighRegister()); break;
		case 0xB5: opcodeOr(HL.getLowRegister()); break;
		case 0xB6: opcodeOr(HL.getValue()); break;
		case 0xB7: opcodeOr(AF.getHighRegister()); break;
		case 0xB8: opcodeCP(BC.getHighRegister()); break;
		case 0xB9: opcodeCP(BC.getLowRegister()); break;
		case 0xBA: opcodeCP(DE.getHighRegister()); break;
		case 0xBB: opcodeCP(DE.getLowRegister()); break;
		case 0xBC: opcodeCP(HL.getHighRegister()); break;
		case 0xBD: opcodeCP(HL.getLowRegister()); break;
		case 0xBE: opcodeCP(HL.getValue()); break;
		case 0xBF: opcodeCP(AF.getHighRegister()); break;
		case 0xC0: opcodeRet(Flag::NZ); break;
		case 0xC1: opcodeStackPop(BC); break;
		case 0xC2: opcodeJP(Flag::NZ); break;
		case 0xC3: opcodeJP(); break;
		case 0xC4: opcodeCall(Flag::NZ); break;
		case 0xC5: opcodeStackPush(BC); break;
		case 0xC6: opcodeAddA(); break;
		case 0xC7: opcodeRST(0x00); break;
		case 0xC8: opcodeRet(Flag::Z); break;
		case 0xC9: opcodeRet(); break;
		case 0xCA: opcodeJP(Flag::Z); break;
		case 0xCC: opcodeCall(Flag::Z); break;
		case 0xCD: opcodeCall(); break;
		case 0xCE: opcodeADC(); break;
		case 0xCF: opcodeRST(0x08); break;
		case 0xD0: opcodeRet(Flag::NC); break;
		case 0xD1: opcodeStackPop(DE); break;
		case 0xD2: opcodeJP(Flag::NC); break;
		case 0xD3: break; //no operation
		case 0xD4: opcodeCall(Flag::NC); break;
		case 0xD5: opcodeStackPush(DE); break;
		case 0xD6: opcodeSubA(); break;
		case 0xD7: opcodeRST(0x10); break;
		case 0xD8: opcodeRet(Flag::C); break;
		case 0xD9: opcodeRetI(); break;
		case 0xDA: opcodeJP(Flag::C); break;
		case 0xDB: break; //no operation
		case 0xDC: opcodeCall(Flag::C); break;
		case 0xDD: break; //no operation
		case 0xDE: opcodeSBC(); break;
		case 0xDF: opcodeRST(0x18); break;
		case 0xE0: opcodeLoadAToMemory(); break;
		case 0xE1: opcodeStackPop(HL); break;
		case 0xE2: opcodeLoadAToMemory(BC.getLowRegister()); break;
		case 0xE3: break; //no operation
		case 0xE4: break; //no operation
		case 0xE5: opcodeStackPush(HL); break;
		case 0xE6: opcodeAnd(); break;
		case 0xE7: opcodeRST(0x20); break;
		case 0xE8: opcodeAddSP(); break;
		case 0xE9: opcodeJPHL(); break;
		case 0xEA: opcodeLoadAToMemory16(); break;
		case 0xEB: break; //no operation
		case 0xEC: break; //no operation
		case 0xED: break; //no operation
		case 0xEE: opcodeXOR(); break;
		case 0xEF: opcodeRST(0x28); break;
		case 0xF0: opcodeLoadMemoryToA(); break;
		case 0xF1: opcodeStackPop(AF, true); break;
		case 0xF2: opcodeLoadMemoryToA(BC.getLowRegister()); break;
		case 0xF3: opcodeDI(); break;
		case 0xF4: break; //no operation
		case 0xF5: opcodeStackPush(AF); break;
		case 0xF6: opcodeOr(); break;
		case 0xF7: opcodeRST(0x30); break;
		case 0xF8: opcodeLoadHLSP(); break;
		case 0xF9: opcodeLoadSPHL(); break;
		case 0xFA: opcodeLoadMemoryToA16(); break;
		case 0xFB: opcodeEI(); break;
		case 0xFC: break; //no operation
		case 0xFD: break; //no operation
		case 0xFE: opcodeCP(); break;
		case 0xFF: opcodeRST(0x38); break;
	}

	return clock.p;
	
}

int CPU::executePrefixedOpcode(const uint8_t opcode, uint16_t PCValue) {
	switch (opcode) {
	case 0x00: opcodeRLC(BC.getHighRegister()); break;
	case 0x01: opcodeRLC(BC.getLowRegister()); break;
	case 0x02: opcodeRLC(DE.getHighRegister()); break;
	case 0x03: opcodeRLC(DE.getLowRegister()); break;
	case 0x04: opcodeRLC(HL.getHighRegister()); break;
	case 0x05: opcodeRLC(HL.getLowRegister()); break;
	case 0x06: opcodeRLC(HL.getValue()); break;
	case 0x07: opcodeRLC(AF.getHighRegister()); break;
	case 0x08: opcodeRRC(BC.getHighRegister()); break;
	case 0x09: opcodeRRC(BC.getLowRegister()); break;
	case 0x0A: opcodeRRC(DE.getHighRegister()); break;
	case 0x0B: opcodeRRC(DE.getLowRegister()); break;
	case 0x0C: opcodeRRC(HL.getHighRegister()); break;
	case 0x0D: opcodeRRC(HL.getLowRegister()); break;
	case 0x0E: opcodeRRC(HL.getValue()); break;
	case 0x0F: opcodeRRC(AF.getHighRegister()); break;
	case 0x10: opcodeRL(BC.getHighRegister()); break;
	case 0x11: opcodeRL(BC.getLowRegister()); break;
	case 0x12: opcodeRL(DE.getHighRegister()); break;
	case 0x13: opcodeRL(DE.getLowRegister()); break;
	case 0x14: opcodeRL(HL.getHighRegister()); break;
	case 0x15: opcodeRL(HL.getLowRegister()); break;
	case 0x16: opcodeRL(HL.getValue()); break;
	case 0x17: opcodeRL(AF.getHighRegister()); break;
	case 0x18: opcodeRR(BC.getHighRegister()); break;
	case 0x19: opcodeRR(BC.getLowRegister()); break;
	case 0x1A: opcodeRR(DE.getHighRegister()); break;
	case 0x1B: opcodeRR(DE.getLowRegister()); break;
	case 0x1C: opcodeRR(HL.getHighRegister()); break;
	case 0x1D: opcodeRR(HL.getLowRegister()); break;
	case 0x1E: opcodeRR(HL.getValue()); break;
	case 0x1F: opcodeRR(AF.getHighRegister()); break;
	case 0x20: opcodeSLA(BC.getHighRegister()); break;
	case 0x21: opcodeSLA(BC.getLowRegister()); break;
	case 0x22: opcodeSLA(DE.getHighRegister()); break;
	case 0x23: opcodeSLA(DE.getLowRegister()); break;
	case 0x24: opcodeSLA(HL.getHighRegister()); break;
	case 0x25: opcodeSLA(HL.getLowRegister()); break;
	case 0x26: opcodeSLA(HL.getValue()); break;
	case 0x27: opcodeSLA(AF.getHighRegister()); break;
	case 0x28: opcodeSRA(BC.getHighRegister()); break;
	case 0x29: opcodeSRA(BC.getLowRegister()); break;
	case 0x2A: opcodeSRA(DE.getHighRegister()); break;
	case 0x2B: opcodeSRA(DE.getLowRegister()); break;
	case 0x2C: opcodeSRA(HL.getHighRegister()); break;
	case 0x2D: opcodeSRA(HL.getLowRegister()); break;
	case 0x2E: opcodeSRA(HL.getValue()); break;
	case 0x2F: opcodeSRA(AF.getHighRegister()); break;
	case 0x30: opcodeSwap(BC.getHighRegister()); break;
	case 0x31: opcodeSwap(BC.getLowRegister()); break;
	case 0x32: opcodeSwap(DE.getHighRegister()); break;
	case 0x33: opcodeSwap(DE.getLowRegister()); break;
	case 0x34: opcodeSwap(HL.getHighRegister()); break;
	case 0x35: opcodeSwap(HL.getLowRegister()); break;
	case 0x36: opcodeSwap(HL.getValue()); break;
	case 0x37: opcodeSwap(AF.getHighRegister()); break;
	case 0x38: opcodeSRL(BC.getHighRegister()); break;
	case 0x39: opcodeSRL(BC.getLowRegister()); break;
	case 0x3A: opcodeSRL(DE.getHighRegister()); break;
	case 0x3B: opcodeSRL(DE.getLowRegister()); break;
	case 0x3C: opcodeSRL(HL.getHighRegister()); break;
	case 0x3D: opcodeSRL(HL.getLowRegister()); break;
	case 0x3E: opcodeSRL(HL.getValue()); break;
	case 0x3F: opcodeSRL(AF.getHighRegister()); break;
	case 0x40: opcodeBit(0, BC.getHighRegister()); break;
	case 0x41: opcodeBit(0, BC.getLowRegister()); break;
	case 0x42: opcodeBit(0, DE.getHighRegister()); break;
	case 0x43: opcodeBit(0, DE.getLowRegister()); break;
	case 0x44: opcodeBit(0, HL.getHighRegister()); break;
	case 0x45: opcodeBit(0, HL.getLowRegister()); break;
	case 0x46: opcodeBit(0, HL.getValue()); break;
	case 0x47: opcodeBit(0, AF.getHighRegister()); break;
	case 0x48: opcodeBit(1, BC.getHighRegister()); break;
	case 0x49: opcodeBit(1, BC.getLowRegister()); break;
	case 0x4A: opcodeBit(1, DE.getHighRegister()); break;
	case 0x4B: opcodeBit(1, DE.getLowRegister()); break;
	case 0x4C: opcodeBit(1, HL.getHighRegister()); break;
	case 0x4D: opcodeBit(1, HL.getLowRegister()); break;
	case 0x4E: opcodeBit(1, HL.getValue()); break;
	case 0x4F: opcodeBit(1, AF.getHighRegister()); break;
	case 0x50: opcodeBit(2, BC.getHighRegister()); break;
	case 0x51: opcodeBit(2, BC.getLowRegister()); break;
	case 0x52: opcodeBit(2, DE.getHighRegister()); break;
	case 0x53: opcodeBit(2, DE.getLowRegister()); break;
	case 0x54: opcodeBit(2, HL.getHighRegister()); break;
	case 0x55: opcodeBit(2, HL.getLowRegister()); break;
	case 0x56: opcodeBit(2, HL.getValue()); break;
	case 0x57: opcodeBit(2, AF.getHighRegister()); break;
	case 0x58: opcodeBit(3, BC.getHighRegister()); break;
	case 0x59: opcodeBit(3, BC.getLowRegister()); break;
	case 0x5A: opcodeBit(3, DE.getHighRegister()); break;
	case 0x5B: opcodeBit(3, DE.getLowRegister()); break;
	case 0x5C: opcodeBit(3, HL.getHighRegister()); break;
	case 0x5D: opcodeBit(3, HL.getLowRegister()); break;
	case 0x5E: opcodeBit(3, HL.getValue()); break;
	case 0x5F: opcodeBit(3, AF.getHighRegister()); break;
	case 0x60: opcodeBit(4, BC.getHighRegister()); break;
	case 0x61: opcodeBit(4, BC.getLowRegister()); break;
	case 0x62: opcodeBit(4, DE.getHighRegister()); break;
	case 0x63: opcodeBit(4, DE.getLowRegister()); break;
	case 0x64: opcodeBit(4, HL.getHighRegister()); break;
	case 0x65: opcodeBit(4, HL.getLowRegister()); break;
	case 0x66: opcodeBit(4, HL.getValue()); break;
	case 0x67: opcodeBit(4, AF.getHighRegister()); break;
	case 0x68: opcodeBit(5, BC.getHighRegister()); break;
	case 0x69: opcodeBit(5, BC.getLowRegister()); break;
	case 0x6A: opcodeBit(5, DE.getHighRegister()); break;
	case 0x6B: opcodeBit(5, DE.getLowRegister()); break;
	case 0x6C: opcodeBit(5, HL.getHighRegister()); break;
	case 0x6D: opcodeBit(5, HL.getLowRegister()); break;
	case 0x6E: opcodeBit(5, HL.getValue()); break;
	case 0x6F: opcodeBit(5, AF.getHighRegister()); break;
	case 0x70: opcodeBit(6, BC.getHighRegister()); break;
	case 0x71: opcodeBit(6, BC.getLowRegister()); break;
	case 0x72: opcodeBit(6, DE.getHighRegister()); break;
	case 0x73: opcodeBit(6, DE.getLowRegister()); break;
	case 0x74: opcodeBit(6, HL.getHighRegister()); break;
	case 0x75: opcodeBit(6, HL.getLowRegister()); break;
	case 0x76: opcodeBit(6, HL.getValue()); break;
	case 0x77: opcodeBit(6, AF.getHighRegister()); break;
	case 0x78: opcodeBit(7, BC.getHighRegister()); break;
	case 0x79: opcodeBit(7, BC.getLowRegister()); break;
	case 0x7A: opcodeBit(7, DE.getHighRegister()); break;
	case 0x7B: opcodeBit(7, DE.getLowRegister()); break;
	case 0x7C: opcodeBit(7, HL.getHighRegister()); break;
	case 0x7D: opcodeBit(7, HL.getLowRegister()); break;
	case 0x7E: opcodeBit(7, HL.getValue()); break;
	case 0x7F: opcodeBit(7, AF.getHighRegister()); break;
	case 0x80: opcodeReset(0, BC.getHighRegister()); break;
	case 0x81: opcodeReset(0, BC.getLowRegister()); break;
	case 0x82: opcodeReset(0, DE.getHighRegister()); break;
	case 0x83: opcodeReset(0, DE.getLowRegister()); break;
	case 0x84: opcodeReset(0, HL.getHighRegister()); break;
	case 0x85: opcodeReset(0, HL.getLowRegister()); break;
	case 0x86: opcodeReset(0, HL.getValue()); break;
	case 0x87: opcodeReset(0, AF.getHighRegister()); break;
	case 0x88: opcodeReset(1, BC.getHighRegister()); break;
	case 0x89: opcodeReset(1, BC.getLowRegister()); break;
	case 0x8A: opcodeReset(1, DE.getHighRegister()); break;
	case 0x8B: opcodeReset(1, DE.getLowRegister()); break;
	case 0x8C: opcodeReset(1, HL.getHighRegister()); break;
	case 0x8D: opcodeReset(1, HL.getLowRegister()); break;
	case 0x8E: opcodeReset(1, HL.getValue()); break;
	case 0x8F: opcodeReset(1, AF.getHighRegister()); break;
	case 0x90: opcodeReset(2, BC.getHighRegister()); break;
	case 0x91: opcodeReset(2, BC.getLowRegister()); break;
	case 0x92: opcodeReset(2, DE.getHighRegister()); break;
	case 0x93: opcodeReset(2, DE.getLowRegister()); break;
	case 0x94: opcodeReset(2, HL.getHighRegister()); break;
	case 0x95: opcodeReset(2, HL.getLowRegister()); break;
	case 0x96: opcodeReset(2, HL.getValue()); break;
	case 0x97: opcodeReset(2, AF.getHighRegister()); break;
	case 0x98: opcodeReset(3, BC.getHighRegister()); break;
	case 0x99: opcodeReset(3, BC.getLowRegister()); break;
	case 0x9A: opcodeReset(3, DE.getHighRegister()); break;
	case 0x9B: opcodeReset(3, DE.getLowRegister()); break;
	case 0x9C: opcodeReset(3, HL.getHighRegister()); break;
	case 0x9D: opcodeReset(3, HL.getLowRegister()); break;
	case 0x9E: opcodeReset(3, HL.getValue()); break;
	case 0x9F: opcodeReset(3, AF.getHighRegister()); break;
	case 0xA0: opcodeReset(4, BC.getHighRegister()); break;
	case 0xA1: opcodeReset(4, BC.getLowRegister()); break;
	case 0xA2: opcodeReset(4, DE.getHighRegister()); break;
	case 0xA3: opcodeReset(4, DE.getLowRegister()); break;
	case 0xA4: opcodeReset(4, HL.getHighRegister()); break;
	case 0xA5: opcodeReset(4, HL.getLowRegister()); break;
	case 0xA6: opcodeReset(4, HL.getValue()); break;
	case 0xA7: opcodeReset(4, AF.getHighRegister()); break;
	case 0xA8: opcodeReset(5, BC.getHighRegister()); break;
	case 0xA9: opcodeReset(5, BC.getLowRegister()); break;
	case 0xAA: opcodeReset(5, DE.getHighRegister()); break;
	case 0xAB: opcodeReset(5, DE.getLowRegister()); break;
	case 0xAC: opcodeReset(5, HL.getHighRegister()); break;
	case 0xAD: opcodeReset(5, HL.getLowRegister()); break;
	case 0xAE: opcodeReset(5, HL.getValue()); break;
	case 0xAF: opcodeReset(5, AF.getHighRegister()); break;
	case 0xB0: opcodeReset(6, BC.getHighRegister()); break;
	case 0xB1: opcodeReset(6, BC.getLowRegister()); break;
	case 0xB2: opcodeReset(6, DE.getHighRegister()); break;
	case 0xB3: opcodeReset(6, DE.getLowRegister()); break;
	case 0xB4: opcodeReset(6, HL.getHighRegister()); break;
	case 0xB5: opcodeReset(6, HL.getLowRegister()); break;
	case 0xB6: opcodeReset(6, HL.getValue()); break;
	case 0xB7: opcodeReset(6, AF.getHighRegister()); break;
	case 0xB8: opcodeReset(7, BC.getHighRegister()); break;
	case 0xB9: opcodeReset(7, BC.getLowRegister()); break;
	case 0xBA: opcodeReset(7, DE.getHighRegister()); break;
	case 0xBB: opcodeReset(7, DE.getLowRegister()); break;
	case 0xBC: opcodeReset(7, HL.getHighRegister()); break;
	case 0xBD: opcodeReset(7, HL.getLowRegister()); break;
	case 0xBE: opcodeReset(7, HL.getValue()); break;
	case 0xBF: opcodeReset(7, AF.getHighRegister()); break;
	case 0xC0: opcodeSet(0, BC.getHighRegister()); break;
	case 0xC1: opcodeSet(0, BC.getLowRegister()); break;
	case 0xC2: opcodeSet(0, DE.getHighRegister()); break;
	case 0xC3: opcodeSet(0, DE.getLowRegister()); break;
	case 0xC4: opcodeSet(0, HL.getHighRegister()); break;
	case 0xC5: opcodeSet(0, HL.getLowRegister()); break;
	case 0xC6: opcodeSet(0, HL.getValue()); break;
	case 0xC7: opcodeSet(0, AF.getHighRegister()); break;
	case 0xC8: opcodeSet(1, BC.getHighRegister()); break;
	case 0xC9: opcodeSet(1, BC.getLowRegister()); break;
	case 0xCA: opcodeSet(1, DE.getHighRegister()); break;
	case 0xCB: opcodeSet(1, DE.getLowRegister()); break;
	case 0xCC: opcodeSet(1, HL.getHighRegister()); break;
	case 0xCD: opcodeSet(1, HL.getLowRegister()); break;
	case 0xCE: opcodeSet(1, HL.getValue()); break;
	case 0xCF: opcodeSet(1, AF.getHighRegister()); break;
	case 0xD0: opcodeSet(2, BC.getHighRegister()); break;
	case 0xD1: opcodeSet(2, BC.getLowRegister()); break;
	case 0xD2: opcodeSet(2, DE.getHighRegister()); break;
	case 0xD3: opcodeSet(2, DE.getLowRegister()); break;
	case 0xD4: opcodeSet(2, HL.getHighRegister()); break;
	case 0xD5: opcodeSet(2, HL.getLowRegister()); break;
	case 0xD6: opcodeSet(2, HL.getValue()); break;
	case 0xD7: opcodeSet(2, AF.getHighRegister()); break;
	case 0xD8: opcodeSet(3, BC.getHighRegister()); break;
	case 0xD9: opcodeSet(3, BC.getLowRegister()); break;
	case 0xDA: opcodeSet(3, DE.getHighRegister()); break;
	case 0xDB: opcodeSet(3, DE.getLowRegister()); break;
	case 0xDC: opcodeSet(3, HL.getHighRegister()); break;
	case 0xDD: opcodeSet(3, HL.getLowRegister()); break;
	case 0xDE: opcodeSet(3, HL.getValue()); break;
	case 0xDF: opcodeSet(3, AF.getHighRegister()); break;
	case 0xE0: opcodeSet(4, BC.getHighRegister()); break;
	case 0xE1: opcodeSet(4, BC.getLowRegister()); break;
	case 0xE2: opcodeSet(4, DE.getHighRegister()); break;
	case 0xE3: opcodeSet(4, DE.getLowRegister()); break;
	case 0xE4: opcodeSet(4, HL.getHighRegister()); break;
	case 0xE5: opcodeSet(4, HL.getLowRegister()); break;
	case 0xE6: opcodeSet(4, HL.getValue()); break;
	case 0xE7: opcodeSet(4, AF.getHighRegister()); break;
	case 0xE8: opcodeSet(5, BC.getHighRegister()); break;
	case 0xE9: opcodeSet(5, BC.getLowRegister()); break;
	case 0xEA: opcodeSet(5, DE.getHighRegister()); break;
	case 0xEB: opcodeSet(5, DE.getLowRegister()); break;
	case 0xEC: opcodeSet(5, HL.getHighRegister()); break;
	case 0xED: opcodeSet(5, HL.getLowRegister()); break;
	case 0xEE: opcodeSet(5, HL.getValue()); break;
	case 0xEF: opcodeSet(5, AF.getHighRegister()); break;
	case 0xF0: opcodeSet(6, BC.getHighRegister()); break;
	case 0xF1: opcodeSet(6, BC.getLowRegister()); break;
	case 0xF2: opcodeSet(6, DE.getHighRegister()); break;
	case 0xF3: opcodeSet(6, DE.getLowRegister()); break;
	case 0xF4: opcodeSet(6, HL.getHighRegister()); break;
	case 0xF5: opcodeSet(6, HL.getLowRegister()); break;
	case 0xF6: opcodeSet(6, HL.getValue()); break;
	case 0xF7: opcodeSet(6, AF.getHighRegister()); break;
	case 0xF8: opcodeSet(7, BC.getHighRegister()); break;
	case 0xF9: opcodeSet(7, BC.getLowRegister()); break;
	case 0xFA: opcodeSet(7, DE.getHighRegister()); break;
	case 0xFB: opcodeSet(7, DE.getLowRegister()); break;
	case 0xFC: opcodeSet(7, HL.getHighRegister()); break;
	case 0xFD: opcodeSet(7, HL.getLowRegister()); break;
	case 0xFE: opcodeSet(7, HL.getValue()); break;
	case 0xFF: opcodeSet(7, AF.getHighRegister()); break;
	}

	return clock.p;
}

