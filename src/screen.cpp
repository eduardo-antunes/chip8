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
    win = SDL_CreateWindow("Chip-8 emulator", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            width * 10, height * 10, SDL_WINDOW_SHOWN);
    if(win == NULL) {
        std::cerr << "Could not create window" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        throw std::exception();

    }
    // Create a renderer for the window
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if(ren == NULL) {
        std::cerr << "Could not create renderer" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        throw std::exception();
    }
    SDL_RenderSetLogicalSize(ren, width * 10, height * 10);
    // Create a texture
    tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, 
            SDL_TEXTUREACCESS_STREAMING, width, height);
    if(tex == NULL) {
        std::cerr << "Could not create texture" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        throw std::exception();
    }
}

void Screen::clear() {
    draw_flag = true;
    memset(pixels, 0, width * height * sizeof(uint32_t));
}

void Screen::update() {
    if(draw_flag) {
        draw_flag = false;
        int pitch = 0;
        uint32_t *locked_pixels = NULL;
        SDL_LockTexture(tex, NULL, reinterpret_cast<void**>(&locked_pixels), &pitch);
        memcpy(locked_pixels, pixels, width * height * sizeof(uint32_t));
        SDL_UnlockTexture(tex);

        // Present the updated texture though the renderer
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);

    }
}

Screen::~Screen() {
    SDL_DestroyWindow(win);
}
