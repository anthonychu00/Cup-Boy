#include "noise_channel.h"

NoiseChannel::NoiseChannel(MemoryMap& mm): Channel(mm) {
	NRRegisters[0] = 0xFF1F;//not used
	NRRegisters[1] = 0xFF20;//sound length
	NRRegisters[2] = 0xFF21;//volume info
	NRRegisters[3] = 0xFF22;//polynomial counter
	NRRegisters[4] = 0xFF23;//consecutive counter
	
	resetFrequencyPeriod(mm.readAddress(0xFF22));
	resetFrequencyTimer();

	currentVolume = mm.readAddress(NRRegisters[2]) >> 4;
	volumePeriod = mm.readAddress(NRRegisters[2]) & 0x7;
	volumeTimer = volumePeriod;
	volumeDirection = getBit(mm.readAddress(NRRegisters[2]), 3);

	lengthCounter = 64 - getLengthData();
	lengthEnabled = getBit(mm.readAddress(NRRegisters[4]), 6);
}

uint8_t NoiseChannel::getSample() const {
	if (isDisabled == true) {
		return 0;
	}
	if (getBit(LSFR, 0)) { //1 = no sound
		return 0;
	}
	else {
		return currentVolume;
	}
}

void NoiseChannel::handleWrittenRegister(const uint16_t address, const uint8_t data) {
	switch (address) {
	case 0xFF20: resetLengthCounter(data & 0x3F); break;
	case 0xFF21: resetVolume(); break;
	case 0xFF22: resetFrequencyPeriod(data);  break;
	case 0xFF23:
		lengthEnabled = getBit(data, 6);
		if (getBit(data, 7)) {
			reset();
		}
		break;
	}
}

void NoiseChannel::decrementFrequencyTimer(int ticks) {
	frequencyTimer -= ticks;
	if (frequencyTimer < 0) {
		int carryOver = frequencyTimer;
		resetFrequencyTimer();
		stepLSFR();
		frequencyTimer += carryOver;
	}
}

void NoiseChannel::reset() {
	isDisabled = false;
	if (lengthCounter == 0) {
		lengthCounter = 64;
	}
	resetFrequencyTimer();
	resetVolume();
	LSFR = 0x7FFF;
}

void NoiseChannel::resetFrequencyPeriod(uint8_t data) {
	int divisorCode = data & 0x7;
	int divisor = 0;
	if (divisorCode == 0) {
		divisor = 8;
	}
	else {
		divisor = divisorCode * 16;
	}

	int shift = data >> 4;
	frequencyPeriod = divisor << shift;
	widthMode = getBit(data, 3);
}

void NoiseChannel::resetFrequencyTimer() {
	frequencyTimer = frequencyPeriod;
}

void NoiseChannel::stepLSFR() {
	bool newHighBit = getBit(LSFR, 0) ^ getBit(LSFR, 1);
	LSFR = LSFR >> 1;

	setBit(LSFR, 14, newHighBit);
	if (widthMode) {
		setBit(LSFR, 6, newHighBit);
	}
}