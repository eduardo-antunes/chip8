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

#ifndef CHIP8_SPEAKERS_HPP
#define CHIP8_SPEAKERS_HPP

#include <SDL.h>

namespace chip8 {
    class Speakers {
        public:
            Speakers();
            void play();
            void stop();
            ~Speakers();

        private:
            // Frequency of the beeping sound
            static const uint32_t frequency = 441;
            // Volume of the beeping sound
            static const int16_t tone_vol = 2800;
            // Sample rate of the beeping sound
            static const uint32_t sample_rate = 44100;

            // SDL objects to produce audio
            SDL_AudioSpec spec;
            SDL_AudioDeviceID dev;

            // Simple flag to control audio playing
            bool sound_playing = false;

            // Audio sampling callback
            static void generate(void *user, uint8_t *buf, int bytes);
    };
}

#endif // CHIP8_SPEAKERS_HPP
