#pragma once
#include "tone_channel.h"

//Channel 1, Square Wave 1, Tone and Sweep
class ToneSweepChannel : public ToneChannel {
public:
	ToneSweepChannel(MemoryMap& mm);

	void handleWrittenRegister(uint16_t address, uint8_t data);
	void decrementSweepTimer();
private:
	bool sweepDisabled = false;
	int sweepTimer = 0;
	uint16_t frequencyShadowRegister = 0;

	void reset() override;
	void resetSweep();

	void setFrequency(uint16_t newFrequency);
	void frequencyCalculation();
	//sweep register functions
	uint8_t getSweepTime() const;
	bool getSweepDirection() const;
	uint8_t getSweepShift() const;
};