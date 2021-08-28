#include "noise_channel.h"

NoiseChannel::NoiseChannel(MemoryMap& mm): Channel(mm) {
	NRRegisters[0] = 0xFF1F;//not used
	NRRegisters[1] = 0xFF20;//sound length
	NRRegisters[2] = 0xFF21;//volume info
	NRRegisters[3] = 0xFF22;//polynomial counter
	NRRegisters[4] = 0xFF23;//consecutive counter
	
	currentVolume = mm.readAddress(NRRegisters[2]) >> 4;
	volumePeriod = mm.readAddress(NRRegisters[2]) & 0x7;
	volumeTimer = volumePeriod;
	volumeDirection = getBit(mm.readAddress(NRRegisters[2]), 3);

	lengthCounter = 64 - getLengthData();
	lengthEnabled = getBit(mm.readAddress(NRRegisters[4]), 6);
}

uint8_t NoiseChannel::getSample() const {
	return 0;
}

void NoiseChannel::handleWrittenRegister(const uint16_t address, const uint8_t data) {
	switch (address) {
	case 0xFF20: resetLengthCounter(data & 0x3F); break;
	case 0xFF21: resetVolume(); break;
	case 0xFF22: resetFrequencyPeriod();  break;
	case 0xFF23:
		lengthEnabled = getBit(data, 6);
		if (getBit(data, 7)) {
			reset();
		}
		break;
	}
}

void NoiseChannel::reset() {
	isDisabled = false;
	if (lengthCounter == 0) {
		lengthCounter = 64;
	}
	resetVolume();
}