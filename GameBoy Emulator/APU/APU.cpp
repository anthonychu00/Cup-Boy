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
}

void APU::notifyRegistersWritten(const uint16_t address, const uint8_t byte) {
	uint8_t newLength = byte & 0x3F;
	switch (address) {
	case 0xFF11: channel1->resetLengthCounter(newLength); break;//channel 1 and so on, reset frequency counter and enable
	case 0xFF16: channel2->resetLengthCounter(newLength); break;
	case 0xFF1B: channel3->resetLengthCounter(newLength); break;
	case 0xFF20: channel4->resetLengthCounter(newLength); break;
	}
}

void APU::tick(int ticks) {
	if (sampleTimer >= samplingRate) {
		sampleTimer -= samplingRate;
		std::array<float, 4> DACValues = getSamples();
		std::pair<float, float> terminals = mixSamples(DACValues);
		amplifyTerminals(terminals);
	}
	if ( samples.size() >= maxSamples) {
		SDL_QueueAudio(1, static_cast<void*>(samples.data()), maxSamples);
		samples.clear();//optimize?
	}
	incrementFrameSequencerTimer(ticks);
	sampleTimer += ticks;
}

channelArray APU::getSamples() {
	std::array<float, 4> DACValues;
	//get volume bytes [0, 15] -> [1, -1]
	DACValues[0] = channel1->getSample();
	DACValues[1] = channel2->getSample();
	
	return DACValues;
}

std::pair<float, float> APU::mixSamples(channelArray values) {
	float leftTerminal = 0.0f, rightTerminal = 0.0f;
	uint8_t outputLocations = mm.readAddress(soundOutputLocation);
	
	if (getBit(outputLocations, 0)) {
		//channel 1 to left terminal
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
	
	//amplify terminals by amp level
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
		case 6: decrementLengthCounters(); break; //sweep clocks here
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