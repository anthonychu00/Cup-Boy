#pragma once
#include <memory>
#include <iostream>
#include "CPU/CPU.h"
#include "memory/memory_map.h"
#include "video/video.h"
#include "Cartridge/cartridge.h"
#include "joypad/joypad.h"
#include "APU/APU.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imfilebrowser.h"

class System {
public:
	System();
	~System() = default;

	void start();
private:
	const int CYCLE_REFRESH = 69905;

	std::string path = "";

	//window variables
	int screenScale = 4;
	const float menuBarHeight = 19;

	Cartridge cartridge;
	CPU cpu;
	MemoryMap mm;
	Video video;
	Joypad joypad;
	APU apu;

	void reset();
	void preloadRom();
	inline void showMenuBar(SDL_Window* gbWindow, ImGui::FileBrowser& fileDialog);
};