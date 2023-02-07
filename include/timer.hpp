/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#ifndef CHIP8_TIMER_HPP
#define CHIP8_TIMER_HPP

#include "SDL_timer.h"
#include <cstdint>
#include <SDL2/SDL.h>

namespace chip8 {
    class Timer {
        public:
            Timer() : start(SDL_GetPerformanceCounter()) {}
            double get_elapsed();
        private:
            uint64_t start, end;
    };
}

#endif // CHIP8_TIMER_HPP
