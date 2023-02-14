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

#include <cstdint>
#include <iostream>
#include <SDL2/SDL.h>
#include "audio.hpp"

using namespace chip8;

void audio_callback(void *user_data, uint8_t *raw_buf, int bytes) {
    uint16_t *buf = reinterpret_cast<uint16_t*>(raw_buf);
    int &sample_nr = *reinterpret_cast<int*>(user_data);
    int len = bytes / 2;
    for(int i = 0; i < len; ++i, ++sample_nr) {
        double time = (double)sample_nr / 44100;
        buf[i] = (uint16_t)(28000 * sin(2.0 * M_PI * 441.0 * time));
    }
}

Audio::Audio() {
    SDL_AudioSpec want;
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 2048;
    want.callback = audio_callback;
    want.userdata = &sample_nr;

    if(SDL_OpenAudio(&want, &spec_) != 0) {
        std::cerr << "Could not open audio" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        throw std::exception();
    }
    if(want.format != spec_.format) {
        std::cerr << "Could not get desired audio spec" << std::endl;
        throw std::exception();
    }
}

Audio::~Audio() {
    SDL_CloseAudio();
}
