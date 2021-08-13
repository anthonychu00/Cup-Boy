#include "APU.h"

APU::APU(MemoryMap& newMM) :
	mm(newMM),
	dutyCycleTable{
	0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 1, 1,
	0, 1, 1, 1, 1, 1, 1, 0
}
{
	
}

void APU::playBeeps() {
	//bit 6 and 7 select the duty cycle
	uint8_t ch2CurrentDuty = mm.readAddress(CH2_wave) >> 6;
	currentDutyCycleLocation = ch2CurrentDuty * 8;

	uint16_t ch2HiFrequency = mm.readAddress(CH2_hi_frequency);
	uint16_t ch2Frequency = ((ch2HiFrequency & 0x7) << 8) | mm.readAddress(CH2_lo_frequency);
	timer = (2048 - ch2Frequency) * 4;
	//timer constant across all channels
}