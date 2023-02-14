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

#ifndef CHIP8_AUDIO_HPP
#define CHIP8_AUDIO_HPP

#include <SDL2/SDL.h>

namespace chip8 {
    class Audio {
        public:
            Audio();
            void play()  { SDL_PauseAudio(0); }
            void pause() { SDL_PauseAudio(1); }
            ~Audio();
        private:
            SDL_AudioSpec spec_;
            int sample_nr = 4400;
    };
}

#endif // CHIP8_AUDIO_HPP
