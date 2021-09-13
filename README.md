# Cup-Boy

A journey to build my own GameBoy emulator in C++. 

Game are now playable!

Zelda demo : https://youtu.be/k6hQsjLdoiY

Pokemon Demo: https://youtu.be/0VYCkKw7jg4

For some reason, Zelda doesn't like being recorded with all 4 audio channels playing, so recording is with some of the channels turned off

Main tools used:
- SDL2 : Window creation and management, audio, input handling
- OpenGL : rendering backend (2D textures)
- ImGUI : GUI, debugging tools


Implemented:
- Working CPU/MMU that passes Blargg tests, with instruction level timing
- Cartridges (MBC0, MBC1, MBC3)
- PPU using Pixel FIFO algorithm, passes acid2 tests
- Audio (all channels, passes some Blargg tests)
- Joypad support
- File explorer to enable choosing/switching roms from user directory
- Debugging tools to view VRAM data and trace the execution of opcodes

To-do list:

High priority:
- Add support for other MBCs
- Persistent .sav data

Nice to have:
- Memory level timing
- Linkports
- CGB
- debugging some graphical/audio quirks

# Usage

Run the .exe file. All necessary .dll files are included. If the program doesn't work, check if you have the dependencies for SDL and OpenGL. Only games that run on MBC0, MBC1, and MBC3 are supported right now, so make sure your ROM is one of those MBC types.

# Controls

D-pad : arrow keys

Start : S

Select : A

A : Z

B : X
