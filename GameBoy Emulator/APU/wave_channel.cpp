#include "wave_channel.h"

WaveChannel::WaveChannel(MemoryMap& mm) : Channel(mm){
	NRRegisters[0] = 0xFF1A;//enabler
	NRRegisters[1] = 0xFF1B;//sound length
	NRRegisters[2] = 0xFF1C;//volume level
	NRRegisters[3] = 0xFF1D;//lo frequency, written by ROM automatically
	NRRegisters[4] = 0xFF1E;//hi frequency + other info
	frequencyTimer = (2048 - getFrequency()) * 4;//put in separate init()?
}

uint8_t WaveChannel::getSample() const {
	return 0;
}

void WaveChannel::handleWrittenRegister(const uint16_t address, const uint8_t data) {

}

void WaveChannel::reset() {

}