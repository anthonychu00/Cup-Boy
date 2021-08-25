#pragma once
#include "channel.h"

//Channel 2, Square Wave 2, Tone
class ToneChannel : public Channel {
public:
	ToneChannel(MemoryMap& mm);
	~ToneChannel() = default;

	uint8_t getSample() const;
	void decrementFrequencyTimer(int ticks);
protected:
	static const std::array<bool, 4 * 8> dutyCycleTable;
	int selectedDutyCycle = 0;
	int dutyCycleLocation = 0;

	void setDutyCycleLocation();
	bool getDutyCycleLocation();
	void incrementDutyCycleLocation();

	
};