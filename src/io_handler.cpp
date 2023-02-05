/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <iostream>
#include <SDL2/SDL.h>
#include "io_handler.hpp"

using namespace chip8;

Io_handler::Io_handler() {
    int status = SDL_Init(SDL_INIT_EVERYTHING);
    if(status < 0) {
        std::cerr << "Could not start SDL" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        throw std::exception();
    }
}

uint8_t Io_handler::get_key() {
    key_flag = false;
    return key_value;
}

Io_event Io_handler::poll_event() {
    SDL_Event event;
    if(!SDL_PollEvent(&event)) {
        return Io_event::NONE;
    }
    switch(event.type) {
        case SDL_QUIT: return Io_event::QUIT;
        case SDL_KEYDOWN:
            key_flag = true;
            switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_1: key_value = 0x1; break;
                    case SDL_SCANCODE_2: key_value = 0x2; break;
                    case SDL_SCANCODE_3: key_value = 0x3; break;
                    case SDL_SCANCODE_4: key_value = 0xC; break;
                    case SDL_SCANCODE_Q: key_value = 0x4; break;
                    case SDL_SCANCODE_W: key_value = 0x5; break;
                    case SDL_SCANCODE_E: key_value = 0x6; break;
                    case SDL_SCANCODE_R: key_value = 0xD; break;
                    case SDL_SCANCODE_A: key_value = 0x7; break;
                    case SDL_SCANCODE_S: key_value = 0x8; break;
                    case SDL_SCANCODE_D: key_value = 0x9; break;
                    case SDL_SCANCODE_F: key_value = 0xe; break;
                    case SDL_SCANCODE_Z: key_value = 0xA; break;
                    case SDL_SCANCODE_X: key_value = 0x0; break;
                    case SDL_SCANCODE_C: key_value = 0xB; break;
                    case SDL_SCANCODE_V: key_value = 0xF; break;
                    default:
                        key_flag = false;
            }
            return Io_event::KEYPRESS;
        case SDL_KEYUP:
            key_flag = false;
            break;
    }
    return Io_event::NONE;
}

Io_handler::~Io_handler() {
    SDL_Quit();
}
