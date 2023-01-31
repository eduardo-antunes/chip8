/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <cstdint>
#include <iostream>

#include "display.hpp"

using namespace chip8;

// I have yet to make this graphical
void Display::draw(void) const {
    for(int y = 0; y < height(); ++y) {
        for(int x = 0; x < width(); ++x) {
            char rep = get(x, y) ? '@' : '_';
            std::cout << rep;
        }
        std::cout << std::endl;
    }
}
