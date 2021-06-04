# Cup-Boy

A journey to build my own GameBoy emulator in C++. I'm just aiming in the finished product to be able to run a few games with minimal issues. A fully working emulator would need to account for a wide variety of tilesets and other gotchas, since no game is built the same.

Game are now playable!

Zelda demo : https://youtu.be/qiKBEbkgWC0

Implemented:
- Working CPU/MMU that passes Blargg tests, with instruction level timing
- Cartridges (MBC0, MBC1)
- PPU using Pixel FIFO algorithm in SDL2, passes acid2 tests
- Joypad support
- Debugging tools to view VRAM data and trace the execution of opcodes

To-do list:
High priority:
- implement sound
- Add support for other MBCs
- Persistent .sav data

Nice to have:
- Linkports
- CGB
