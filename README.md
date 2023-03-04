# Yet another Chip-8 emulator

Chip-8 is a low-level, interpreted programming language made in the mid-1970s by Joseph Weisbecker. Its original purpose was to facilitate game development for the COSMAC VIP and Telmac 1800 computers. Today, despite both of these having long become obsolete, Chip-8 remains relevant for its simplicity and low-level nature. Building an emulator (or, more accuratelly, an interpreter) capable of running it is considered to be kind of the "Hello, World" of emulation. You can find out more about this language [here](https://en.wikipedia.org/wiki/CHIP-8).

This particular emulator was written in C++ 17, using SDL2 for graphics, sound and user input. It supports Chip-8's instruction set in its entirety. For ambiguous instructions (that is, those that have historically had different behaviors in different implementations of the language), it mostly follows the conventions stablished by more modern interpreters, such as Chip-48 and Super-Chip; consult the [source code](https://github.com/eduardo-antunes/chip8/blob/main/src/emulator.cpp#L111) to view exactly which instructions received this treatment.

# Installation

To compile this emulator, you will need a C++ compiler that supports C++ 17, meson, a meson backend (such as ninja), and SDL2 development files. Once all of them are installed, you can get the emulator binaries with the following commands:

```sh
$ git clone https://github.com/eduardo-antunes/chip8 chip8-emulator
$ cd chip8-emulator
$ meson setup build
# If your installation of meson is of version 0.54.0 or later:
$ meson compile build
# Otherwise, you'll have to use a meson backend directly. For ninja, you'd have to run:
$ ninja -C build
```

# Thanks

Without Tobias V. Langhoff's [comprehensive guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) on Chip-8 emulation, this emulator would not have left the paper. Check it out if you're interested in this sort of thing, it is really good. I also made sporadic use of Cowgod's [technical reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM), which is also very complete and well-written.

# License

```
Copyright 2023 Eduardo Antunes dos Santos Vieira

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. 
```
