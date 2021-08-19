#pragma once
#include "channel.h"

//Channel 3, Sine Waves, Wave Output
class WaveChannel : public Channel {
public:
	WaveChannel(MemoryMap& mm);

	uint8_t getSample() const;
private:
	//FF30 - FF3F stores RAM for wave samples
};