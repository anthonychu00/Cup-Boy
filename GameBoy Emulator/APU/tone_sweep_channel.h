#pragma once
#include "tone_channel.h"

//Channel 1, Square Wave 1, Tone and Sweep
class ToneSweepChannel : public ToneChannel {
public:
	ToneSweepChannel(MemoryMap& mm);

	void handleWrittenRegister(uint16_t address, uint8_t data);
	void decrementSweepTimer();
private:
	bool sweepDisabled = false, sweepDirection = false;
	int sweepTimer = 0, sweepPeriod = 0, sweepShift = 0;

	uint16_t frequencyShadowRegister = 0;

	void reset() override;
	void resetSweep();

	void setFrequency(uint16_t newFrequency);
	void frequencyCalculation();
};