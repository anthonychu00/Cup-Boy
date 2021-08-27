#pragma once
#include "channel.h"

//Channel 2, Square Wave 2, Tone
class ToneChannel : public Channel {
public:
	ToneChannel(MemoryMap& mm);
	~ToneChannel() = default;

	void handleWrittenRegister(uint16_t address, uint8_t data) override;
	void reset() override;
	uint8_t getSample() const override;
	void decrementFrequencyTimer(int ticks);
protected:
	static const std::array<bool, 4 * 8> dutyCycleTable;
	int dutyCycleLocation = 0;//can't be reset with change 
							  //in selected duty cycle, only reset trigger

	bool getDutyCycleLocation() const;
	void incrementDutyCycleLocation();

	
};