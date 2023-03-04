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

#include <SDL.h>
#include <cstdint>
#include <cstring>

namespace chip8 {
    class Screen {
        public:
            // Conceptual dimensions of the screen
            static const int width = 64, height = 32;

            // Create SDL objects
            Screen();

            // Clear the screen
            void clear() { for(auto &v : p) v = false; refresh_flag = true; }

            // Inspect the conceptual model
            bool is_on(int x, int y) { return p[width * y + x]; }

            // Manipulate the conceptual model
            void set_p(int x, int y, bool b) { p[width * y + x] = b; }

            // Mark changes as complete, triggering graphics to be refreshed
            void request_refresh() { refresh_flag = true; }

            // Refresh if requested
            void refresh();

            // Destroy SDL objects
            ~Screen();

        private:
            // Colors used in the graphics, in RGBA format
            static const uint32_t fg_color = 0xFFFFFFFF;
            static const uint32_t bg_color = 0x000000FF;

            // Color used in the transition from the background color to the
            // foreground color and vice-versa, to reduce screen flickering
            static const uint32_t tr_color = 0x7F7F7FFF;

            // t-factor used for linear interpolation
            constexpr static const double lerp_factor = 0.5;

            // Conceptual model of the screen
            bool p[width * height] = {0};

            // Real pixels that get displayed
            uint32_t pixels[width * height] = {0};

            // Condition for a refresh to happen
            bool refresh_flag = false;

            // The scale factor for the pixels
            static const int scale = 10;

            // SDL objects to produce the graphics
            SDL_Window *window = nullptr;
            SDL_Renderer *renderer = nullptr;
            SDL_Texture *texture  = nullptr;
    };
}

#endif // CHIP8_SCREEN_HPP
