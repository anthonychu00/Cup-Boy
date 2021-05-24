#pragma once
#include <cstdint>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "../CPU/CPU.h"
#include "../memory/memory_map.h"

class Joypad {
public:
	Joypad(CPU& newCpu);
	~Joypad() = default;
	uint8_t getState();
	void handleInput(SDL_Event& e);
private:
	uint8_t currentState = 0xFF;
	uint8_t directionActionNibbles = 0xFF; //top nibble is action, bottom nibble direction
	bool directionsActive = false;
	bool actionsActive = false;

	CPU& cpu;

	void buttonPressed(int button);
	void buttonReleased(int button);
};
