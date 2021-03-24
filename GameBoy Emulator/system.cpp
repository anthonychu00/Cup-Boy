#include "system.h"

//more efficient to use initializer lists as opposed to in body assignment
System::System(string path) : 
	cartridge(path), 
	cpu(mm),
	mm(cpu, cartridge),
	video(cpu, mm){
}

void System::start() {
	while (true) {
		int previousTicks = cpu.executeOperations();
		cpu.updateTimer(previousTicks);
		cpu.checkInterruptRequests();
	}
}