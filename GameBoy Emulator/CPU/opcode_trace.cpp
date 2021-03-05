#pragma once
#include "CPU.h"

string CPU::traceOpcode(uint8_t opcode) {
	string trace = "error";
	//perform action according to the opcode
	switch (opcode) {
	case 0x00: trace = "NOP"; break;
	case 0x01: trace = "LD BC, u16"; break;
	case 0x02: trace = "LD (BC), A"; break;
	case 0x03: trace = "INC BC"; break;
	case 0x04: trace = "INC B"; break;
	case 0x05: trace = "DEC B"; break;
	case 0x06: trace = "LD B, u8"; break;
	case 0x07: trace = "RLCA" ; break;
	case 0x08: trace = "LD (u16), SP"; break;
	case 0x09: trace = "ADD HL, BC"; break;
	case 0x0A: trace = "LD A, (BC)"; break;
	case 0x0B: trace = "DEC BC"; break;
	case 0x0C: trace = "INC C"; break;
	case 0x0D: trace = "DEC C"; break;
	case 0x0E: trace = "LD C, u8"; break;
	case 0x0F: trace = "RRCA"; break;
	case 0x10: trace = "STOP"; break;
	case 0x11: trace = "LD DE, u16"; break;
	case 0x12: trace = "LD (DE), A"; break;
	case 0x13: trace = "INC DE"; break;
	case 0x14: trace = "INC D"; break;
	case 0x15: trace = "DEC D"; break;
	case 0x16: trace = "LD D, u8"; break;
	case 0x17: trace = "RLA"; break;
	case 0x18: trace = "i8"; break;
	case 0x19: trace = "ADD HL, DE"; break;
	case 0x1A: trace = "LD A, (DE)"; break;
	case 0x1B: trace = "DEC DE"; break;
	case 0x1C: trace = "INC E"; break;
	case 0x1D: trace = "DEC E"; break;
	case 0x1E: trace = "LD E, u8"; break;
	case 0x1F: trace = "RRA"; break;
	case 0x20: trace = "JR NZ, i8"; break;
	case 0x21: trace = "LD HL, u16"; break;
	case 0x22: trace = "LD (HL+), A"; break;
	case 0x23: trace = "INC HL"; break;
	case 0x24: trace = "INC H"; break;
	case 0x25: trace = "DEC H"; break;
	case 0x26: trace = "LD H, u8"; break;
	case 0x27: trace = "DAA"; break;
	case 0x28: trace = "JR Z, i8"; break;
	case 0x29: trace = "ADD HL, HL"; break;
	case 0x2A: trace = "LD A, (HL+)"; break;
	case 0x2B: trace = "DEC HL"; break;
	case 0x2C: trace = "INC L"; break;
	case 0x2D: trace = "DEC L"; break;
	case 0x2E: trace = "LD L, u8"; break;
	case 0x2F: trace = "CPL"; break;
	case 0x30: trace = "JR NC, i8"; break;
	case 0x31: trace = "LD SP, u16"; break;
	case 0x32: trace = "LD (HL-), A"; break;
	case 0x33: trace = "INC SP"; break;
	case 0x34: trace = "INC (HL)"; break;
	case 0x35: trace = "DEC (HL)"; break;
	case 0x36: trace = "LD (HL), u8"; break;
	case 0x37: trace = "SCF"; break;
	case 0x38: trace = "JR C, i8"; break;
	case 0x39: trace = "ADD HL, SP"; break;
	case 0x3A: trace = "LD A, (HL-)"; break;
	case 0x3B: trace = "DEC SP"; break;
	case 0x3C: trace = "INC A"; break;
	case 0x3D: trace = "DEC A"; break;
	case 0x3E: trace = "LD A, u8"; break;
	case 0x3F: trace = "CCF"; break;
	case 0x40: trace = "LD B, B"; break;
	case 0x41: trace = "LD B, C"; break;
	case 0x42: trace = "LD B, D"; break;
	case 0x43: trace = "LD B, E"; break;
	case 0x44: trace = "LD B, H"; break;
	case 0x45: trace = "LD B, L"; break;
	case 0x46: trace = "LD B, (HL)"; break;
	case 0x47: trace = "LD B, A"; break;
	case 0x48: trace = "LD C, B"; break;
	case 0x49: trace = "LD C, C"; break;
	case 0x4A: trace = "LD C, D"; break;
	case 0x4B: trace = "LD C, E"; break;
	case 0x4C: trace = "LD C, H"; break;
	case 0x4D: trace = "LD C, L"; break;
	case 0x4E: trace = "LD C, (HL)"; break;
	case 0x4F: trace = "LD C, A"; break;
	case 0x50: trace = "LD D, B"; break;
	case 0x51: trace = "LD D, C"; break;
	case 0x52: trace = "LD D, D"; break;
	case 0x53: trace = "LD D, E"; break;
	case 0x54: trace = "LD D, H"; break;
	case 0x55: trace = "LD D, L"; break;
	case 0x56: trace = "LD D, (HL)"; break;
	case 0x57: trace = "LD D, A"; break;
	case 0x58: trace = "LD E, B"; break;
	case 0x59: trace = "LD E, C"; break;
	case 0x5A: trace = "LD E, D"; break;
	case 0x5B: trace = "LD E, E"; break;
	case 0x5C: trace = "LD E, H"; break;
	case 0x5D: trace = "LD E, L"; break;
	case 0x5E: trace = "LD E, (HL)"; break;
	case 0x5F: trace = "LD E, A"; break;
	case 0x60: trace = "LD H, B"; break;
	case 0x61: trace = "LD H, C"; break;
	case 0x62: trace = "LD H, D"; break;
	case 0x63: trace = "LD H, E"; break;
	case 0x64: trace = "LD H, H"; break;
	case 0x65: trace = "LD H, L"; break;
	case 0x66: trace = "LD H, (HL)"; break;
	case 0x67: trace = "LD H, A"; break;
	case 0x68: trace = "LD L, B"; break;
	case 0x69: trace = "LD L, C"; break;
	case 0x6A: trace = "LD L, D"; break;
	case 0x6B: trace = "LD L, E"; break;
	case 0x6C: trace = "LD L, H"; break;
	case 0x6D: trace = "LD L, L"; break;
	case 0x6E: trace = "LD L, (HL)"; break;
	case 0x6F: trace = "LD L, A"; break;
	case 0x70: trace = "LD (HL), B"; break;
	case 0x71: trace = "LD (HL), C"; break;
	case 0x72: trace = "LD (HL), D"; break;
	case 0x73: trace = "LD (HL), E"; break;
	case 0x74: trace = "LD (HL), H"; break;
	case 0x75: trace = "LD (HL), L"; break;
	case 0x76: trace = "HALT"; break;
	case 0x77: trace = "LD (HL), A"; break;
	case 0x78: trace = "LD A, B"; break;
	case 0x79: trace = "LD A, C"; break;
	case 0x7A: trace = "LD A, D"; break;
	case 0x7B: trace = "LD A, E"; break;
	case 0x7C: trace = "LD A, H"; break;
	case 0x7D: trace = "LD A, L"; break;
	case 0x7E: trace = "LD A, (HL)"; break;
	case 0x7F: trace = "LD A, A"; break;
	case 0x80: trace = "ADD A, B"; break;
	case 0x81: trace = "ADD A, C"; break;
	case 0x82: trace = "ADD A, D"; break;
	case 0x83: trace = "ADD A, E"; break;
	case 0x84: trace = "ADD A, H"; break;
	case 0x85: trace = "ADD A, L"; break;
	case 0x86: trace = "ADD A, (HL)"; break;
	case 0x87: trace = "ADD A, A"; break;
	case 0x88: trace = "ADC A, B"; break;
	case 0x89: trace = "ADC A, C"; break;
	case 0x8A: trace = "ADC A, D"; break;
	case 0x8B: trace = "ADC A, E"; break;
	case 0x8C: trace = "ADC A, H"; break;
	case 0x8D: trace = "ADC A, L"; break;
	case 0x8E: trace = "ADC A, (HL)"; break;
	case 0x8F: trace = "ADC A, A"; break;
	case 0x90: trace = "SUB A, B"; break;
	case 0x91: trace = "SUB A, C"; break;
	case 0x92: trace = "SUB A, D"; break;
	case 0x93: trace = "SUB A, E"; break;
	case 0x94: trace = "SUB A, H"; break;
	case 0x95: trace = "SUB A, L"; break;
	case 0x96: trace = "SUB A, (HL)"; break;
	case 0x97: trace = "SUB A, A"; break;
	case 0x98: trace = "SBC A, B"; break;
	case 0x99: trace = "SBC A, C"; break;
	case 0x9A: trace = "SBC A, D"; break;
	case 0x9B: trace = "SBC A, E"; break;
	case 0x9C: trace = "SBC A, H"; break;
	case 0x9D: trace = "SBC A, L"; break;
	case 0x9E: trace = "SBC A, (HL)"; break;
	case 0x9F: trace = "SBC A, A"; break;
	case 0xA0: trace = "AND A, B"; break;
	case 0xA1: trace = "AND A, C"; break;
	case 0xA2: trace = "AND A, D"; break;
	case 0xA3: trace = "AND A, E"; break;
	case 0xA4: trace = "AND A, H"; break;
	case 0xA5: trace = "AND A, L"; break;
	case 0xA6: trace = "AND A, (HL)"; break;
	case 0xA7: trace = "AND A, A"; break;
	case 0xA8: trace = "XOR A, B"; break;
	case 0xA9: trace = "XOR A, C"; break;
	case 0xAA: trace = "XOR A, D"; break;
	case 0xAB: trace = "XOR A, E"; break;
	case 0xAC: trace = "XOR A, H"; break;
	case 0xAD: trace = "XOR A, L"; break;
	case 0xAE: trace = "XOR A, (HL)"; break;
	case 0xAF: trace = "XOR A, A"; break;
	case 0xB0: trace = "OR A, B"; break;
	case 0xB1: trace = "OR A, C"; break;
	case 0xB2: trace = "OR A, D"; break;
	case 0xB3: trace = "OR A, E"; break;
	case 0xB4: trace = "OR A, H"; break;
	case 0xB5: trace = "OR A, L"; break;
	case 0xB6: trace = "OR A, (HL)"; break;
	case 0xB7: trace = "OR A, A"; break;
	case 0xB8: trace = "CP A, B"; break;
	case 0xB9: trace = "CP A, C"; break;
	case 0xBA: trace = "CP A, D"; break;
	case 0xBB: trace = "CP A, E"; break;
	case 0xBC: trace = "CP A, H"; break;
	case 0xBD: trace = "CP A, L"; break;
	case 0xBE: trace = "CP A, (HL)"; break;
	case 0xBF: trace = "CP A, A"; break;
	case 0xC0: trace = "RET NZ"; break;
	case 0xC1: trace = "POP BC"; break;
	case 0xC2: trace = "JP NZ, u16"; break;
	case 0xC3: trace = "JP u16"; break;
	case 0xC4: trace = "CALL NZ, u16"; break;
	case 0xC5: trace = "PUSH BC"; break;
	case 0xC6: trace = "ADD A, u8"; break;
	case 0xC7: trace = "RST 00h"; break;
	case 0xC8: trace = "RET Z"; break;
	case 0xC9: trace = "RET"; break;
	case 0xCA: trace = "JP Z, u16"; break;
	case 0xCC: trace = "CALL Z, u16"; break;
	case 0xCD: trace = "CALL u16"; break;
	case 0xCE: trace = "ADC A, u8"; break;
	case 0xCF: trace = "RST 08h"; break;
	case 0xD0: trace = "RET NC"; break;
	case 0xD1: trace = "POP DE"; break;
	case 0xD2: trace = "JP NC, u16"; break;
	case 0xD3: trace = "no operation"; break; //no operation
	case 0xD4: trace = "CALL NC, u16"; break;
	case 0xD5: trace = "PUSH DE"; break;
	case 0xD6: trace = "SUB A, u8"; break;
	case 0xD7: trace = "RST 10h"; break;
	case 0xD8: trace = "RET C"; break;
	case 0xD9: trace = "RETI"; break;
	case 0xDA: trace = "JP C, u16"; break;
	case 0xDB: trace = "no operation";  break; //no operation
	case 0xDC: trace = "CALL C, u16"; break;
	case 0xDD: trace = "no operation"; break; //no operation
	case 0xDE: trace = "SBC A, u8"; break;
	case 0xDF: trace = "RST 18h"; break;
	case 0xE0: trace = "LD (FF00+u8), A"; break;
	case 0xE1: trace = "POP HL"; break;
	case 0xE2: trace = "LD (FF00+C), A"; break;
	case 0xE3: trace = "no operation"; break; //no operation
	case 0xE4: trace = "no operation"; break; //no operation
	case 0xE5: trace = "PUSH HL"; break;
	case 0xE6: trace = "AND A, u8"; break;
	case 0xE7: trace = "RST 20h"; break;
	case 0xE8: trace = "ADD SP, i8"; break;
	case 0xE9: trace = "JP HL"; break;
	case 0xEA: trace = "LD (u16), A"; break;
	case 0xEB: trace = "no operation"; break; //no operation
	case 0xEC: trace = "no operation"; break; //no operation
	case 0xED: trace = "no operation"; break; //no operation
	case 0xEE: trace = "XOR A, u8"; break;
	case 0xEF: trace = "RST 28h"; break;
	case 0xF0: trace = "LD A, (FF00+u8)"; break;
	case 0xF1: trace = "POP AF"; break;
	case 0xF2: trace = "LD A, (FF00+C)"; break;
	case 0xF3: trace = "DI"; break;
	case 0xF4: trace = "no operation"; break; //no operation
	case 0xF5: trace = "PUSH AF"; break;
	case 0xF6: trace = "OR A, u8"; break;
	case 0xF7: trace = "RST 30h"; break;
	case 0xF8: trace = "LD HL, SP+i8"; break;
	case 0xF9: trace = "LD SP, HL"; break;
	case 0xFA: trace = "LD A, (u16)"; break;
	case 0xFB: trace = "EI"; break;
	case 0xFC: trace = "no operation"; break; //no operation
	case 0xFD: trace = "no operation"; break; //no operation
	case 0xFE: trace = "CP A, u8"; break;
	case 0xFF: trace = "RST 38h"; break;
	}

	return trace;
}

string CPU::tracePrefixedOpcode(const uint8_t opcode) {
	string trace = "error";
	switch (opcode) {
	case 0x00: trace = "RLC B"; break;
	case 0x01: trace = "RLC C"; break;
	case 0x02: trace = "RLC D"; break;
	case 0x03: trace = "RLC E"; break;
	case 0x04: trace = "RLC H"; break;
	case 0x05: trace = "RLC L"; break;
	case 0x06: trace = "RLC (HL)"; break;
	case 0x07: trace = "RLC A"; break;
	case 0x08: trace = "RRC B"; break;
	case 0x09: trace = "RRC C"; break;
	case 0x0A: trace = "RRC D"; break;
	case 0x0B: trace = "RRC E"; break;
	case 0x0C: trace = "RRC H"; break;
	case 0x0D: trace = "RRC L"; break;
	case 0x0E: trace = "RRC (HL)"; break;
	case 0x0F: trace = "RRC A"; break;
	case 0x10: trace = "RL B"; break;
	case 0x11: trace = "RL C"; break;
	case 0x12: trace = "RL D"; break;
	case 0x13: trace = "RL E"; break;
	case 0x14: trace = "RL H"; break;
	case 0x15: trace = "RL L"; break;
	case 0x16: trace = "RL (HL)"; break;
	case 0x17: trace = "RL A"; break;
	case 0x18: trace = "RR B"; break;
	case 0x19: trace = "RR C"; break;
	case 0x1A: trace = "RR D"; break;
	case 0x1B: trace = "RR E"; break;
	case 0x1C: trace = "RR H"; break;
	case 0x1D: trace = "RR L"; break;
	case 0x1E: trace = "RR (HL)"; break;
	case 0x1F: trace = "RR A"; break;
	case 0x20: trace = "SLA B"; break;
	case 0x21: trace = "SLA C"; break;
	case 0x22: trace = "SLA D"; break;
	case 0x23: trace = "SLA E"; break;
	case 0x24: trace = "SLA H"; break;
	case 0x25: trace = "SLA L"; break;
	case 0x26: trace = "SLA (HL)"; break;
	case 0x27: trace = "SLA A"; break;
	case 0x28: trace = "SRA B"; break;
	case 0x29: trace = "SRA C"; break;
	case 0x2A: trace = "SRA D"; break;
	case 0x2B: trace = "SRA E"; break;
	case 0x2C: trace = "SRA H"; break;
	case 0x2D: trace = "SRA L"; break;
	case 0x2E: trace = "SRA (HL)"; break;
	case 0x2F: trace = "SRA A"; break;
	case 0x30: trace = "SWAP B"; break;
	case 0x31: trace = "SWAP C"; break;
	case 0x32: trace = "SWAP D"; break;
	case 0x33: trace = "SWAP E"; break;
	case 0x34: trace = "SWAP H"; break;
	case 0x35: trace = "SWAP L"; break;
	case 0x36: trace = "SWAP (HL)"; break;
	case 0x37: trace = "SWAP A"; break;
	case 0x38: trace = "SRL B"; break;
	case 0x39: trace = "SRL C"; break;
	case 0x3A: trace = "SRL D"; break;
	case 0x3B: trace = "SRL E"; break;
	case 0x3C: trace = "SRL H"; break;
	case 0x3D: trace = "SRL L"; break;
	case 0x3E: trace = "SRL (HL)"; break;
	case 0x3F: trace = "SRL A"; break;
	case 0x40: trace = "BIT 0, B"; break;
	case 0x41: trace = "BIT 0, C"; break;
	case 0x42: trace = "BIT 0, D"; break;
	case 0x43: trace = "BIT 0, E"; break;
	case 0x44: trace = "BIT 0, H"; break;
	case 0x45: trace = "BIT 0, L"; break;
	case 0x46: trace = "BIT 0, (HL)"; break;
	case 0x47: trace = "BIT 0, A"; break;
	case 0x48: trace = "BIT 1, B"; break;
	case 0x49: trace = "BIT 1, C"; break;
	case 0x4A: trace = "BIT 1, D"; break;
	case 0x4B: trace = "BIT 1, E"; break;
	case 0x4C: trace = "BIT 1, H"; break;
	case 0x4D: trace = "BIT 1, L"; break;
	case 0x4E: trace = "BIT 1, (HL)"; break;
	case 0x4F: trace = "BIT 1, A"; break;
	case 0x50: trace = "BIT 2, B"; break;
	case 0x51: trace = "BIT 2, C"; break;
	case 0x52: trace = "BIT 2, D"; break;
	case 0x53: trace = "BIT 2, E"; break;
	case 0x54: trace = "BIT 2, H"; break;
	case 0x55: trace = "BIT 2, L"; break;
	case 0x56: trace = "BIT 2, (HL)"; break;
	case 0x57: trace = "BIT 2, A"; break;
	case 0x58: trace = "BIT 3, B"; break;
	case 0x59: trace = "BIT 3, C"; break;
	case 0x5A: trace = "BIT 3, D"; break;
	case 0x5B: trace = "BIT 3, E"; break;
	case 0x5C: trace = "BIT 3, H"; break;
	case 0x5D: trace = "BIT 3, L"; break;
	case 0x5E: trace = "BIT 3, (HL)"; break;
	case 0x5F: trace = "BIT 3, A"; break;
	case 0x60: trace = "BIT 4, B"; break;
	case 0x61: trace = "BIT 4, C"; break;
	case 0x62: trace = "BIT 4, D"; break;
	case 0x63: trace = "BIT 4, E"; break;
	case 0x64: trace = "BIT 4, H"; break;
	case 0x65: trace = "BIT 4, L"; break;
	case 0x66: trace = "BIT 4, (HL)"; break;
	case 0x67: trace = "BIT 4, A"; break;
	case 0x68: trace = "BIT 5, B"; break;
	case 0x69: trace = "BIT 5, C"; break;
	case 0x6A: trace = "BIT 5, D"; break;
	case 0x6B: trace = "BIT 5, E"; break;
	case 0x6C: trace = "BIT 5, H"; break;
	case 0x6D: trace = "BIT 5, L"; break;
	case 0x6E: trace = "BIT 5, (HL)"; break;
	case 0x6F: trace = "BIT 5, A"; break;
	case 0x70: trace = "BIT 6, B"; break;
	case 0x71: trace = "BIT 6, C"; break;
	case 0x72: trace = "BIT 6, D"; break;
	case 0x73: trace = "BIT 6, E"; break;
	case 0x74: trace = "BIT 6, H"; break;
	case 0x75: trace = "BIT 6, L"; break;
	case 0x76: trace = "BIT 6, (HL)"; break;
	case 0x77: trace = "BIT 6, A"; break;
	case 0x78: trace = "BIT 7, B"; break;
	case 0x79: trace = "BIT 7, C"; break;
	case 0x7A: trace = "BIT 7, D"; break;
	case 0x7B: trace = "BIT 7, E"; break;
	case 0x7C: trace = "BIT 7, H"; break;
	case 0x7D: trace = "BIT 7, L"; break;
	case 0x7E: trace = "BIT 7, (HL)"; break;
	case 0x7F: trace = "BIT 7, A"; break;
	case 0x80: trace = "RES 0, B"; break;
	case 0x81: trace = "RES 0, C"; break;
	case 0x82: trace = "RES 0, D"; break;
	case 0x83: trace = "RES 0, E"; break;
	case 0x84: trace = "RES 0, H"; break;
	case 0x85: trace = "RES 0, L"; break;
	case 0x86: trace = "RES 0, (HL)"; break;
	case 0x87: trace = "RES 0, A"; break;
	case 0x88: trace = "RES 1, B"; break;
	case 0x89: trace = "RES 1, C"; break;
	case 0x8A: trace = "RES 1, D"; break;
	case 0x8B: trace = "RES 1, E"; break;
	case 0x8C: trace = "RES 1, H"; break;
	case 0x8D: trace = "RES 1, L"; break;
	case 0x8E: trace = "RES 1, (HL)"; break;
	case 0x8F: trace = "RES 1, A"; break;
	case 0x90: trace = "RES 2, B"; break;
	case 0x91: trace = "RES 2, C"; break;
	case 0x92: trace = "RES 2, D"; break;
	case 0x93: trace = "RES 2, E"; break;
	case 0x94: trace = "RES 2, H"; break;
	case 0x95: trace = "RES 2, L"; break;
	case 0x96: trace = "RES 2, (HL)"; break;
	case 0x97: trace = "RES 2, A"; break;
	case 0x98: trace = "RES 3, B"; break;
	case 0x99: trace = "RES 3, C"; break;
	case 0x9A: trace = "RES 3, D"; break;
	case 0x9B: trace = "RES 3, E"; break;
	case 0x9C: trace = "RES 3, H"; break;
	case 0x9D: trace = "RES 3, L"; break;
	case 0x9E: trace = "RES 3, (HL)"; break;
	case 0x9F: trace = "RES 3, A"; break;
	case 0xA0: trace = "RES 4, B"; break;
	case 0xA1: trace = "RES 4, C"; break;
	case 0xA2: trace = "RES 4, D"; break;
	case 0xA3: trace = "RES 4, E"; break;
	case 0xA4: trace = "RES 4, H"; break;
	case 0xA5: trace = "RES 4, L"; break;
	case 0xA6: trace = "RES 4, (HL)"; break;
	case 0xA7: trace = "RES 4, A"; break;
	case 0xA8: trace = "RES 5, B"; break;
	case 0xA9: trace = "RES 5, C"; break;
	case 0xAA: trace = "RES 5, D"; break;
	case 0xAB: trace = "RES 5, E"; break;
	case 0xAC: trace = "RES 5, H"; break;
	case 0xAD: trace = "RES 5, L"; break;
	case 0xAE: trace = "RES 5, (HL)"; break;
	case 0xAF: trace = "RES 5, A"; break;
	case 0xB0: trace = "RES 6, B"; break;
	case 0xB1: trace = "RES 6, C"; break;
	case 0xB2: trace = "RES 6, D"; break;
	case 0xB3: trace = "RES 6, E"; break;
	case 0xB4: trace = "RES 6, H"; break;
	case 0xB5: trace = "RES 6, L"; break;
	case 0xB6: trace = "RES 6, (HL)"; break;
	case 0xB7: trace = "RES 6, A"; break;
	case 0xB8: trace = "RES 7, B"; break;
	case 0xB9: trace = "RES 7, C"; break;
	case 0xBA: trace = "RES 7, D"; break;
	case 0xBB: trace = "RES 7, E"; break;
	case 0xBC: trace = "RES 7, H"; break;
	case 0xBD: trace = "RES 7, L"; break;
	case 0xBE: trace = "RES 7, (HL)"; break;
	case 0xBF: trace = "RES 7, A"; break;
	case 0xC0: trace = "SET 0, B"; break;
	case 0xC1: trace = "SET 0, C"; break;
	case 0xC2: trace = "SET 0, D"; break;
	case 0xC3: trace = "SET 0, E"; break;
	case 0xC4: trace = "SET 0, H"; break;
	case 0xC5: trace = "SET 0, L"; break;
	case 0xC6: trace = "SET 0, (HL)"; break;
	case 0xC7: trace = "SET 0, A"; break;
	case 0xC8: trace = "SET 1, B"; break;
	case 0xC9: trace = "SET 1, C"; break;
	case 0xCA: trace = "SET 1, D"; break;
	case 0xCB: trace = "SET 1, E"; break;
	case 0xCC: trace = "SET 1, H"; break;
	case 0xCD: trace = "SET 1, L"; break;
	case 0xCE: trace = "SET 1, (HL)"; break;
	case 0xCF: trace = "SET 1, A"; break;
	case 0xD0: trace = "SET 2, B"; break;
	case 0xD1: trace = "SET 2, C"; break;
	case 0xD2: trace = "SET 2, D"; break;
	case 0xD3: trace = "SET 2, E"; break;
	case 0xD4: trace = "SET 2, H"; break;
	case 0xD5: trace = "SET 2, L"; break;
	case 0xD6: trace = "SET 2, (HL)"; break;
	case 0xD7: trace = "SET 2, A"; break;
	case 0xD8: trace = "SET 3, B"; break;
	case 0xD9: trace = "SET 3, C"; break;
	case 0xDA: trace = "SET 3, D"; break;
	case 0xDB: trace = "SET 3, E"; break;
	case 0xDC: trace = "SET 3, H"; break;
	case 0xDD: trace = "SET 3, L"; break;
	case 0xDE: trace = "SET 3, (HL)"; break;
	case 0xDF: trace = "SET 3, A"; break;
	case 0xE0: trace = "SET 4, B"; break;
	case 0xE1: trace = "SET 4, C"; break;
	case 0xE2: trace = "SET 4, D"; break;
	case 0xE3: trace = "SET 4, E"; break;
	case 0xE4: trace = "SET 4, H"; break;
	case 0xE5: trace = "SET 4, L"; break;
	case 0xE6: trace = "SET 4, (HL)"; break;
	case 0xE7: trace = "SET 4, A"; break;
	case 0xE8: trace = "SET 5, B"; break;
	case 0xE9: trace = "SET 5, C"; break;
	case 0xEA: trace = "SET 5, D"; break;
	case 0xEB: trace = "SET 5, E"; break;
	case 0xEC: trace = "SET 5, H"; break;
	case 0xED: trace = "SET 5, L"; break;
	case 0xEE: trace = "SET 5, (HL)"; break;
	case 0xEF: trace = "SET 5, A"; break;
	case 0xF0: trace = "SET 6, B"; break;
	case 0xF1: trace = "SET 6, C"; break;
	case 0xF2: trace = "SET 6, D"; break;
	case 0xF3: trace = "SET 6, E"; break;
	case 0xF4: trace = "SET 6, H"; break;
	case 0xF5: trace = "SET 6, L"; break;
	case 0xF6: trace = "SET 6, (HL)"; break;
	case 0xF7: trace = "SET 6, A"; break;
	case 0xF8: trace = "SET 7, B"; break;
	case 0xF9: trace = "SET 7, C"; break;
	case 0xFA: trace = "SET 7, D"; break;
	case 0xFB: trace = "SET 7, E"; break;
	case 0xFC: trace = "SET 7, H"; break;
	case 0xFD: trace = "SET 7, L"; break;
	case 0xFE: trace = "SET 7, (HL)"; break;
	case 0xFF: trace = "SET 7, A"; break;
	}

	return trace;
}