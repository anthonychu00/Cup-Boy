#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "system.h"

//more efficient to use initializer lists as opposed to in body assignment
System::System() : 
	cartridge(), 
	cpu(mm),
	apu(mm),
	mm(cpu, apu, joypad),
	video(cpu, mm),
	joypad(cpu)	
{
	SDL_Init(SDL_INIT_EVERYTHING);
}

void System::start() {
	//preloadRom();
	SDL_Window* gbWindow = SDL_CreateWindow(
		"CupBoy",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		160 * screenScale,
		(144 * screenScale) + menuBarHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
	);

	video.initializeSDL(gbWindow, screenScale, menuBarHeight);

	ImGui::FileBrowser fileDialog (ImGuiFileBrowserFlags_NoModal);
	fileDialog.SetTypeFilters({ ".gb" });

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

		if (path.compare("") == 0) {
			showMenuBar(gbWindow, fileDialog);
		}
		else {
			while (cycles < CYCLE_REFRESH) {
				int previousTicks = cpu.executeOperations();
				cpu.updateTimer(previousTicks);
				video.tick(previousTicks);
				apu.tick(previousTicks);
				cpu.checkInterruptRequests();
				cycles += previousTicks;
			}


			showMenuBar(gbWindow, fileDialog);
			video.renderFrameBuffer();
			cycles -= CYCLE_REFRESH;
		}
		
	}
}

void System::reset() {
	cpu.reset();
	mm.resetMemory();
	video.reset();
	apu.reset();
}

void System::preloadRom() {
	path = "../../../../gb-test-roms/Zelda.gb";
	//std::string path = "../../../../gb-test-roms/Mario.gb";
	//std::string path = "../../../../gb-test-roms/Pokemon.gb";
	//std::string path = "../../../../gb-test-roms/Mega Man.gb";
	cartridge.loadRom(path);
	mm.loadCartridgeToMemory(cartridge.MBCFactory());
}

void System::showMenuBar(SDL_Window* gbWindow, ImGui::FileBrowser& fileDialog) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::SetNextWindowPos({ -1, menuBarHeight });
	ImGui::Begin("T", nullptr,
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open ROM"))
			{
				fileDialog.Open();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	ImGui::End();
	
	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
		if (path.compare("") != 0) {
			reset();
		}
		path = fileDialog.GetSelected().string();
		cartridge.loadRom(path);
		mm.loadCartridgeToMemory(cartridge.MBCFactory());
		fileDialog.ClearSelected();
	}
	
	ImGui::Render();
	glViewport(0, menuBarHeight, 160 * screenScale, 144 * screenScale);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (path.compare("") == 0) {
		SDL_GL_SwapWindow(gbWindow);
	}
	
}

