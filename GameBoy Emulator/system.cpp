#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "system.h"

//more efficient to use initializer lists as opposed to in body assignment
System::System(string path) : 
	cartridge(path), 
	cpu(mm),
	mm(cpu, cartridge),
	video(cpu, mm){
}

void System::start() {
	SDL_Event e;
	bool quit = false;
	int cycles  = 0;
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		while (cycles < CYCLE_REFRESH) {
			int previousTicks = cpu.executeOperations();
			//printf("Previous ticks: %d\n", previousTicks);
			cpu.updateTimer(previousTicks);
			video.tick(previousTicks);
			cpu.checkInterruptRequests();
			cycles += previousTicks;
		}
		//video.renderFrameBuffer();
		cycles = 0;

	}

}