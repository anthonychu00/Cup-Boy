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

	virtual void handleWrittenRegister(const uint16_t address, const uint8_t data) = 0;
	virtual uint8_t getSample() const = 0;
	void decrementVolumeTimer();
	void decrementLengthCounter();
	void decrementFrequencyTimer(int ticks);
	
protected:
    MemoryMap& mm;
	bool isDisabled = false, envelopeDisabled = false; //controlled by length counter
	int frequencyTimer = 0, currentVolume = 0, lengthCounter = 0;

	int volumeTimer = 0, volumePeriod = 0;
	bool volumeDirection = false;
	//NR0 = ch1 - sweep, ch3 - enabler
	//NR1 = sound wave length, duty
	//NR2 = volume
	//NR3 = frequency LSB, ch4 - clock shift
	//NR4 = frequency MSB, trigger, length enable
	std::array<uint16_t, 5> NRRegisters;

	virtual void reset() = 0;
	void resetVolume();
	void incrementVolume();
	void decrementVolume();

	void resetLengthCounter(uint8_t newLength);
	uint8_t getLengthData();
	bool lengthClocksEnabled();
	
	uint16_t getFrequency() const;
	void resetFrequencyTimer();

};