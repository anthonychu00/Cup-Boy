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
	//length counter written to resets it
}

void APU::tick(int ticks) {
	if (sampleTimer >= samplingRate) {
		sampleTimer -= samplingRate;
		getSamples();
	}
	if ( samples.size() >= maxSamples) {
		SDL_QueueAudio(1, static_cast<void*>(samples.data()), maxSamples);
		samples.clear();//optimize?
	}
	incrementFrameSequencerTimer(ticks);
	sampleTimer += ticks;
}

void APU::getSamples() {
	//get samples from all channels, convert to DAC analogs, and mix them
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