#ifndef CHIP8_IO_HANDLER_HPP
#define CHIP8_IO_HANDLER_HPP

#include "display.hpp"

namespace chip8 {
    class IO_handler {
        public:
            Display display;

            IO_handler();
            void set_draw_flag() { draw_flag = true; }
            void update_display();
            ~IO_handler();

        private:
            bool draw_flag = false;
    };
}

#endif // CHIP8_IO_HANDLER_HPP
