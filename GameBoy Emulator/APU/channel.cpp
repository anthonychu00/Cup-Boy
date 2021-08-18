#include "channel.h"

Channel::Channel(MemoryMap& newmm) :
	mm(newmm)
{
	NRRegisters.fill(0);
}

uint8_t Channel::getSample() const {
	//return modified volume level
}

void Channel::decrementVolumeTimer() {
	volumeTimer--;//decremented every time it gets clocked by frame sequencer
	if (volumeTimer <= 0) {
		volumeTimer = getVolumePeriod();
		bool direction = getVolumeDirection();
		if (!direction) {
			decrementVolume();
		}
		else {
			incrementVolume();
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
	return NRRegisters[1] & 0x1F;
}

bool Channel::lengthClocksEnabled() {
	return getBit(NRRegisters[4], 6);
}

void Channel::decrementLengthCounter() {
	if (lengthClocksEnabled() && lengthCounter > 0) {
		lengthCounter--;
		if (lengthCounter == 0) {
			isDisabled = true;
		}
	}
	
}

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

