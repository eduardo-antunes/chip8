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

#include "display.hpp"

namespace chip8 {

    class Emulator {
        public:
            Emulator(void);
            Emulator(const std::vector<uint8_t> &code) : Emulator() { load_code(code); }

            void load_code(const std::vector<uint8_t> &code);
            int step(void);

            void show_mem(void) const;
            void show_regs(void) const;
            void show_current_instruction(void) const;
            void show_display(void) const { display.draw(); }

        private: 
            // Registers and stack:
            uint16_t pc, index_reg;
            uint8_t v[16], delay_timer = 0, sound_timer = 0;
            std::vector<uint16_t> stack;

            // Memory and display:
            uint8_t mem[4096] = {0};
            Display display;

            void set_flag(void)   { v[0xF] = 1; }
            void clear_flag(void) { v[0xF] = 0; }
    };

}

#endif // CHIP8_EMULATOR_HPP
