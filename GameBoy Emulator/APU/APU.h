#pragma once
#include <cstdint>
#include "../memory/memory_map.h"
class APU {
public:
	APU(MemoryMap& mm);
	~APU() = default;

private:
	MemoryMap& mm;

	const uint16_t CH1_sweep = 0xFF10;
	const uint16_t CH1_wave = 0xFF11;
	const uint16_t CH1_volume = 0xFF12;
	const uint16_t CH1_lo_frequency = 0xFF13;
	const uint16_t CH1_hi_frequency = 0xFF14;

	const uint16_t CH2_wave = 0xFF16;
	const uint16_t CH2_volume = 0xFF17;
	const uint16_t CH2_lo_frequency = 0xFF18;
	const uint16_t CH2_hi_frequency = 0xFF19;

	const uint16_t CH3_enable = 0xFF1A;
	const uint16_t CH3_length = 0xFF1B;
	const uint16_t CH3_level = 0xFF1C;
	const uint16_t CH3_lo_frequency = 0xFF1D;
	const uint16_t CH3_hi_frequency = 0xFF1E;

	//FF30 - FF3F stores RAM for wave samples

	const uint16_t CH4_length = 0xFF20;
	const uint16_t CH4_volume = 0xFF21;
	const uint16_t CH4_poly_counter = 0xFF22;
	const uint16_t CH4_consec_counter = 0xFF23;

	const uint16_t channelControl = 0xFF24;
	const uint16_t soundOutputLocation = 0xFF25;
	const uint16_t soundEnabled = 0xFF26;
};