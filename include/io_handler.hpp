/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#ifndef CHIP8_IO_HANDLER_HPP
#define CHIP8_IO_HANDLER_HPP

#include <cstdint>
#include "screen.hpp"

// Input and output for the emulator, using SDL

namespace chip8 {
    enum class Io_event {
        NONE,
        KEYPRESS,
        QUIT,
    };
    class Io_handler {
        public:
            Screen screen;

            Io_handler();
            bool key_pressed() { return key_flag; }
            uint8_t get_key();
            Io_event poll_event();
            ~Io_handler();
        private:
            bool key_flag = false;
            uint8_t key_value = 0;
    };
}

#endif // CHIP8_IO_HANDLER_HPP
