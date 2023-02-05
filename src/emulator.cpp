/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <ctime>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include "emulator.hpp"

using namespace chip8;

Emulator::Emulator() {
    pc = 0x200; // user code starts here
    uint8_t font[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    // load font into the address range 0x50 - 0x9F
    for(uint16_t i = 0; i < 80; ++i)
        memory[i + 0x50] = font[i];
}

void Emulator::load_code(const std::vector<uint8_t> &code) {
    // load code into memory, starting from the address 0x200
    uint16_t addr = 0x200;
    for(auto byte : code) {
        memory[addr] = byte;
        ++addr;
    }
}

int Emulator::run() {
    Io_event event;
    bool quit = false;
    io.screen.request_update(); // to show the screen from the begginning
    srand(time(NULL));
    while(!quit) {
        do {
            event = io.poll_event();
            if(event == Io_event::QUIT)
                quit = true;
        } while(event != Io_event::NONE);
        int err = single_step();
        if(err) quit = true;
        io.screen.update();
    }
    return 0;
}

// Join two bytes into a single 16-bit value
#define JOIN_BYTES(b1, b2) (((b1) << 8) | b2)

// Get the nth bit of a byte
#define GET_BIT(byte, n) (((byte) & (1 << (7 - (n)))) ? 1 : 0)

// Get the nth half (nibble) of a byte
#define GET_NIB(byte, n) ((n) ? ((byte) & 0x0F) : (((byte) & 0xF0) >> 4))

int Emulator::single_step() {
    // Fetch an instruction, break it and advance the PC
    uint16_t inst = JOIN_BYTES(memory[pc], memory[pc + 1]);
    uint8_t op    = GET_NIB(memory[pc], 0);
    uint8_t x     = GET_NIB(memory[pc], 1);
    uint8_t y     = GET_NIB(memory[pc + 1], 0);
    uint8_t n     = GET_NIB(memory[pc + 1], 1);
    uint8_t nn    = memory[pc + 1];
    uint16_t nnn  = inst & 0x0FFF;
    pc += 2;
    // Decode and execute instruction
    switch(op) {
        case 0:
            if(inst == 0x00E0) {
                // 0x00E0: clear the display
                io.screen.clear();
                io.screen.request_update();
            } else if(inst == 0x00EE) {
                // 0x00EE: return from subroutine
                pc = stack.back();
                stack.pop_back();
            }
            break;
        case 1:
            // 0x1NNN: unconditional jump to NNN
            pc = nnn;
            break;
        case 2:
            // 0x2NNN: jump to subroutine
            stack.push_back(pc);
            pc = nnn;
            break;
        case 3:
            // 0x3XNN: skip next instruction if V[X] == N
            if(v[x] == nn) pc += 2;
            break;
        case 4:
            // 0x4NNN: skip next instruction if V[X] != NN
            if(v[x] != nn) pc += 2;
            break;
        case 5:
            // 0x5XY0: skip next instruction if V[X] == V[Y]
            if(n == 0 && v[x] == v[y]) pc += 2;
            break;
        case 6:
            // 0x6XNN: set register V[X] to NN
            v[x] = nn;
            break;
        case 7:
            // 0x7XNN: add NN to register V[X], don't change flag
            v[x] += nn;
            break;
        case 8:
            // Arithmetic and logic instructions
            switch(n) {
                case 0:
                    // 0x8XY0: set V[X] to V[Y]
                    v[x] = v[y];
                    break;
                case 1:
                    // 0x8XY1: set V[X] to V[X] | V[Y]
                    v[x] |= v[y];
                    break;
                case 2:
                    // 0x8XY2: set V[X] to V[X] & V[Y]
                    v[x] &= v[y];
                    break;
                case 3:
                    // 0x8XY3: set V[x] to V[X] ^ V[Y]
                    v[x] ^= v[y];
                    break;
                case 4:
                    // 0x8XY4: add V[Y] to V[X], set flag on overflow
                    set_flag(v[x] == 255);
                    v[x] += v[y];
                    break;
                case 5:
                    // 0x8XY5: subtract V[Y] from V[X], clear flag on borrow
                    set_flag(v[x] >= v[y]);
                    v[x] -= v[y];
                    break;
                case 6:
                    // 0x8XY6: bitwise shift V[X] to the right, set flag to bit
                    // that was shifted out. NOTE: this instruction behaves
                    // differently in different variants of chip-8; here we
                    // adopt the chip-48 behavior instead of the original
                    set_flag(v[x] & 1);
                    v[x] >>= 1;
                    break;
                case 7:
                    // 0x8XY7: subtract V[X] from V[Y], clear flag on borrow
                    set_flag(v[y] >= v[x]);
                    v[y] -= v[x];
                    break;
                case 14:
                    // 0x8XYE: bitwise shift V[X] to the left, set flag to bit
                    // that was shifted out. NOTE: this instruction behaves
                    // differently in different variants of chip-8; here we
                    // adopt the chip-48 behavior instead of the original
                    set_flag(v[x] & 128);
                    v[x] >>= 1;
                    break;
                default:
                    std::cerr << "Unrecognized instruction" << std::endl;
            }
        case 9:
            // 0x9XY0: skip next instruction if V[X] != V[Y]
            if(n == 0 && v[x] != v[y]) pc += 2;
            break;
        case 10:
            // 0xANNN: set index register to NNN
            index_reg = nnn;
            break;
        case 11:
            // 0xBNNN: jump with offset. NOTE: this instruction is different in
            // different versions of chip-8; here, we adopt the original chip-8
            // instruction, which jumps to NNN + V[0]
            pc = nnn + v[0];
            break;
        case 12:
            // 0xCXNN: generate random number, bitwise and it with NN and store
            // the result in V[X]
            v[x] = rand() & nn;
            break;
        case 13: {
            // 0xDXYN: draw N pixels tall sprite, found at the memory address
            // specified by the index register, to the position (V[X], V[Y]) of
            // the display, setting a flag if any pixel is erased during this
            set_flag(0);
            uint8_t x_pos = v[x] & (io.screen.width - 1);
            uint8_t y_pos = v[y] & (io.screen.height - 1);

            // Sprites are stored in memory as a series of bytes, where each
            // byte represents the operations that must be done to draw a line
            // of the sprite to the display. Each operation is represented by
            // a single bit. Each bit is associated with a particular pixel in
            // the line; if the bit is set, the pixel's value must be flipped
            // (i.e. if it's on it must be erased, if it is off it must be
            // filled). Otherwise, the pixel is kept as is.
            for(int i = 0; i < n; ++i) {
                uint8_t line = memory[index_reg + i];
                for(int j = 0; j < 8; ++j) {
                    int pixel = GET_BIT(line, j);
                    if(pixel) {
                        if(io.screen.is_on(x_pos, y_pos)) {
                            io.screen.erase(x_pos, y_pos);
                            set_flag(1);
                        } else {
                            io.screen.fill(x_pos, y_pos);
                        }
                    }
                    ++x_pos;
                    // don't wrap around the edge of the screen
                    if(x_pos >= io.screen.width) break;
                }
                ++y_pos;
                // don't wrap around the edge of the screen
                if(y_pos >= io.screen.height) break;
                x_pos = v[x] & (io.screen.width - 1); // reset x position
            }
            io.screen.request_update(); // refresh graphics
            break;
        }
        case 14:
            switch(nn) {
                case 0x9E:
                    // 0xEX9E: skip next instruction if the key specified by
                    // V[X] is being pressed
                    if(io.key_pressed() && io.get_key() == v[x])
                        pc += 2;
                    break;
                case 0xA1:
                    // 0xEXA1: skip next instruction if the key specified by
                    // V[X] is not being pressed
                    if(!io.key_pressed() || io.get_key() != v[x])
                        pc += 2;
                    break;
            }
        case 15:
            switch(nn) {
                case 0x0A:
                    // 0xFX0A: when a key is pressed, put its value in V[X]
                    if(io.key_pressed()) v[x] = io.get_key();
                    else pc -= 2;
                    break;
            }
        default:
            std::cerr << "Unrecognized instruction: " << inst << std::endl;
            return 1;
    }
    return 0;
}

#undef GET_BIT
#undef GET_NIB
#undef JOIN_BYTES