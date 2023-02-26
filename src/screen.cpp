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

#include <SDL.h>
#include <stdexcept>

#include "screen.hpp"

using namespace chip8;

Screen::Screen() {
    // Create window
    window = SDL_CreateWindow("Chip-8 emulator", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            width * scale, height * scale, SDL_WINDOW_SHOWN);
    if(window == nullptr)
        throw std::runtime_error(std::string("Could not create window: ")
                    + SDL_GetError());

    // Create renderer for the window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == nullptr) {
        SDL_DestroyWindow(window);
        throw std::runtime_error(std::string("Could not create renderer: ")
                    + SDL_GetError());
    }
    SDL_RenderSetLogicalSize(renderer, width, height);

    // Create a texture to render things to
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
            SDL_TEXTUREACCESS_STREAMING, width, height);
    if(texture == nullptr) {
        SDL_DestroyWindow(window);
        throw std::runtime_error(std::string("Could not create texture: ")
                    + SDL_GetError());
    }
}

void Screen::refresh() {
    // Only do something if requested
    if(!refresh_flag) return;
    refresh_flag = false;

    // Map the conceptual model to the pixels
    for(uint32_t i = 0; i < width * height; ++i) {
        pixels[i] = p[i] ? fg_color : bg_color;
    }

    // Lock texture and pass in the pixels
    int pitch; uint32_t *buf;
    SDL_LockTexture(texture, nullptr, (void**) &buf, &pitch);
    memcpy(buf, pixels, pitch * height);

    // Unlock texture and present changes
    SDL_UnlockTexture(texture);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

Screen::~Screen() {
    SDL_DestroyWindow(window);
}
