/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <SDL2/SDL.h>
#include "timer.hpp"

using namespace chip8;

double Timer::get_elapsed() {
    end = SDL_GetPerformanceCounter();
    double elapsed = (end - start) / (double) SDL_GetPerformanceFrequency();
    start = end;
    return elapsed;
}
