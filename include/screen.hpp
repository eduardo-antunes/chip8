/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
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

            void clear() { memset(pixels, 0, width * height * sizeof(uint32_t)); }

            void request_update() { draw_flag = true; }

            void update();

            ~Screen();
        private:
            static const uint32_t on = 0xFFFFFFFF, off = 0xFF000000;
            uint32_t pixels[width * height] = {0};
            bool draw_flag = false;

            SDL_Window   *win = NULL;
            SDL_Renderer *ren = NULL;
            SDL_Texture  *tex = NULL;
    };
}

#endif // CHIP8_SCREEN_HPP
