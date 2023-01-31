/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#ifndef CHIP8_DISPLAY_HPP
#define CHIP8_DISPLAY_HPP

#include <cstdint>
#include <cstring>

namespace chip8 {

    class Display {
        public:
            int width(void) const { return 64; }

            int height(void) const { return 32; }

            int get(int x, int y) const { return pixels[64 * y + x]; }

            void fill(int x, int y) { pixels[64 * y + x] = 1; }

            void erase(int x, int y) { pixels[64 * y + x] = 0; }

            void clear(void) { std::memset(pixels, 0, 32 * 64); }

            void draw(void) const;

        private:
            uint8_t pixels[32 * 64] = {0};
    };

}

#endif // CHIP8_DISPLAY_HPP
