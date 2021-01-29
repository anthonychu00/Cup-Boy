#include "CPU.h"

void CPU::executeOperations() {
	for (int ticks = 0; ticks <= 69905;) {
		uint16_t PCValue = PC.getValue();
		//uint8_t opcode = PCFetchByte();
		//int ticksToExecute = executeOpcode(opcode, PCValue);
		//ticks += ticksToExecute;
	}
}

uint8_t CPU::PCFetchByte() {
	//uint8_t byte = fetch byte from memory address
	PC.increment();
	//return byte;
}

uint16_t CPU::PCFetchWord() {
	//lower byte is first in memory from the PC according to timings
	uint8_t low = PCFetchByte();
	uint8_t high = PCFetchByte();

	return ((uint16_t)low << 8) | high;
}

//pop 2 bytes from SP and load into 2 registers
void CPU::stackPop(Word& r) {
	//get lower byte memory from address in memory pointed to by SP
	//SP.increment();
	//get higher byte memory from address
	//SP.increment();

	//word = ((uint16_t)low.getByte() << 8) | high.getByte();
	//set register to popped val (jumps if PC)
}

void CPU::stackPush(const Word& r) {
	//decrement SP
	//write r -> high to address pointed to by SP
	//decrement
	//write r-> low to new address pointed by SP
}


bool CPU::checkCondition(Condition c) {
	bool result = 0;

	switch (c) {
	case Condition::NZ:
		result = !F.getZeroFlag();
		break;
	case Condition::Z:
		result = F.getZeroFlag();
		break;
	case Condition::NC:
		result = !F.getCarryFlag();
		break;
	case Condition::C:
		result = F.getCarryFlag();
		break;

	}

	return result;
}

int CPU::executeOpcode(uint8_t opcode, uint16_t PCValue) {
	if (opcode == 0xCB) {
		return executePrefixedOpcode(PCFetchByte(), PCValue);
	}
	//perform action according to the opcode
	switch (opcode) {
		case 0x00: opcodeNOP(); break;
		case 0x01: opcodeLoadWord(BC); break;
		case 0x02: opcodeLoadAToMemory(BC); break;
		case 0x03: opcodeIncrement(BC); break;
		case 0x04: opcodeIncrement(BC.getHighRegister()); break;
		case 0x05: opcodeDecrement(BC.getHighRegister()); break;
		case 0x06: opcodeLoadByte(BC.getHighRegister()); break;
		case 0x07: opcodeRLCA(); break;
		//case 0x08: opcodeLoadWord(Address, SP); break;
		case 0x09: opcodeAddHL(BC); break;
		case 0x0A: opcodeLoadMemoryToA(BC); break;
		case 0x0B: opcodeDecrement(BC); break;
		case 0x0C: opcodeIncrement(BC.getLowRegister()); break;
		case 0x0D: opcodeDecrement(BC.getLowRegister()); break;
		case 0x0E: opcodeLoadByte(BC.getLowRegister()); break;
		case 0x0F: opcodeRRCA(); break;
		case 0x11: opcodeLoadWord(DE); break;
		case 0x12: opcodeLoadAToMemory(DE); break;
		case 0x13: opcodeIncrement(DE); break;
		case 0x14: opcodeIncrement(DE.getHighRegister()); break;
		case 0x15: opcodeDecrement(DE.getHighRegister()); break;
		case 0x16: opcodeLoadByte(DE.getHighRegister()); break;
		case 0x17: opcodeRLA(); break;
		case 0x18: opcodeJR(); break;
		case 0x19: opcodeAddHL(DE); break;
		case 0x1A: opcodeLoadMemoryToA(DE); break;
		case 0x1B: opcodeDecrement(DE); break;
		case 0x1C: opcodeIncrement(DE.getLowRegister()); break;
		case 0x1D: opcodeDecrement(DE.getLowRegister()); break;
		case 0x1E: opcodeLoadByte(DE.getLowRegister()); break;
		case 0x1F: opcodeRRA(); break;
		case 0x21: opcodeLoadWord(HL); break;
		case 0x22: opcodeLoadAToMemory(HL); break;//examine difference
		case 0x23: opcodeIncrement(HL); break;
		case 0x24: opcodeIncrement(HL.getHighRegister()); break;
		case 0x25: opcodeDecrement(HL.getHighRegister()); break;
		case 0x26: opcodeLoadByte(HL.getHighRegister()); break;
		case 0x27: opcodeDAA(); break;
		case 0x28: opcodeJR(Condition::Z); break;
		case 0x29: opcodeAddHL(HL); break;
		case 0x2A: opcodeLoadMemoryToA(HL); break;//examine difference
		case 0x2B: opcodeDecrement(HL); break;
		case 0x2C: opcodeIncrement(HL.getLowRegister()); break;
		case 0x2D: opcodeDecrement(HL.getLowRegister()); break;
		case 0x2E: opcodeLoadByte(HL.getLowRegister()); break;
		case 0x2F: opcodeCPL(); break;
		case 0x31: opcodeLoadWord(SP); break;
		case 0x32: opcodeLoadAToMemory(HL); break;//examine difference
		case 0x33: opcodeIncrement(SP); break;
		//case 0x34: opcodeIncrement(); break; <- increment memory address pointed to by HL
		//case 0x35: opcodeIncrement(); break; <- decrement memory address pointed to by HL
		//case 0x36: opcodeLoadByte(Address) <- load byte to HL address in memory
		case 0x37: opcodeSCF(); break;
		case 0x38: opcodeJR(Condition::C); break;
		case 0x39: opcodeAddHL(SP); break;
		case 0x3A: opcodeLoadMemoryToA(HL); break;//examine difference
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
		//case 0x46: opcodeLoadByte(BC.getHighRegister(), HL.getValue()); break;
		case 0x47: opcodeLoadByte(BC.getHighRegister(), AF.getHighRegister()); break;
		case 0x48: opcodeLoadByte(BC.getLowRegister(), BC.getHighRegister()); break;
		case 0x49: opcodeLoadByte(BC.getLowRegister(), BC.getLowRegister()); break;
		case 0x4A: opcodeLoadByte(BC.getLowRegister(), DE.getHighRegister()); break;
		case 0x4B: opcodeLoadByte(BC.getLowRegister(), DE.getLowRegister()); break;
		case 0x4C: opcodeLoadByte(BC.getLowRegister(), HL.getHighRegister()); break;
		case 0x4D: opcodeLoadByte(BC.getLowRegister(), HL.getLowRegister()); break;
		//case 0x4E: opcodeLoadByte(BC.getLowRegister(), HL.getValue()); break;
		case 0x4F: opcodeLoadByte(BC.getLowRegister(), AF.getHighRegister()); break;
		case 0x50: opcodeLoadByte(DE.getHighRegister(), BC.getHighRegister()); break;
		case 0x51: opcodeLoadByte(DE.getHighRegister(), BC.getLowRegister()); break;
		case 0x52: opcodeLoadByte(DE.getHighRegister(), DE.getHighRegister()); break;
		case 0x53: opcodeLoadByte(DE.getHighRegister(), DE.getLowRegister()); break;
		case 0x54: opcodeLoadByte(DE.getHighRegister(), HL.getHighRegister()); break;
		case 0x55: opcodeLoadByte(DE.getHighRegister(), HL.getLowRegister()); break;
		//case 0x56: opcodeLoadByte(DE.getHighRegister(), HL.getValue()); break;
		case 0x57: opcodeLoadByte(DE.getHighRegister(), AF.getHighRegister()); break;
		case 0x58: opcodeLoadByte(DE.getLowRegister(), BC.getHighRegister()); break;
		case 0x59: opcodeLoadByte(DE.getLowRegister(), BC.getLowRegister()); break;
		case 0x5A: opcodeLoadByte(DE.getLowRegister(), DE.getHighRegister()); break;
		case 0x5B: opcodeLoadByte(DE.getLowRegister(), DE.getLowRegister()); break;
		case 0x5C: opcodeLoadByte(DE.getLowRegister(), HL.getHighRegister()); break;
		case 0x5D: opcodeLoadByte(DE.getLowRegister(), HL.getLowRegister()); break;
		//case 0x5E: opcodeLoadByte(DE.getLowRegister(), HL.getValue()); break;
		case 0x5F: opcodeLoadByte(DE.getLowRegister(), AF.getHighRegister()); break;
		case 0x60: opcodeLoadByte(HL.getHighRegister(), BC.getHighRegister()); break;
		case 0x61: opcodeLoadByte(HL.getHighRegister(), BC.getLowRegister()); break;
		case 0x62: opcodeLoadByte(HL.getHighRegister(), DE.getHighRegister()); break;
		case 0x63: opcodeLoadByte(HL.getHighRegister(), DE.getLowRegister()); break;
		case 0x64: opcodeLoadByte(HL.getHighRegister(), HL.getHighRegister()); break;
		case 0x65: opcodeLoadByte(HL.getHighRegister(), HL.getLowRegister()); break;
		//case 0x66: opcodeLoadByte(HL.getHighRegister(), HL.getValue()); break;
		case 0x67: opcodeLoadByte(HL.getHighRegister(), AF.getHighRegister()); break;
		case 0x68: opcodeLoadByte(HL.getLowRegister(), BC.getHighRegister()); break;
		case 0x69: opcodeLoadByte(HL.getLowRegister(), BC.getLowRegister()); break;
		case 0x6A: opcodeLoadByte(HL.getLowRegister(), DE.getHighRegister()); break;
		case 0x6B: opcodeLoadByte(HL.getLowRegister(), DE.getLowRegister()); break;
		case 0x6C: opcodeLoadByte(HL.getLowRegister(), HL.getHighRegister()); break;
		case 0x6D: opcodeLoadByte(HL.getLowRegister(), HL.getLowRegister()); break;
		//case 0x6E: opcodeLoadByte(HL.getLowRegister(), HL.getValue()); break;
		case 0x6F: opcodeLoadByte(HL.getLowRegister(), AF.getHighRegister()); break;
		/*
		case 0x70: opcodeLoadByte(HL.getValue(), BC.getHighRegister()); break;
		case 0x71: opcodeLoadByte(HL.getValue(), BC.getLowRegister()); break;
		case 0x72: opcodeLoadByte(HL.getValue(), DE.getHighRegister()); break;
		case 0x73: opcodeLoadByte(HL.getValue(), DE.getLowRegister()); break;
		case 0x74: opcodeLoadByte(HL.getValue(), HL.getHighRegister()); break;
		case 0x75: opcodeLoadByte(HL.getValue(), HL.getLowRegister()); break;
		*/
		case 0x76: opcodeHalt(); break;
		//case 0x77: opcodeLoadByte(HL.getValue(), AF.getHighRegister()); break;
		case 0x78: opcodeLoadByte(AF.getHighRegister(), BC.getHighRegister()); break;
		case 0x79: opcodeLoadByte(AF.getHighRegister(), BC.getLowRegister()); break;
		case 0x7A: opcodeLoadByte(AF.getHighRegister(), DE.getHighRegister()); break;
		case 0x7B: opcodeLoadByte(AF.getHighRegister(), DE.getLowRegister()); break;
		case 0x7C: opcodeLoadByte(AF.getHighRegister(), HL.getHighRegister()); break;
		case 0x7D: opcodeLoadByte(AF.getHighRegister(), HL.getLowRegister()); break;
		//case 0x7E: opcodeLoadByte(AF.getHighRegister(), HL.getValue()); break;
		case 0x7F: opcodeLoadByte(AF.getHighRegister(), AF.getHighRegister()); break;
		case 0x80: opcodeAddA(BC.getHighRegister()); break;
		case 0x81: opcodeAddA(BC.getLowRegister()); break;
		case 0x82: opcodeAddA(DE.getHighRegister()); break;
		case 0x83: opcodeAddA(DE.getLowRegister()); break;
		case 0x84: opcodeAddA(HL.getHighRegister()); break;
		case 0x85: opcodeAddA(HL.getLowRegister()); break;
		//case 0x86: opcodeAddA(HL.getValue()); break;
		case 0x87: opcodeAddA(AF.getHighRegister()); break;
		case 0x88: opcodeADC(BC.getHighRegister()); break;
		case 0x89: opcodeADC(BC.getLowRegister()); break;
		case 0x8A: opcodeADC(DE.getHighRegister()); break;
		case 0x8B: opcodeADC(DE.getLowRegister()); break;
		case 0x8C: opcodeADC(HL.getHighRegister()); break;
		case 0x8D: opcodeADC(HL.getLowRegister()); break;
		//case 0x8E: opcodeADC(HL.getValue()); break;
		case 0x8F: opcodeADC(AF.getHighRegister()); break;
		case 0x90: opcodeSubA(BC.getHighRegister()); break;
		case 0x91: opcodeSubA(BC.getLowRegister()); break;
		case 0x92: opcodeSubA(DE.getHighRegister()); break;
		case 0x93: opcodeSubA(DE.getLowRegister()); break;
		case 0x94: opcodeSubA(HL.getHighRegister()); break;
		case 0x95: opcodeSubA(HL.getLowRegister()); break;
		//case 0x96: opcodeSubA(HL.getValue()); break;
		case 0x97: opcodeSubA(AF.getHighRegister()); break;
		case 0x98: opcodeSBC(BC.getHighRegister()); break;
		case 0x99: opcodeSBC(BC.getLowRegister()); break;
		case 0x9A: opcodeSBC(DE.getHighRegister()); break;
		case 0x9B: opcodeSBC(DE.getLowRegister()); break;
		case 0x9C: opcodeSBC(HL.getHighRegister()); break;
		case 0x9D: opcodeSBC(HL.getLowRegister()); break;
		//case 0x9E: opcodeSBC(HL.getValue()); break;
		case 0x9F: opcodeSBC(AF.getHighRegister()); break;
		case 0xA0: opcodeAnd(BC.getHighRegister()); break;
		case 0xA1: opcodeAnd(BC.getLowRegister()); break;
		case 0xA2: opcodeAnd(DE.getHighRegister()); break;
		case 0xA3: opcodeAnd(DE.getLowRegister()); break;
		case 0xA4: opcodeAnd(HL.getHighRegister()); break;
		case 0xA5: opcodeAnd(HL.getLowRegister()); break;
		//case 0xA6: opcodeAnd(HL.getValue()); break;
		case 0xA7: opcodeAnd(AF.getHighRegister()); break;
		case 0xA8: opcodeXOR(BC.getHighRegister()); break;
		case 0xA9: opcodeXOR(BC.getLowRegister()); break;
		case 0xAA: opcodeXOR(DE.getHighRegister()); break;
		case 0xAB: opcodeXOR(DE.getLowRegister()); break;
		case 0xAC: opcodeXOR(HL.getHighRegister()); break;
		case 0xAD: opcodeXOR(HL.getLowRegister()); break;
		//case 0xAE: opcodeXOR(HL.getValue()); break;
		case 0xAF: opcodeXOR(AF.getHighRegister()); break;
		case 0xB0: opcodeOr(BC.getHighRegister()); break;
		case 0xB1: opcodeOr(BC.getLowRegister()); break;
		case 0xB2: opcodeOr(DE.getHighRegister()); break;
		case 0xB3: opcodeOr(DE.getLowRegister()); break;
		case 0xB4: opcodeOr(HL.getHighRegister()); break;
		case 0xB5: opcodeOr(HL.getLowRegister()); break;
		//case 0xB6: opcodeOr(HL.getValue()); break;
		case 0xB7: opcodeOr(AF.getHighRegister()); break;
		case 0xB8: opcodeCP(BC.getHighRegister()); break;
		case 0xB9: opcodeCP(BC.getLowRegister()); break;
		case 0xBA: opcodeCP(DE.getHighRegister()); break;
		case 0xBB: opcodeCP(DE.getLowRegister()); break;
		case 0xBC: opcodeCP(HL.getHighRegister()); break;
		case 0xBD: opcodeCP(HL.getLowRegister()); break;
		//case 0xBE: opcodeCP(HL.getValue()); break;
		case 0xBF: opcodeCP(AF.getHighRegister()); break;
		case 0xC0: opcodeRet(Condition::NZ); break;
		case 0xC1: stackPop(BC); break;
		case 0xC2: opcodeJP(Condition::NZ); break;
		case 0xC3: opcodeJP(); break;
		case 0xC4: opcodeCall(Condition::NZ); break;
		case 0xC5: stackPush(BC); break;
		case 0xC6: opcodeAddA(); break;
		case 0xC7: opcodeRST(0x00); break;
		case 0xC8: opcodeRet(Condition::Z); break;
		case 0xC9: opcodeRet(); break;
		case 0xCA: opcodeJP(Condition::Z); break;
		case 0xCC: opcodeCall(Condition::Z); break;
		case 0xCD: opcodeCall(); break;
		case 0xCE: opcodeADC(); break;
		case 0xCF: opcodeRST(0x08); break;
		case 0xD0: opcodeRet(Condition::NC); break;
		case 0xD1: stackPop(DE); break;
		case 0xD2: opcodeJP(Condition::NC); break;
		case 0xD3: break; //no operation
		case 0xD4: opcodeCall(Condition::NC); break;
		case 0xD5: stackPush(DE); break;
		case 0xD6: opcodeSubA(); break;
		case 0xD7: opcodeRST(0x10); break;
		case 0xD8: opcodeRet(Condition::C); break;
		case 0xD9: opcodeRetI(); break;
		case 0xDA: opcodeJP(Condition::C); break;
		case 0xDB: break; //no operation
		case 0xDC: opcodeCall(Condition::C); break;
		case 0xDD: break; //no operation
		case 0xDE: opcodeSBC(); break;
		case 0xDF: opcodeRST(0x18); break;
		case 0xE0: opcodeLoadAToMemory(); break;
		case 0xE1: stackPop(HL); break;
		case 0xE2: opcodeLoadAToMemory(BC.getLowRegister()); break;
		case 0xE3: break; //no operation
		case 0xE4: break; //no operation
		case 0xE5: stackPush(HL); break;
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
		case 0xF1: stackPop(AF); break;
		case 0xF2: opcodeLoadMemoryToA(BC.getLowRegister()); break;
		case 0xF3: opcodeDI(); break;
		case 0xF4: break; //no operation
		case 0xF5: stackPush(AF); break;
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

int CPU::executePrefixedOpcode(uint8_t opcode, uint16_t PCValue) {
	switch (opcode) {
	case 0x00: //rlc b break;
	}

	return clock.p;
}

