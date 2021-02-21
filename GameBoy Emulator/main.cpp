#include <memory>
#include "system.h"
#include "Cartridge/cartridge.h"
using namespace std;

int main() {

	unique_ptr<System> gb = make_unique<System>();//might change arguments later
	gb->start();
	return 0;
}