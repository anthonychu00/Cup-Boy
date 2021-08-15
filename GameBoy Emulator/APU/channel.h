#pragma once
#include <array>
#include <cstdint>
#include "../memory/memory_map.h"
#include "../utils/utils.h"
class Channel {
public:
	Channel() = default;
	Channel(MemoryMap& newmm);
	~Channel() = default;
protected:
    MemoryMap& mm;
	int frequencyTimer = 0, volumeTimer = 0;
	int currentVolume = 0;

	//NR0 = ch1 - sweep, ch3 - enabler
	//NR1 = sound wave length, duty
	//NR2 = volume
	//NR3 = frequency LSB, ch4 - clock shift
	//NR4 = frequency MSB, trigger, length enable
	std::array<uint16_t, 5> NRRegisters;

	void decrementVolumeTimer();
	int getInitialVolume() const;
	bool getVolumeDirection() const;//0 = decrease, 1 = increase
	int getVolumePeriod() const;
	void incrementVolume();
	void decrementVolume();
	
	uint16_t getFrequency() const;
	void decrementFrequencyTimer(int ticks);
	void resetFrequencyTimer();

};