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

static uint32_t lerp_colors(uint32_t start_color, uint32_t end_color, double t) {
    // Break start color into components
    uint8_t r1 = (start_color >> 24) & 0xFF,
            g1 = (start_color >> 16) & 0xFF,
            b1 = (start_color >> 8)  & 0xFF,
            a1 = (start_color >> 0)  & 0xFF;
    // Break end color into components
    uint8_t r2 = (end_color >> 24) & 0xFF,
            g2 = (end_color >> 16) & 0xFF,
            b2 = (end_color >> 8)  & 0xFF,
            a2 = (end_color >> 0)  & 0xFF;
    // Linear interpolation of the individual components
    uint8_t r3 = (1 - t) * r1 + t * r2,
            g3 = (1 - t) * g1 + t * g2,
            b3 = (1 - t) * b1 + t * b2,
            a3 = (1 - t) * a1 + t * a2;
    // Join the results together into the final color
    uint32_t partial_color = (r3 << 24) | (g3 << 16) | (b3 << 8) | a3;
    return partial_color;
}

void Screen::refresh() {
    // Only do something if requested
    if(!refresh_flag) return;
    refresh_flag = false;

    // Map the conceptual model to the pixels
    for(uint32_t i = 0; i < width * height; ++i) {
        if(p[i] && pixels[i] != fg_color)
            // Progress towards foreground color
            pixels[i] = lerp_colors(pixels[i], fg_color, lerp_factor);
        else if(!p[i] && pixels[i] != bg_color)
            // Progress towards background color
            pixels[i] = lerp_colors(pixels[i], bg_color, lerp_factor);
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
