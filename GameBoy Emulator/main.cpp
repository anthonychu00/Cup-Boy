#include <memory>
#include "system.h"
#include "Cartridge/cartridge.h"
using namespace std;

int main() {
	
	//string path = "../../../../gb-test-roms/cpu_instrs/individual/01-special.gb";//get filepath
	//string path = "../../../../gb-test-roms/cpu_instrs/individual/02-interrupts.gb";
	//string path = "../../../../gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb";
	//string path = "../../../../gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb";
	//string path = "../../../../gb-test-roms/cpu_instrs/individual/06-ld r,r.gb";
	//string path = "../../../../gb-test-roms/cpu_instrs/individual/10-bit ops.gb";
	//string path = "../../../../gb-test-roms/cpu_instrs/cpu_instrs.gb";

	//string path = "../../../../gb-test-roms/instr_timing/instr_timing.gb";

	//string path = "../../../../gb-test-roms/mem_timing/mem_timing.gb"; //mem timings aren't strictly needed by many games
	//string path = "../../../../gb-test-roms/mem_timing/individual/01-read_timing.gb";

	//string path = "../../..";

	string path = "../../../../gb-test-roms/dmg-acid2.gb";
	//string path = "../../../../gb-test-roms/Tetris.gb";
	//string path = "../../../../gb-test-roms/boot.gb";

	unique_ptr<System> gb = make_unique<System>(path);
	gb->start();

	
	return 0;
}