/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
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
