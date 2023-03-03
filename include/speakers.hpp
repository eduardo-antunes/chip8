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

#define SAMPLE_RATE 44100

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
            constexpr static const double freq = 441.0;
            // Amplitude of the beeping sound
            constexpr static const double amp = 28000;

            // SDL objects to produce audio
            SDL_AudioSpec spec;
            SDL_AudioDeviceID dev;

            bool sound_playing = false;
            int sample_index = 0;

            // Audio sampling callback
            static void audio_callback(void *user_data, uint8_t *raw_buf, int bytes);
    };
}

#endif // CHIP8_SPEAKERS_HPP
