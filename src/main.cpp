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
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include "emulator.hpp"

std::vector<uint8_t> read_file(const char *path) {
    std::ifstream file(path, std::ios::binary);
    if(!file) {
        std::cerr << "Could not open " << path << std::endl;
        exit(74);
    }
    file.seekg(0, std::ios::end);
    auto file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(file_size);
    file.read((char*) &data[0], file_size);
    return data;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <rom-file>" << std::endl;
        return 1;
    }
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Could not initialize SDL" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return 2;
    }
    auto program = read_file(argv[1]);
    chip8::Emulator chip8_emu(program);
    chip8_emu.run();
    SDL_Quit();
    return 0;
}
