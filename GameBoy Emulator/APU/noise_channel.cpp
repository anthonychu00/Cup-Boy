#include "noise_channel.h"

NoiseChannel::NoiseChannel(MemoryMap& mm): Channel(mm) {
	NRRegisters[0] = 0xFF1F;//not used
	NRRegisters[1] = 0xFF20;//sound length
	NRRegisters[2] = 0xFF21;//volume info
	NRRegisters[3] = 0xFF22;//polynomial counter
	NRRegisters[4] = 0xFF23;//consecutive counter
	currentVolume = getInitialVolume();
	volumeTimer = getVolumePeriod();
}