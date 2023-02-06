/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#ifndef CHIP8_EMULATOR_HPP
#define CHIP8_EMULATOR_HPP

#include <cstdint>
#include <vector>
#include "screen.hpp"
#include "keypad.hpp"

// The real meat of the emulator

namespace chip8 {
    class Emulator {
        public:
            Emulator();
            Emulator(const std::vector<uint8_t> &code) : Emulator() { load_code(code); }
            void load_code(const std::vector<uint8_t> &code);
            int run();
        private:
            // font is in the address range 0x50 - 0x9F
            static const uint16_t font_addr = 0x50;
            // CPU state:
            uint8_t memory[4096] = {0}, v[16] = {0};
            uint8_t delay_timer = 0, sound_timer = 0;
            uint16_t pc, index_reg;
            std::vector<uint16_t> stack;

            void set_flag(int f) { v[15] = f ? 1 : 0; }
            int single_step();

            // Input and output:
            Screen screen;
            Keypad keys;
    };
}

#endif // CHIP8_EMULATOR_HPP
