#pragma once
#include "channel.h"

class NoiseChannel : public Channel {
public:
	NoiseChannel(MemoryMap& mm);

	void handleWrittenRegister(const uint16_t address, const uint8_t data);
	uint8_t getSample() const;
	void decrementFrequencyTimer(int ticks);
private:
	uint16_t LSFR = 0x7FFF;
	bool widthMode = false;//0 = 15 bits, 1 = 7 bits

	void reset();
	void resetFrequencyPeriod(uint8_t data);
	void resetFrequencyTimer();
	void stepLSFR();
};