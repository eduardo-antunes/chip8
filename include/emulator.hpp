/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#ifndef CHIP8_EMULATOR_HPP
#define CHIP8_EMULATOR_HPP

#include <cstdint>
#include <cstring>
#include <vector>

#include "io_handler.hpp"

namespace chip8 {
    class Emulator {
        public:
            Emulator();
            Emulator(const std::vector<uint8_t> &code) : Emulator() { load_code(code); }
            void load_code(const std::vector<uint8_t> &code);

            int step();
            int run();
            int run_debug();
            void show_mem() const;
            void show_regs() const;
            void show_current_instruction() const;

        private: 
            // CPU state:
            uint8_t mem[4096] = {0};
            uint16_t pc, index_reg;
            uint8_t v[16], delay_timer = 0, sound_timer = 0;
            std::vector<uint16_t> stack;

            void set_flag()   { v[0xF] = 1; }
            void clear_flag() { v[0xF] = 0; }

            // Input and output:
            IO_handler io;
    };
}

#endif // CHIP8_EMULATOR_HPP
