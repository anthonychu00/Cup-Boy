#include "joypad.h"

Joypad::Joypad(CPU& newCpu) :
	cpu(newCpu){
}

uint8_t Joypad::getState() {
	if (!getBit(currentState, 4)) {
		return directionActionNibbles & 0xF;
	}
	else if (!getBit(currentState, 5)) {
		return (directionActionNibbles >> 4) & 0xF;
	}
	return directionActionNibbles & 0xF;
}

void Joypad::writeButtonMode(uint8_t byte) {
	setBit(currentState, 4, getBit(byte, 4));
	setBit(currentState, 5, getBit(byte, 5));
}

void Joypad::buttonPressed(int button) {
	setBit(directionActionNibbles, button, 0);
	//cpu.setInterruptFlag(4);
}

void Joypad::buttonReleased(int button) {
	setBit(directionActionNibbles, button, 1);
}

void Joypad::handleInput(SDL_Event& e) {
	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
		case SDLK_RIGHT: buttonPressed(0); break;
		case SDLK_LEFT: buttonPressed(1); break;
		case SDLK_UP: buttonPressed(2); break;
		case SDLK_DOWN: buttonPressed(3); break;
		case SDLK_z: buttonPressed(4); break;
		case SDLK_x: buttonPressed(5); break;
		case SDLK_a: buttonPressed(6); break; //select
		case SDLK_s: buttonPressed(7); break; //start
		}
	}
	else if (e.type == SDL_KEYUP) {
		switch (e.key.keysym.sym) {
		case SDLK_RIGHT: buttonReleased(0); break;
		case SDLK_LEFT: buttonReleased(1); break;
		case SDLK_UP: buttonReleased(2); break;
		case SDLK_DOWN: buttonReleased(3); break;
		case SDLK_z: buttonReleased(4); break;
		case SDLK_x: buttonReleased(5); break;
		case SDLK_a: buttonReleased(6); break;
		case SDLK_s: buttonReleased(7); break;
		}
	}
}