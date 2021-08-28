#pragma once
#include "channel.h"

//Channel 3, Sine Waves, Wave Output
class WaveChannel : public Channel {
public:
	WaveChannel(MemoryMap& mm);

	void handleWrittenRegister(const uint16_t address, const uint8_t data);
	uint8_t getSample() const;
	void reset();

	void decrementFrequencyTimer(int ticks);
private:
	uint8_t outputLevel = 0;
	int waveTableCounter = 0;

	void resetLengthCounter(uint8_t newLength);
	//FF30 - FF3F stores RAM for wave samples
	void resetFrequencyTimer();
};