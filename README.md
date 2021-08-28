# Cup-Boy

A journey to build my own GameBoy emulator in C++. 

Game are now playable!

Zelda demo : https://youtu.be/Z52HkgKxp34

Pokemon Demo: https://youtu.be/0VYCkKw7jg4

For some reason, Zelda doesn't like being recorded with all 4 audio channels playing, so that only has a few of the channels
Implemented:
- Working CPU/MMU that passes Blargg tests, with instruction level timing
- Cartridges (MBC0, MBC1)
- PPU using Pixel FIFO algorithm in SDL2, passes acid2 tests
- Audio (all channels, passes some Blargg tests)
- Joypad support
- Debugging tools to view VRAM data and trace the execution of opcodes

To-do list:

High priority:
- Add support for other MBCs
- Persistent .sav data

Nice to have:
- Linkports
- CGB
