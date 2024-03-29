#include "channel.h"

Channel::Channel(MemoryMap& newmm) :
	mm(newmm)
{
	NRRegisters.fill(0);
}


void Channel::resetVolume() {
	uint8_t volumeData = mm.readAddress(NRRegisters[2]);
	volumePeriod = volumeData & 0x7;
	volumeTimer = volumePeriod;
	if (volumeTimer > 0) {
		envelopeDisabled = false;
	}

	currentVolume = volumeData >> 4;
	volumeDirection = getBit(volumeData, 3);
}

void Channel::decrementVolumeTimer() {
	if (volumeTimer > 0) {
		volumeTimer--;//decremented every time it gets clocked by frame sequencer
	}
	if (volumeTimer <= 0 && !envelopeDisabled) {
		bool direction = volumeDirection;
		if (!direction) {
			decrementVolume();
		}
		else {
			incrementVolume();
		}

		volumeTimer = volumePeriod;//change to stored value here?
		if (volumeTimer == 0) {
			envelopeDisabled = true;
		}
	}
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
	return mm.readAddress(NRRegisters[1]) & 0x3F;
}

void Channel::resetLengthCounter(uint8_t newLength) {
	lengthCounter = 64 - newLength;
	isDisabled = false;
	//lengthEnabled = getBit(mm.readAddress(NRRegisters[4]), 6);
}

void Channel::decrementLengthCounter() {
	if (lengthEnabled && lengthCounter > 0) {
		lengthCounter--;
		if (lengthCounter == 0) {
			isDisabled = true;
		}
	}
	
}

void Channel::decrementFrequencyTimer(int ticks) {
	frequencyTimer -= ticks;
	if (frequencyTimer < 0) {
		int carryOver = frequencyTimer;
		resetFrequencyTimer();
		frequencyTimer += carryOver;
	}
}

void Channel::resetFrequencyPeriod() {
	uint16_t hiFrequency = mm.readAddress(NRRegisters[4]);
	frequencyPeriod = ((hiFrequency & 0x7) << 8) | mm.readAddress(NRRegisters[3]);
}

void Channel::resetFrequencyTimer() {
	frequencyTimer = (2048 - frequencyPeriod) * 4;
}

