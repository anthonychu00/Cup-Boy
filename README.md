# Cup-Boy

A journey to build my own GameBoy emulator in C++. I'm just aiming in the finished product to be able to run a few games with minimal issues. A fully working emulator would need to account for a wide variety of tilesets and other gotchas, since no game is built the same.

To do list:

- Finish writing opcodes for the CPU (done)
- Write the memory unit for the CPU to interact with other components (done except for some special routines for some registers)
- Use SDL2 to emulate the GPU components (researching)
- Figure out a model for timing and synchronization (in progress)
- Make sure everything runs against test roms (in progress)
- Implement sound
