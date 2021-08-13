#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "system.h"

//more efficient to use initializer lists as opposed to in body assignment
System::System(std::string path) : 
	cartridge(path), 
	cpu(mm),
	mm(cpu, joypad, cartridge.MBCFactory()),
	video(cpu, mm),
	joypad(cpu),
	apu(mm)
{

}

void System::start() {
	SDL_Event e;
	bool quit = false;
	int cycles  = 0;
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
				joypad.handleInput(e);
			}
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		while (cycles < CYCLE_REFRESH) {
			int previousTicks = cpu.executeOperations();
			cpu.updateTimer(previousTicks);
			video.tick(previousTicks);
			cpu.checkInterruptRequests();
			cycles += previousTicks;
		}
		video.renderFrameBuffer();
		cycles -= CYCLE_REFRESH;
		

	}

}