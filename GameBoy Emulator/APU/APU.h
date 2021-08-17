#pragma once
#include <cstdint>
#include <array>
#include <memory>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include "../memory/memory_map.h"
#include "tone_channel.h"
#include "tone_sweep_channel.h"
#include "wave_channel.h"
#include "noise_channel.h"

class APU {
public:
	APU(MemoryMap& newmm);
	~APU() = default;
	void notifyRegistersWritten(const uint16_t address, const uint8_t byte);
private:
	MemoryMap& mm;

	std::unique_ptr<ToneSweepChannel> channel1;
	std::unique_ptr<ToneChannel> channel2;
	std::unique_ptr<WaveChannel> channel3;
	std::unique_ptr<NoiseChannel> channel4;

	const int frameSequencerMaxTicks = 8192;
	int frameSequencerTimer = 0;
	int frameSequencerStep = 0;

	const uint16_t channelControl = 0xFF24;
	const uint16_t soundOutputLocation = 0xFF25;
	const uint16_t soundEnabled = 0xFF26;

	void playBeeps();
	void incrementFrameSequencerTimer(int ticks);
	void advanceFrameSequencer();

	void decrementLengthCounters();
	void decrementVolumeTimers();
};