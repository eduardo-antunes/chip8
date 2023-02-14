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
