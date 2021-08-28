#include "tone_channel.h"
const std::array<bool, 4 * 8> ToneChannel::dutyCycleTable = {
	0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 1, 1,
	0, 1, 1, 1, 1, 1, 1, 0
};

ToneChannel::ToneChannel(MemoryMap& mm) : Channel(mm) {
	NRRegisters[0] = 0xFF15;//not used
	NRRegisters[1] = 0xFF16;//duty info
	NRRegisters[2] = 0xFF17;//volume info
	NRRegisters[3] = 0xFF18;//lo frequency, written by ROM automatically
	NRRegisters[4] = 0xFF19;//hi frequency + other info

	frequencyTimer = (2048 - getFrequency()) * 4;

	currentVolume = mm.readAddress(NRRegisters[2]) >> 4;
	volumePeriod = mm.readAddress(NRRegisters[2]) & 0x7;
	volumeTimer = volumePeriod;
	volumeDirection = getBit(mm.readAddress(NRRegisters[2]), 3);

	lengthCounter = 64 - getLengthData();
	lengthEnabled = getBit(mm.readAddress(NRRegisters[4]), 6);
}

void ToneChannel::handleWrittenRegister(uint16_t address, uint8_t data) {
	switch (address) {
		case 0xFF16: resetLengthCounter(data & 0x3F); break;
		case 0xFF17: resetVolume(); break;
		case 0xFF18: break;
		case 0xFF19: 
			if (getBit(data, 7)) {
				reset();
			}
			break;
	}
}

void ToneChannel::reset() {
	isDisabled = false;
	if (lengthCounter == 0) {
		lengthCounter = 64;
	}
	resetFrequencyTimer();
	resetVolume();
}

uint8_t ToneChannel::getSample() const {
	if (isDisabled == true) {
		return 0;
	}
	if (!getDutyCycleLocation()) {
		return 0;
	}
	else {
		return currentVolume;
	}
}

bool ToneChannel::getDutyCycleLocation() const {
	int selectedDutyCycle = mm.readAddress(NRRegisters[1]) >> 6;
	return dutyCycleTable[selectedDutyCycle * 8 + dutyCycleLocation];
}

void ToneChannel::incrementDutyCycleLocation() {
	dutyCycleLocation++;
	if (dutyCycleLocation > 7) {
		dutyCycleLocation = 0;
	}
}

void ToneChannel::decrementFrequencyTimer(int ticks) {
	frequencyTimer -= ticks;
	if (frequencyTimer < 0) {
		int carryOver = frequencyTimer;
		resetFrequencyTimer();
		incrementDutyCycleLocation();
		frequencyTimer += carryOver;
	}
}