/*
 * Copyright 2023 Eduardo Antunes dos Santos Vieira
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License. 
 */

#ifndef CHIP8_EMULATOR_HPP
#define CHIP8_EMULATOR_HPP

#include <cstdint>
#include <vector>
#include "audio.hpp"
#include "screen.hpp"
#include "keypad.hpp"

// This class represents the emulator itself; the CPU, its state
// and its peripherals. It also includes a variety of utility functions.

namespace chip8 {
    class Emulator {
        public:
            Emulator();

            Emulator(const std::vector<uint8_t> &prog) 
                : Emulator() { load_prog(prog); }

            // Load program into RAM
            void load_prog(const std::vector<uint8_t> &prog);

            // Run currently loaded program
            int run();

        private:
            // Address of the built-in fontset
            static const uint16_t font_start = 0x50;

            // Starting address of the program
            static const uint16_t prog_start = 0x200;

            // 4KiB of RAM for program data
            uint8_t ram[4096] = {0};

            // Program register: contains the address of the next instruction
            // to be executed. Thus, it is used for flow control
            uint16_t pc = prog_start;

            // Index register: used to point at stuff in memory
            uint16_t index = 0;

            // Stack pointer and stack: for subroutines
            uint16_t stack[16] = {0};
            uint16_t sp = 0;

            // Delay timer: simply a general purpose timer. It should be
            // decremented at a constante rate of 60Hz
            uint8_t delay = 0;

            // Sound timer: behaves exactly like the delay timer, but while
            // it's non-zero, a sound is played
            uint8_t sound = 0;

            // Variable registers: 16 general purpose registers, except for the
            // last one, which serves as a flag register
            uint8_t v[16] = {0};

            // Graphics component
            Screen screen;

            // Set or clear the flag register, VF
            void set_flag(bool f) { v[0xF] = f ? 1 : 0; }

            // Update both timers
            void update_timers();

            // Single step through the program
            int single_step();

            Keypad keys;
            Audio audio;
    };
}

#endif // CHIP8_EMULATOR_HPP
