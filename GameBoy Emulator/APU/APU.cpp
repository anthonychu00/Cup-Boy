#include "APU.h"

APU::APU(MemoryMap& newmm) :
	mm(newmm)
{
	channel1 = std::make_unique<ToneSweepChannel>(mm);
	channel2 = std::make_unique<ToneChannel>(mm);
	channel3 = std::make_unique<WaveChannel>(mm);
	channel4 = std::make_unique<NoiseChannel>(mm);
}

void APU::playBeeps() {
	
}

void APU::incrementFrameSequencerTimer(int ticks) {
	frameSequencerTimer += ticks;
	if (frameSequencerTimer >= frameSequencerMaxTicks) {
		frameSequencerTimer -= frameSequencerMaxTicks;
		advanceFrameSequencer();
	}
}

void APU::advanceFrameSequencer() {
	frameSequencerStep++;
	if (frameSequencerStep >= 8) {
		frameSequencerStep = 0;
	}
	//check steps to do in each part of the frame
}