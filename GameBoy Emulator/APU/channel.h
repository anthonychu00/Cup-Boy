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

	virtual uint8_t getSample () const = 0;
	void decrementVolumeTimer();
	void resetLengthCounter(uint8_t newLength);
	void decrementLengthCounter();
	void decrementFrequencyTimer(int ticks);
	
protected:
    MemoryMap& mm;
	bool isDisabled = false; //controlled by length counter
	int frequencyTimer = 0, volumeTimer = 0, currentVolume = 0, lengthCounter = 0;

	//NR0 = ch1 - sweep, ch3 - enabler
	//NR1 = sound wave length, duty
	//NR2 = volume
	//NR3 = frequency LSB, ch4 - clock shift
	//NR4 = frequency MSB, trigger, length enable
	std::array<uint16_t, 5> NRRegisters;

	int getInitialVolume() const;
	bool getVolumeDirection() const;//0 = decrease, 1 = increase
	int getVolumePeriod() const;
	void incrementVolume();
	void decrementVolume();

	uint8_t getLengthData();
	bool lengthClocksEnabled();
	
	uint16_t getFrequency() const;
	void resetFrequencyTimer();

};