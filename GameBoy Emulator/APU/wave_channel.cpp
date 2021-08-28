#include "wave_channel.h"

WaveChannel::WaveChannel(MemoryMap& mm) : Channel(mm){
	NRRegisters[0] = 0xFF1A;//enabler
	NRRegisters[1] = 0xFF1B;//sound length
	NRRegisters[2] = 0xFF1C;//volume level
	NRRegisters[3] = 0xFF1D;//lo frequency, written by ROM automatically
	NRRegisters[4] = 0xFF1E;//hi frequency + other info

	resetFrequencyPeriod();
	resetFrequencyTimer();

}

uint8_t WaveChannel::getSample() const {
	if (isDisabled) {
		return 0;
	}
	if (outputLevel == 0) {
		return 0;
	}
	else {
		int waveRamPostion = waveTableCounter / 2;
		uint8_t ramData = mm.readAddress(0xFF30 + waveRamPostion);

		bool nibble = waveTableCounter % 2; // 0 = higher nibble, 1 = lower nibble
		if (!nibble) {
			return (ramData >> 4) >> (outputLevel - 1);
		}
		else {
			return (ramData & 0xF) >> (outputLevel - 1);
		}
	}
}

void WaveChannel::handleWrittenRegister(const uint16_t address, const uint8_t data) {
	switch (address) {
	case 0xFF1A: isDisabled = !getBit(data, 7); break;
	case 0xFF1B: resetLengthCounter(data); break;
	case 0xFF1C: outputLevel = (mm.readAddress(NRRegisters[2]) >> 5) & 0x3; break;
	case 0xFF1D: resetFrequencyPeriod();  break;
	case 0xFF1E:
		resetFrequencyPeriod();
		lengthEnabled = getBit(data, 6);
		if (getBit(data, 7)) {
			reset();
		}
		break;
	}
	
}

void WaveChannel::reset() {
	isDisabled = false;
	if (lengthCounter == 0) {
		lengthCounter = 256;
	}
	resetFrequencyTimer();
	outputLevel = (mm.readAddress(NRRegisters[2]) >> 5) & 0x3;
	waveTableCounter = 0;
}

void WaveChannel::resetLengthCounter(uint8_t newLength) {
	lengthCounter = 256 - newLength;
	isDisabled = false;
}

void WaveChannel::decrementFrequencyTimer(int ticks) {
	frequencyTimer -= ticks;
	if (frequencyTimer < 0) {
		int carryOver = frequencyTimer;
		resetFrequencyTimer();
		waveTableCounter = (waveTableCounter + 1) & 0x1F;//1F = 31
		frequencyTimer += carryOver;
	}
}

void WaveChannel::resetFrequencyTimer() {
	frequencyTimer = (2048 - frequencyPeriod) * 2;
}