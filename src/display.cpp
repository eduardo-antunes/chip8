/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <cstdint>
#include <cstring>
#include <iostream>
#include <SDL.h>

#include "display.hpp"

using namespace chip8;

Display::Display(void) {
    // Initialize SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Could not initialize SDL. Error: " 
            << SDL_GetError() << std::endl;
        SDL_Quit();
        throw std::exception();
    }
    // Create a window
    window = SDL_CreateWindow("Chip-8 emulator", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            width * 10, height * 10, SDL_WINDOW_SHOWN);
    if(window == NULL) {
        std::cerr << "Could not create window. Error: " 
            << SDL_GetError() << std::endl;
        SDL_Quit();
        throw std::exception();

    }
    // Create a renderer for the window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) {
        std::cerr << "Could not create renderer. Error: " 
            << SDL_GetError() << std::endl;
        SDL_Quit();
        throw std::exception();
    }
    SDL_RenderSetLogicalSize(renderer, width * 10, height * 10);
    // Create a texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
            SDL_TEXTUREACCESS_STREAMING, width, height);
    if(texture == NULL) {
        std::cerr << "Could not create texture. Error: " 
            << SDL_GetError() << std::endl;
        SDL_Quit();
        throw std::exception();
    }
}

void Display::draw(void) const {
    int pitch = 0;
    uint32_t *locked_pixels = NULL;
    SDL_LockTexture(texture, NULL, reinterpret_cast<void**>(&locked_pixels), &pitch);
    memcpy(locked_pixels, pixels, width * height * sizeof(uint32_t));
    SDL_UnlockTexture(texture);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

Display::~Display(void) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
