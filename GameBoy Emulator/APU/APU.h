#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include <utility>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include "../memory/memory_map.h"
#include "tone_channel.h"
#include "tone_sweep_channel.h"
#include "wave_channel.h"
#include "noise_channel.h"
#include "../utils/utils.h"

using channelArray = std::array<float, 4>;

class APU {
public:
	APU(MemoryMap& newmm);
	~APU() = default;
	void notifyRegistersWritten(const uint16_t address, const uint8_t byte);
	void tick(int ticks);
private:
	MemoryMap& mm;
	SDL_AudioDeviceID myDev;

	std::vector<float> samples;
	const int samplingRate = 88; //gather a sample every 88 CPU ticks
	const int maxSamples = 4096; //samples to gather before outputting them
	int sampleTimer = 0;
	const int bytesPerSample = sizeof(float);

	//add reference to a buffer
	std::unique_ptr<ToneSweepChannel> channel1;
	std::unique_ptr<ToneChannel> channel2;
	std::unique_ptr<WaveChannel> channel3;
	std::unique_ptr<NoiseChannel> channel4;

	void initializeSDL();

	channelArray getSamples();
	std::pair<float, float> mixSamples(channelArray values);
	void amplifyTerminals(std::pair<float, float>& terminals);
	float convertToDAC(uint8_t volumeLevel);

	const int frameSequencerMaxTicks = 8192;
	int frameSequencerTimer = 0;
	int frameSequencerStep = 0;

	const uint16_t channelControl = 0xFF24;
	const uint16_t soundOutputLocation = 0xFF25;
	const uint16_t soundEnabled = 0xFF26;

	void incrementFrameSequencerTimer(int ticks);
	void advanceFrameSequencer();

	void decrementLengthCounters();
	void decrementVolumeTimers();
};