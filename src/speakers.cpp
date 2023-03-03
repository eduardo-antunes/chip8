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
#include <iostream>
#include <cmath>

#include "speakers.hpp"

using namespace chip8;

void Speakers::audio_callback(void *user_data, uint8_t *raw_buf, int bytes) {
    int length = bytes / 2; // two bytes per sample
    int16_t *buf = reinterpret_cast<int16_t*>(raw_buf);
    int &sample_index = *reinterpret_cast<int*>(user_data);

    for(int i = 0; i < length; ++i, ++sample_index) {
        double time = sample_index / (double) SAMPLE_RATE;
        buf[i] = (int16_t) (amp * std::sin(2.0 * M_PI * freq * time));
    }
}

Speakers::Speakers() {
    SDL_AudioSpec want = {
        .freq = SAMPLE_RATE,
        .format = AUDIO_S16SYS,
        .samples = 2048,
        .callback = audio_callback,
        .userdata = &sample_index,
    };
    dev = SDL_OpenAudioDevice(nullptr, 0, &want, &spec, 0);
    if(dev <= 0)
        throw std::runtime_error(std::string("Could not get audio device: ")
                + SDL_GetError());
}

void Speakers::play() {
    if(!sound_playing) {
        SDL_PauseAudioDevice(dev, 0);
        sound_playing = true;
    }
}

void Speakers::stop() {
    if(sound_playing) {
        SDL_PauseAudioDevice(dev, 1);
        sound_playing = false;
    }
}

Speakers::~Speakers() {
    SDL_CloseAudioDevice(dev);
}
