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

#ifndef CHIP8_KEYPAD_HPP
#define CHIP8_KEYPAD_HPP

#include <cstdint>
#include <optional>

// Handles user input

namespace chip8 {
    class Keypad {
        public:
            // Handles user input
            int handle();

            // Check if a certain key is pressed
            bool is_pressed(uint8_t k) const;

            // If a key is pressed, get it
            std::optional<uint8_t> get_pressed() const;

        private:
            // Abstract representation of the keypad
            bool keys[16] = {0};
    };
}

#endif // CHIP8_KEYPAD_HPP
