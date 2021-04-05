#pragma once
#include<cstdint>
#include<array>
//tiles are the base unit of graphics, consisting of an 8x8 square of pixels
using namespace std;
class Tile {
public:
	Tile(uint8_t location);
	uint16_t getTileRow(int row);
private:
	array<uint16_t, 8> tileRows = {};//a pixel is encoded by 2 bits, 2 * 8 = 16 bits
};