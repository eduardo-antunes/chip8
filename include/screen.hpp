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

#ifndef CHIP8_SCREEN_HPP
#define CHIP8_SCREEN_HPP

#include <SDL2/SDL.h>
#include <cstdint>
#include <cstring>

namespace chip8 {
    class Screen {
        public:
            static const int width = 64, height = 32;

            Screen();

            bool is_on(int x, int y) { return pixels[width * y + x] == on; }

            void fill(int x, int y) { pixels[width * y + x] = on; }

            void erase(int x, int y) { pixels[width * y + x] = off; }

            void clear();

            void request_update() { draw_flag = true; }

            void update();

            ~Screen();
        private:
            static const uint32_t on = 0xFFFFFFFF, off = 0xFF000000;
            uint32_t pixels[width * height] = {0};
            bool draw_flag = false;

            // SDL stuff
            SDL_Window   *window   = NULL;
            SDL_Renderer *renderer = NULL;
            SDL_Texture  *texture  = NULL;
    };
}

#endif // CHIP8_SCREEN_HPP
