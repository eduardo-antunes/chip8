/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#ifndef CHIP8_KEYPAD_HPP
#define CHIP8_KEYPAD_HPP

#include <cstdint>

// Handles user input

namespace chip8 {
    class Keypad {
        public:
            int handle();
            bool any_pressed() const;
            bool is_pressed(uint8_t key) const { return key_state[key]; }
            uint8_t get_key() const;
        private:
            bool key_state[16] = {0};
    };
}

#endif // CHIP8_KEYPAD_HPP
