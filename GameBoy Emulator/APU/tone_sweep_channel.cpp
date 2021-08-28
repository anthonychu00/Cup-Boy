#include "tone_sweep_channel.h"

ToneSweepChannel::ToneSweepChannel(MemoryMap& mm): ToneChannel(mm) {
	NRRegisters[0] = 0xFF10;//sweep period
	NRRegisters[1] = 0xFF11;//duty info
	NRRegisters[2] = 0xFF12;//volume info
	NRRegisters[3] = 0xFF13;//lo frequency, written by ROM automatically
	NRRegisters[4] = 0xFF14;//hi frequency + other info
	
	resetFrequencyPeriod();
	frequencyTimer = (2048 - frequencyPeriod) * 4;

	currentVolume = mm.readAddress(NRRegisters[2]) >> 4;
	volumePeriod = mm.readAddress(NRRegisters[2]) & 0x7;
	volumeTimer = volumePeriod;
	volumeDirection = getBit(mm.readAddress(NRRegisters[2]), 3);

	lengthCounter = 64 - getLengthData();
	lengthEnabled = getBit(mm.readAddress(NRRegisters[4]), 6);

	sweepTimer = getSweepTime();
}

void ToneSweepChannel::handleWrittenRegister(uint16_t address, uint8_t data) {
	switch (address) {
	case 0xFF10: resetSweep(); break;//sweep stuff
	case 0xFF11: resetLengthCounter(data & 0x3F); break;
	case 0xFF12: resetVolume(); break;
	case 0xFF13: resetFrequencyPeriod(); break;
	case 0xFF14:
		resetFrequencyPeriod();
		lengthEnabled = getBit(data, 6);
		if (getBit(data, 7)) {
			reset();
		}
		break;
	}
}

void ToneSweepChannel::reset() {
	isDisabled = false;
	if (lengthCounter == 0) {
		lengthCounter = 64;
	}
	resetFrequencyTimer();
	resetVolume();
	//sweep stuff
	resetSweep();
	frequencyShadowRegister = frequencyPeriod;
	if (getSweepShift() != 0) {
		frequencyCalculation();
	}

}

void ToneSweepChannel::resetSweep() {
	sweepTimer = getSweepTime();
	if (sweepTimer > 0 || getSweepShift() > 0) {
		sweepDisabled = false;
	}
	else {
		sweepDisabled = true;
	}
}

void ToneSweepChannel::setFrequency(uint16_t newFrequency) {
	uint8_t loRegister = newFrequency & 0xFF;
	mm.writeAddress(NRRegisters[3], loRegister);
	
	uint8_t hiRegister = (newFrequency >> 8) & 0x7;
	uint8_t hiFrequency = mm.readAddress(NRRegisters[4]);
	hiFrequency = hiFrequency & 0xF8; //F8 = 1111 1000
	mm.writeAddress(NRRegisters[4], hiFrequency | hiRegister);
}

void ToneSweepChannel::frequencyCalculation() {
	uint16_t newFrequency = 0;
	uint8_t shift = getSweepShift();
	uint16_t addedFrequency = frequencyShadowRegister >> shift;
	bool direction = getSweepDirection();
	if (!direction) {//increase frequency
		newFrequency = frequencyShadowRegister + addedFrequency;
	}
	else {
		newFrequency = frequencyShadowRegister - addedFrequency;
	}

	if (newFrequency > 2047) {
		isDisabled = true;
	}
	else if (shift > 0) {
		isDisabled = false;
		frequencyShadowRegister = newFrequency;
		setFrequency(newFrequency);
		
		//frequency calculation/overflow is run again with new value, but not written
		addedFrequency = frequencyShadowRegister >> shift;
		if (!direction) {//increase frequency
			newFrequency = frequencyShadowRegister + addedFrequency;
		}
		else {
			newFrequency = frequencyShadowRegister - addedFrequency;
		}

		if (newFrequency > 2047) {
			isDisabled = true;
		}
		else {
			isDisabled = false;
		}
	}

}

void ToneSweepChannel::decrementSweepTimer() {
	uint8_t newTime = getSweepTime();
	if (sweepTimer > 0) {
		sweepTimer--;
	}
	if (sweepTimer <= 0 && newTime > 0 && !sweepDisabled) {
		frequencyCalculation();
		sweepTimer = newTime;
		if (sweepTimer == 0 && getSweepShift() == 0) {
			sweepDisabled = true;
		}
		else {
			sweepDisabled = false;
		}
	}
}

uint8_t ToneSweepChannel::getSweepTime() const {
	return (mm.readAddress(NRRegisters[0]) >> 4) & 0x7;
}

bool ToneSweepChannel::getSweepDirection() const {
	return getBit(mm.readAddress(NRRegisters[0]), 3);
}

uint8_t ToneSweepChannel::getSweepShift() const {
	return mm.readAddress(NRRegisters[0]) & 0x7;
}