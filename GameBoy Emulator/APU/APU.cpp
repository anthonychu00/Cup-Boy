#include <iostream>
#include "APU.h"

APU::APU(MemoryMap& newmm) :
	mm(newmm)
{
	initializeSDL();

	channel1 = std::make_unique<ToneSweepChannel>(mm);
	channel2 = std::make_unique<ToneChannel>(mm);
	channel3 = std::make_unique<WaveChannel>(mm);
	channel4 = std::make_unique<NoiseChannel>(mm);
}

void APU::initializeSDL() {
	SDL_Init(SDL_INIT_AUDIO);
	SDL_AudioSpec want, have;
	SDL_AudioDeviceID dev;

	SDL_memset(&want, 0, sizeof(want));
	want.freq = 44100;
	want.format = AUDIO_F32;
	want.channels = 2;
	want.samples = maxSamples;
	want.callback = NULL;

	dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (dev == 0) {
		SDL_Log("Couldn't open audio: %s", SDL_GetError());
	}
	else {
		if (have.format != want.format) {
			SDL_Log("Couldn't find format");
		}
	}
	SDL_PauseAudioDevice(dev, 0);
	myDev = dev;
}

void APU::notifyRegistersWritten(const uint16_t address, const uint8_t byte) {
	
	if (address >= 0xFF10 && address <= 0xFF14) {
		channel1->handleWrittenRegister(address, byte);
	}
	else if (address >= 0xFF16 && address <= 0xFF19) {
		channel2->handleWrittenRegister(address, byte);
	}
}

void APU::tick(int ticks) {
	if (sampleTimer >= samplingRate) {
		sampleTimer -= samplingRate;
		std::array<float, 4> DACValues = getSamples();
		std::pair<float, float> terminals = mixSamples(DACValues);
		amplifyTerminals(terminals);

		
		samples.push_back(terminals.first);
		samples.push_back(terminals.second);
	}
	if ( samples.size() >= maxSamples) {
		//empty queue until it reaches a size less than buffer, prevents sound from going onto screen transitions
		while ((SDL_GetQueuedAudioSize(myDev)) > maxSamples * bytesPerSample) {
			SDL_Delay(1);
		}
		//clear buffer/queue and add new samples
		SDL_QueueAudio(myDev, static_cast<void*>(samples.data()), maxSamples * bytesPerSample);
		samples.clear();//optimize?
	}
	incrementFrameSequencerTimer(ticks);
	channel1->decrementFrequencyTimer(ticks);
	channel2->decrementFrequencyTimer(ticks);
	sampleTimer += ticks;

}

channelArray APU::getSamples() {
	channelArray DACValues;
	//get volume bytes [0, 15] -> [1, -1]
	DACValues[0] = convertToDAC(channel1->getSample());
	DACValues[1] = convertToDAC(channel2->getSample());
	DACValues[2] = convertToDAC(channel3->getSample());
	DACValues[3] = convertToDAC(channel4->getSample());

	return DACValues;
}

std::pair<float, float> APU::mixSamples(channelArray values) {
	float leftTerminal = 0.0f, rightTerminal = 0.0f;
	uint8_t outputLocations = mm.readAddress(soundOutputLocation);
	if (getBit(outputLocations, 0)) {
		leftTerminal += values[0];
	}
	if (getBit(outputLocations, 4)) {
		rightTerminal += values[0];
	}
	if (getBit(outputLocations, 1)) {
		leftTerminal += values[1];
	}
	if (getBit(outputLocations, 5)) {
		rightTerminal += values[1];
	}

	return std::make_pair(leftTerminal, rightTerminal);
}

void APU::amplifyTerminals(std::pair<float, float>& terminals) {
	uint8_t control = mm.readAddress(channelControl);
	uint8_t leftAmp = control & 0x7; //SO1
	uint8_t rightAmp = (control >> 4) & 0x7; //SO2
	
	terminals.first *= (leftAmp + 1);//+1 the amp?
	terminals.second *= (rightAmp + 1);
}

float APU::convertToDAC(uint8_t volumeLevel) {
	float analogValue = 0; 
	if (volumeLevel < 7) {
		analogValue = (7.0 - static_cast<float>(volumeLevel)) / 7.0;
	}
	else {
		analogValue = (static_cast<float>(volumeLevel) + 1.0 - 7.0) / 9.0;
	}
	
	return analogValue;
}

void APU::incrementFrameSequencerTimer(int ticks) {
	frameSequencerTimer += ticks;
	if (frameSequencerTimer >= frameSequencerMaxTicks) {
		frameSequencerTimer -= frameSequencerMaxTicks;
		advanceFrameSequencer();
	}
}

void APU::advanceFrameSequencer() {
	switch (frameSequencerStep) {
		case 0:
		case 4: decrementLengthCounters(); break;
		case 2:
		case 6: channel1->decrementSweepTimer(); decrementLengthCounters(); break; //sweep clocks here
		case 7: decrementVolumeTimers(); break;

	}

	frameSequencerStep++;
	if (frameSequencerStep >= 8) {
		frameSequencerStep = 0;
	}
}

void APU::decrementLengthCounters() {
	channel1->decrementLengthCounter();
	channel2->decrementLengthCounter();
	channel3->decrementLengthCounter();
	channel4->decrementLengthCounter();
}

void APU::decrementVolumeTimers() {
	channel1->decrementVolumeTimer();
	channel2->decrementVolumeTimer();
	channel3->decrementVolumeTimer();//special behavior?
	channel4->decrementVolumeTimer();
}