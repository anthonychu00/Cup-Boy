#include <memory>
#include "system.h"
#include "Cartridge/cartridge.h"
using namespace std;

int main() {

	string path = "../../../../gb-test-roms/cpu_instrs/individual/01-special.gb";//get filepath
	//string path = "../../..";
	unique_ptr<System> gb = make_unique<System>(path);
	//gb->start();
	return 0;
}