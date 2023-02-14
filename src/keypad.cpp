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

#include <SDL2/SDL.h>
#include "keypad.hpp"

using namespace chip8;

int Keypad::handle() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                return 1; // should quit
            case SDL_KEYDOWN:
                switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_1: key_state[0x1] = true; break;
                    case SDL_SCANCODE_2: key_state[0x2] = true; break;
                    case SDL_SCANCODE_3: key_state[0x3] = true; break;
                    case SDL_SCANCODE_4: key_state[0xC] = true; break;
                    case SDL_SCANCODE_Q: key_state[0x4] = true; break;
                    case SDL_SCANCODE_W: key_state[0x5] = true; break;
                    case SDL_SCANCODE_E: key_state[0x6] = true; break;
                    case SDL_SCANCODE_R: key_state[0xD] = true; break;
                    case SDL_SCANCODE_A: key_state[0x7] = true; break;
                    case SDL_SCANCODE_S: key_state[0x8] = true; break;
                    case SDL_SCANCODE_D: key_state[0x9] = true; break;
                    case SDL_SCANCODE_F: key_state[0xe] = true; break;
                    case SDL_SCANCODE_Z: key_state[0xA] = true; break;
                    case SDL_SCANCODE_X: key_state[0x0] = true; break;
                    case SDL_SCANCODE_C: key_state[0xB] = true; break;
                    case SDL_SCANCODE_V: key_state[0xF] = true; break;
                    default: break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_1: key_state[0x1] = false; break;
                    case SDL_SCANCODE_2: key_state[0x2] = false; break;
                    case SDL_SCANCODE_3: key_state[0x3] = false; break;
                    case SDL_SCANCODE_4: key_state[0xC] = false; break;
                    case SDL_SCANCODE_Q: key_state[0x4] = false; break;
                    case SDL_SCANCODE_W: key_state[0x5] = false; break;
                    case SDL_SCANCODE_E: key_state[0x6] = false; break;
                    case SDL_SCANCODE_R: key_state[0xD] = false; break;
                    case SDL_SCANCODE_A: key_state[0x7] = false; break;
                    case SDL_SCANCODE_S: key_state[0x8] = false; break;
                    case SDL_SCANCODE_D: key_state[0x9] = false; break;
                    case SDL_SCANCODE_F: key_state[0xe] = false; break;
                    case SDL_SCANCODE_Z: key_state[0xA] = false; break;
                    case SDL_SCANCODE_X: key_state[0x0] = false; break;
                    case SDL_SCANCODE_C: key_state[0xB] = false; break;
                    case SDL_SCANCODE_V: key_state[0xF] = false; break;
                    default: break;
                }
                break;
        }
    }
    return 0; // no need to quit
}

bool Keypad::any_pressed() const {
    for(auto key_pressed : key_state)
        if(key_pressed) return true;
    return false;
}

uint8_t Keypad::get_key() const {
    for(int key = 0; key <= 0xF; ++key) {
        if(key_state[key])
            return key;
    }
    return 0x10; // should check if there's any key pressed beforehand
}
