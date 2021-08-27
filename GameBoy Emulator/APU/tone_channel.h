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
	int dutyCycleLocation = 0;//can't be reset by any means, even with change 
							  //in selected duty cycle

	bool getDutyCycleLocation() const;
	void incrementDutyCycleLocation();

	
};