/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <iostream>
#include <SDL2/SDL.h>
#include "screen.hpp"

using namespace chip8;

Screen::Screen() {
    // SDL should already be initialized
    // Create a window
    window = SDL_CreateWindow("Chip-8 emulator", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            width * 10, height * 10, SDL_WINDOW_SHOWN);
    if(window == NULL) {
        std::cerr << "Could not create window" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        throw std::exception();

    }
    // Create a renderer for the window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) {
        std::cerr << "Could not create renderer" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        throw std::exception();
    }
    SDL_RenderSetLogicalSize(renderer, width * 10, height * 10);
    // Create a texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
            SDL_TEXTUREACCESS_STREAMING, width, height);
    if(texture == NULL) {
        std::cerr << "Could not create texture" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        throw std::exception();
    }
}

void Screen::clear() {
    memset(pixels, 0, width * height * sizeof(uint32_t));
    request_update();
}

void Screen::update() {
    // TODO: fix screen flickering
    if(draw_flag) {
        draw_flag = false;
        int pitch = 0;
        uint32_t *locked_pixels = NULL;
        SDL_LockTexture(texture, NULL, reinterpret_cast<void**>(&locked_pixels), &pitch);
        memcpy(locked_pixels, pixels, width * height * sizeof(uint32_t));
        SDL_UnlockTexture(texture);
        // Present the updated texture though the renderer
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
}

Screen::~Screen() {
    SDL_DestroyWindow(window);
}
