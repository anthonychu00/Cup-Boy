#pragma once
#include "channel.h"

class NoiseChannel : public Channel {
public:
	NoiseChannel(MemoryMap& mm);
	uint8_t getSample() const;
private:

};