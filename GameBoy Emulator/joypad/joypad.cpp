#include "joypad.h"

Joypad::Joypad(CPU& newCpu) :
	cpu(newCpu){
}

uint8_t Joypad::getState() {
	return currentState;
}
//******************************change for nibble changing
void Joypad::buttonPressed(int button) {
	printf("Button pressed: %d\n", button);
	//both action and direction being read at the same time, figure out why
	setBit(directionActionNibbles, button, 0);//directional buttons
	if (button < 4) {
		setLowerNibble(currentState, directionActionNibbles);
		setBit(currentState, 5, 1);
		setBit(currentState, 4, 0);
		directionsActive = true;
	}
	else if (button >= 4) {
		setLowerNibble(currentState, (directionActionNibbles >> 4));
		setBit(currentState, 4, 1);
		setBit(currentState, 5, 0);
		actionsActive = true;
	}
	printf("state: %d\n", currentState);
	printf("nibbles: %d\n", directionActionNibbles);
	cpu.setInterruptFlag(4);
}

void Joypad::buttonReleased(int button) {
	printf("Button released: %d\n", button);
	printf("pre-state: %d\n", currentState);
	setBit(directionActionNibbles, button, 1);
	if (button < 4) {
		setLowerNibble(currentState, directionActionNibbles);
	}
	else if (button >= 4) {
		setLowerNibble(currentState, (directionActionNibbles >> 4));
	}

	if ((directionActionNibbles & 0x0F) == 0x0F) {
		setBit(currentState, 4, 1);
		directionsActive = false;
		if (actionsActive) {//if pressing other category key interrupts, necessary?
			setBit(currentState, 5, 0);
			setLowerNibble(currentState, (directionActionNibbles >> 4));
		}
	}
	
	if (((directionActionNibbles >> 4) & 0x0F) == 0x0F) {
		setBit(currentState, 5, 1);
		actionsActive = false;
		if (directionsActive) {
			setBit(currentState, 4, 0);
			setLowerNibble(currentState, directionActionNibbles);
		}
	}
	printf("state: %d\n", currentState);
	printf("nibbles: %d\n", directionActionNibbles);
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