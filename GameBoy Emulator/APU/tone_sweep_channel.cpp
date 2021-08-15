#include "tone_sweep_channel.h"

ToneSweepChannel::ToneSweepChannel(MemoryMap& mm): ToneChannel(mm) {
	NRRegisters[0] = 0xFF10;//sweep period
	NRRegisters[1] = 0xFF11;//duty info
	NRRegisters[2] = 0xFF12;//volume info
	NRRegisters[3] = 0xFF13;//lo frequency, written by ROM automatically
	NRRegisters[4] = 0xFF14;//hi frequency + other info
	frequencyTimer = (2048 - getFrequency()) * 4;//put in separate init()?
	currentVolume = getInitialVolume();
	volumeTimer = getVolumePeriod();
}