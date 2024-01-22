# Yet another CHIP-8 emulator

CHIP-8 is a low-level, interpreted programming language made in the mid-1970s by Joseph Weisbecker. Its original purpose was to facilitate game development for
the COSMAC VIP and Telmac 1800 computers. Today, despite both of these having long become obsolete, CHIP-8 remains relevant for its simplicity and low-level
nature. Building an emulator (or, more accuratelly, an interpreter) capable of running it is considered to be kind of the "Hello, World" of emulation. You can
find out more about this language [here](https://en.wikipedia.org/wiki/CHIP-8).

This particular emulator is written in Go (previously C++), using SDL2 and its Go bindings for graphics, sound and user input.

# Instalation

To compile this emulator, you will need the standard Go compiler (1.21) and SDL2 (including development files). Once those are properly installed, downloading
and compiling the emulator is pretty simple.

```sh
# To install go and SDL2 on Fedora:
$ sudo dnf install SDL2-devel
# To install it on Ubuntu:
$ sudo apt-get install libsdl2-dev
# Compiling the emulator
$ git clone https://github.com/eduardo-antunes/chip8
$ cd chip8
$ go mod tidy && go build
```

# Thanks

During the development, I made use of some resources without which this thing would not have left the paper. In particular, I'd like to highlight:

* Tobias V. Langhoff's [comprehensive guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) on CHIP-8 emulation
* Cowgod's [technical reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* This [blog post](https://blog.tigris.fr/2020/05/13/writing-an-emulator-sound-is-complicated/) on audio generation in SDL2

My sincere thanks to all the authors.

# License

```
Copyright 2024 Eduardo Antunes dos Santos Vieira

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
