/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#ifndef CHIP8_INTERFACE_HPP
#define CHIP8_INTERFACE_HPP

#include <cstdint>
#include <cstring>
#include <SDL.h>

namespace chip8 {

    class Display {
        public:
            static const int width = 64, height = 32;

            Display(void);

            int is_on(int x, int y) const { return pixels[64 * y + x] == on ? 1 : 0; }

            void fill(int x, int y) { pixels[64 * y + x] = on; }

            void erase(int x, int y) { pixels[64 * y + x] = off; }

            void clear(void) { SDL_RenderClear(renderer); }

            void draw(void) const;

            ~Display(void);

        private:
            static const uint32_t on = 0xFFFFFFFF, off = 0xFF000000;
            uint32_t pixels[width * height] = {0};

            SDL_Window   *window   = NULL;
            SDL_Renderer *renderer = NULL;
            SDL_Texture  *texture  = NULL;
    };

}

#endif // CHIP8_INTERFACE_HPP
