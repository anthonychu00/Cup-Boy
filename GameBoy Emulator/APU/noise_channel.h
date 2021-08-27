#pragma once
#include "channel.h"

class NoiseChannel : public Channel {
public:
	NoiseChannel(MemoryMap& mm);

	void handleWrittenRegister(const uint16_t address, const uint8_t data);
	uint8_t getSample() const;
	void reset();
private:

};