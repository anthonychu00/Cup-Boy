#pragma once
#include "tone_channel.h"

//Channel 1, Square Wave 1, Tone and Sweep
class ToneSweepChannel : public ToneChannel {
public:
	ToneSweepChannel(MemoryMap& mm);
private:
	//sweep register functions
};