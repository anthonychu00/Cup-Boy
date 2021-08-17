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
	frequencyTimer = (2048 - getFrequency()) * 4;//put in separate init()?
	currentVolume = getInitialVolume();
	volumeTimer = getVolumePeriod();
	lengthCounter = getLengthData();
}

void ToneChannel::setDutyCycleLocation() {
	//bit 6 and 7 select the duty cycle
	selectedDutyCycle = mm.readAddress(NRRegisters[1]) >> 6;
	dutyCycleLocation = 0;
}

bool ToneChannel::getDutyCycleLocation() {
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
		frequencyTimer -= carryOver;
	}
}