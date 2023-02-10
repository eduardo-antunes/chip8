/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
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
