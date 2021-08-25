#include "channel.h"

Channel::Channel(MemoryMap& newmm) :
	mm(newmm)
{
	NRRegisters.fill(0);
}

void Channel::resetVolumeTimer(uint8_t newPeriod) {
	volumeTimer = newPeriod;
	envelopeDisabled = false;
}

void Channel::decrementVolumeTimer() {
	if (volumeTimer > 0) {
		volumeTimer--;//decremented every time it gets clocked by frame sequencer
	}
	if (volumeTimer <= 0 && !envelopeDisabled) {
		bool direction = getVolumeDirection();
		if (!direction) {
			decrementVolume();
		}
		else {
			incrementVolume();
		}

		volumeTimer = getVolumePeriod();
		if (volumeTimer == 0) {
			envelopeDisabled = true;
		}
	}
}

int Channel::getInitialVolume() const {
	return (mm.readAddress(NRRegisters[2]) >> 4);
}

bool Channel::getVolumeDirection() const {
	return getBit(mm.readAddress(NRRegisters[2]), 3);
}

int Channel::getVolumePeriod() const {
	return mm.readAddress(NRRegisters[2]) & 0x7;
}

void Channel::incrementVolume() {
	if (currentVolume >= 0xF) {
		return;
	}
	currentVolume++;
}

void Channel::decrementVolume() {
	if (currentVolume <= 0) {
		return;
	}
	currentVolume--;
}

uint8_t Channel::getLengthData() {
	return NRRegisters[1] & 0x3F;
}

bool Channel::lengthClocksEnabled() {
	return getBit(NRRegisters[4], 6);
}

void Channel::resetLengthCounter(uint8_t newLength) {
	lengthCounter = newLength;
	isDisabled = false;
}

void Channel::decrementLengthCounter() {
	if (lengthClocksEnabled() && lengthCounter > 0) {
		lengthCounter--;
		if (lengthCounter == 0) {
			isDisabled = true;
		}
	}
	
}
//frequency gets updated mid period
uint16_t Channel::getFrequency() const {
	uint16_t hiFrequency = mm.readAddress(NRRegisters[4]);
	return ((hiFrequency & 0x7) << 8) | mm.readAddress(NRRegisters[3]);
}

void Channel::decrementFrequencyTimer(int ticks) {
	frequencyTimer -= ticks;
	if (frequencyTimer < 0) {
		int carryOver = frequencyTimer;
		resetFrequencyTimer();
		frequencyTimer -= carryOver;
	}
}

void Channel::resetFrequencyTimer() {
	frequencyTimer = (2048 - getFrequency()) * 4;
}

