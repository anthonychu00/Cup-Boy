#include <memory>
#include "system.h"
#include "Cartridge/cartridge.h"

int main() {
	//setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	
	//std::string path = "../../../../gb-test-roms/cpu_instrs/cpu_instrs.gb";
	
	//std::string path = "../../../../gb-test-roms/instr_timing/instr_timing.gb";

	//string path = "../../../../gb-test-roms/mem_timing/mem_timing.gb"; //mem timings aren't strictly needed by many games
	//string path = "../../../../gb-test-roms/mem_timing/individual/01-read_timing.gb";

	//std::string path = "../../../../gb-test-roms/dmg_sound/dmg_sound.gb";
	
	//std::string path = "../../../../gb-test-roms/dmg-acid2.gb";
	//std::string path = "../../../../gb-test-roms/Tetris.gb";
	std::string path = "../../../../gb-test-roms/Zelda.gb";
	//std::string path = "../../../../gb-test-roms/Mario.gb";
	//std::string path = "../../../../gb-test-roms/Pokemon.gb";
	//std::string path = "../../../../gb-test-roms/Mega Man.gb";
	//string path = "../../../../gb-test-roms/boot.gb";

	std::unique_ptr<System> gb = std::make_unique<System>(path);
	gb->start();

	
	return 0;
}