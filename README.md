# Cup-Boy

A journey to build my own GameBoy emulator in C++. 

Game are now playable!

Zelda demo : https://www.youtube.com/watch?v=k6hQsjLdoiY

Implemented:
- Working CPU/MMU that passes Blargg tests, with instruction level timing
- Cartridges (MBC0, MBC1)
- PPU using Pixel FIFO algorithm in SDL2, passes acid2 tests
- audio (only channel 2 currently)
- Joypad support
- Debugging tools to view VRAM data and trace the execution of opcodes

To-do list:

High priority:
- Add support for other MBCs
- Persistent .sav data

Nice to have:
- Linkports
- CGB
