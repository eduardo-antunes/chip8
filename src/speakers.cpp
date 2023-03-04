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
#include <stdexcept>

#include "speakers.hpp"

using namespace chip8;

void Speakers::generate(void *user, uint8_t *buf, int bytes) {
    static uint32_t sample_nr = 0;
    int16_t *audio_data = (int16_t*) buf;
    int length = bytes / 2; // each sample has two bytes
    // Generate a standard square wave
    uint32_t half_period = (sample_rate / frequency) / 2;
    for(int i = 0; i < length; ++i, ++sample_nr) {
        audio_data[i] = ((sample_nr / half_period) % 2) ? tone_vol : -tone_vol;
    }
}

Speakers::Speakers() {
    SDL_AudioSpec want = {
        .freq = sample_rate,
        .format = AUDIO_S16SYS,
        .samples = 512,
        .callback = generate,
        .userdata = nullptr,
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
