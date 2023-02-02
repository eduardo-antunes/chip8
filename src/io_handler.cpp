#include <SDL.h>
#include <exception>
#include <iostream>

#include "io_handler.hpp"

using namespace chip8;

IO_handler::IO_handler() {
    int status = SDL_Init(SDL_INIT_EVERYTHING);
    if(status < 0) {
        std::cerr << "Could not initialize SDL" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        throw std::exception();
    }
}

void IO_handler::update_display() {
    if(draw_flag) {
        draw_flag = false;
        display.draw();
    }
}

IO_handler::~IO_handler() {
    SDL_Quit();
}
