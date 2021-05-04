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

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}
		int previousTicks = cpu.executeOperations();
		cpu.updateTimer(previousTicks);
		cpu.checkInterruptRequests();
		video.tick(previousTicks);
	}

	/*while (true) {
		int previousTicks = cpu.executeOperations();
		cpu.updateTimer(previousTicks);
		cpu.checkInterruptRequests();
	}*/
}