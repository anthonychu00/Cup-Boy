#include "joypad.h"

Joypad::Joypad(CPU& newCpu) :
	cpu(newCpu){
}

uint8_t Joypad::getState() {
	return currentState;
}

void Joypad::buttonPressed(int button) {
	printf("Button pressed: %d\n", button);
	if (button < 4) { //directional buttons
		setBit(currentState, 4, 0);
		setBit(currentState, button, 0);
	}
	else if (button >= 4) {//action buttons
		setBit(currentState, 5, 0);
		setBit(currentState, button - 4, 0);
	}
	cpu.setInterruptFlag(4);
}

void Joypad::buttonReleased(int button) {
	printf("Button released: %d\n", button);
	if (button < 4) { //directional buttons
		setBit(currentState, 4, 1);
		setBit(currentState, button, 1);
	}
	else if (button >= 4) {//action buttons
		setBit(currentState, 5, 1);
		setBit(currentState, button - 4, 1);
	}


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