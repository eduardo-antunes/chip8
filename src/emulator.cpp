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
#include <SDL2/SDL.h>
#include "screen.hpp"
#include "emulator.hpp"

using namespace chip8;

Emulator::Emulator() {
    // Built-in fontset; it only includes the 16 hexadecimal characters
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
    // Load built-in font into RAM, starting from font_addr
    for(uint16_t i = 0; i < 80; ++i)
        ram[i + font_addr] = font[i];
}

void Emulator::load_prog(const std::vector<uint8_t> &prog) {
    // Load code into RAM, starting from prog_addr
    uint16_t addr = prog_addr;
    for(auto byte : prog)
        ram[addr++] = byte;
}

#define INSTS_PER_SECOND 700

#define GET_TICKS() SDL_GetTicks()

#define SLEEP(t) SDL_Delay(t)

int Emulator::run() {
    int status = 0;
    bool quit = false;
    srand(time(NULL)); // seed RNG
    screen.request_update(); // to show the screen from the begginning

    while(!quit) {
        uint64_t start = GET_TICKS();
        // Process user input
        quit = keys.handle();
        if(quit) break;
        // Run instructions
        for(int i = 0; i < INSTS_PER_SECOND / 60; ++i) {
            status = single_step();
            if(status != 0) {
                quit = true;
                break;
            }
        }
        // Cap execution speed to 60 FPS
        uint64_t end = GET_TICKS();
        double elapsed = end - start;
        SLEEP(elapsed < 16.667 ? 16.667 - elapsed : 0);
        // Update the CPU timers
        update_timers();
        // Refresh graphics
        screen.update();
    }
    return status;
}

#undef INSTS_PER_SECOND

#undef GET_TICKS

#undef SLEEP

void Emulator::update_timers() {
    // The delay timer is constantly decremented while it's above zero
    if(delay > 0) --delay;
    // That's also the case for the sound timer, but, as long as it is above 
    // zero, a beeping sound must be played
    if(sound > 0) {
        --sound;
        audio.play();
    } else {
        audio.pause();
    }
}

// Join two bytes into a single 16-bit value
#define JOIN_BYTES(b1, b2) (((b1) << 8) | b2)

// Get the nth bit of a byte
#define GET_BIT(byte, n) (((byte) & (1 << (7 - (n)))) ? 1 : 0)

// Get the nth half (nibble) of a byte
#define GET_NIB(byte, n) ((n) ? ((byte) & 0x0F) : (((byte) & 0xF0) >> 4))

int Emulator::single_step() {
    // Fetch the 16 bits of an instruction and advance the PC
    uint8_t first = ram[pc], second = ram[pc + 1];
    uint16_t inst = JOIN_BYTES(first, second);
    pc += 2;
    // Break the instruction into its meaningful parts
    uint8_t op   = GET_NIB(first, 0);
    uint8_t x    = GET_NIB(first, 1);
    uint8_t y    = GET_NIB(second, 0);
    uint8_t n    = GET_NIB(second, 1);
    uint8_t nn   = second;
    uint16_t nnn = inst & 0x0FFF;
    // Decode and execute
    switch(op) {
        case 0x0:
            // Multiple instructions possible
            switch(inst) {
                case 0x00E0:
                    // 0x00E0: clear the display
                    screen.clear();
                    break;
                case 0x00EE:
                    // 0x00EE: return from subroutine
                    pc = stack[--sp];
                    break;
                default:
                    fprintf(stderr, "Unrecognized instruction: %4X\n", inst);
            }
            break;
        case 0x1:
            // 0x1NNN: unconditional jump to NNN
            pc = nnn;
            break;
        case 0x2:
            // 0x2NNN: jump to subroutine
            stack[sp++] = pc;
            pc = nnn;
            break;
        case 0x3:
            // 0x3XNN: skip next instruction if VX == NN
            if(v[x] == nn) pc += 2;
            break;
        case 0x4:
            // 0x4NNN: skip next instruction if VX != NN
            if(v[x] != nn) pc += 2;
            break;
        case 0x5:
            // 0x5XY0: skip next instruction if VX == VY
            if(n == 0 && v[x] == v[y]) pc += 2;
            break;
        case 0x6:
            // 0x6XNN: set register VX to NN
            v[x] = nn;
            break;
        case 0x7:
            // 0x7XNN: add NN to register VX, never set flag
            v[x] += nn;
            break;
        case 0x8:
            // Multiple instructions possible
            switch(n) {
                case 0x0:
                    // 0x8XY0: set VX to VY
                    v[x] = v[y];
                    break;
                case 0x1:
                    // 0x8XY1: set VX to VX | VY
                    v[x] |= v[y];
                    break;
                case 0x2:
                    // 0x8XY2: set VX to VX & VY
                    v[x] &= v[y];
                    break;
                case 0x3:
                    // 0x8XY3: set Vx to VX ^ VY
                    v[x] ^= v[y];
                    break;
                case 0x4:
                    // 0x8XY4: add VY to VX, set flag on overflow
                    set_flag(v[x] + v[y] > 255);
                    v[x] += v[y];
                    break;
                case 0x5:
                    // 0x8XY5: subtract VY from VX, clear flag on borrow
                    set_flag(v[x] >= v[y]);
                    v[x] -= v[y];
                    break;
                case 0x6:
                    // 0x8XY6: bitwise shift VX to the right, set flag to bit
                    // that was shifted out. NOTE: this instruction behaves
                    // differently in different variants of chip-8; here we
                    // adopt the chip-48 behavior instead of the original
                    set_flag(v[x] & 0x01);
                    v[x] >>= 1;
                    break;
                case 0x7:
                    // 0x8XY7: subtract VX from VY, clear flag on borrow
                    set_flag(v[y] >= v[x]);
                    v[y] -= v[x];
                    break;
                case 0xE:
                    // 0x8XYE: bitwise shift VX to the left, set flag to bit
                    // that was shifted out. NOTE: this instruction behaves
                    // differently in different variants of chip-8; here we
                    // adopt the chip-48 behavior instead of the original
                    set_flag(v[x] & 0x80);
                    v[x] <<= 1;
                    break;
                default:
                    fprintf(stderr, "Unrecognized instruction: %4X\n", inst);
            }
            break;
        case 0x9:
            // 0x9XY0: skip next instruction if VX != VY
            if(n == 0 && v[x] != v[y]) pc += 2;
            break;
        case 0xA:
            // 0xANNN: set index register to NNN
            index = nnn;
            break;
        case 0xB:
            // 0xBNNN: jump with offset. NOTE: this instruction is different in
            // different versions of chip-8; here, we adopt the original chip-8
            // behavior, which jumps to NNN + V0
            pc = nnn + v[0];
            break;
        case 0xC:
            // 0xCXNN: generate random number, bitwise and it with NN and store
            // the result in VX
            v[x] = rand() & nn;
            break;
        case 0xD: {
            // 0xDXYN: draw N pixels tall sprite, found at the memory address
            // specified by the index register, to the position (VX, VY) of
            // the display, setting a flag if any pixel is erased during this
            set_flag(false);
            uint8_t x_pos = v[x] & (screen.width - 1);
            uint8_t y_pos = v[y] & (screen.height - 1);
            const uint8_t original_x = x_pos;

            // Sprites are stored in memory as a series of bytes, where each
            // byte represents the operations that must be done to draw a line
            // of the sprite to the display. Each operation is represented by
            // a single bit. Each bit is associated with a particular pixel in
            // the line; if the bit is set, the pixel's value must be flipped
            // (i.e. if it's on it must be erased, if it is off it must be
            // filled). Otherwise, the pixel is kept as is.
            for(int i = 0; i < n; ++i) {
                uint8_t line = ram[index + i];
                for(int j = 0; j < 8; ++j) {
                    int pixel = GET_BIT(line, j);
                    if(pixel) {
                        if(screen.is_on(x_pos, y_pos)) {
                            screen.erase(x_pos, y_pos);
                            set_flag(true);
                        } else {
                            screen.fill(x_pos, y_pos);
                        }
                    }
                    ++x_pos;
                    // don't wrap around the edge of the screen
                    if(x_pos >= screen.width) break;
                }
                ++y_pos;
                // don't wrap around the edge of the screen
                if(y_pos >= screen.height) break;
                x_pos = original_x; // reset x position
            }
            screen.request_update(); // refresh graphics
            break;
        }
        case 0xE:
            switch(nn) {
                case 0x9E:
                    // 0xEX9E: skip next instruction if the key specified by
                    // VX is being pressed
                    if(keys.is_pressed(v[x]))
                        pc += 2;
                    break;
                case 0xA1:
                    // 0xEXA1: skip next instruction if the key specified by
                    // VX is not being pressed
                    if(!keys.is_pressed(v[x]))
                        pc += 2;
                    break;
            }
            break;
        case 0xF:
            switch(nn) {
                case 0x07:
                    // 0xFX07: set VX to the value of the delay timer
                    v[x] = delay;
                    break;
                case 0x0A:
                    // 0xFX0A: when a key is pressed, put its value in VX
                    if(keys.any_pressed()) v[x] = keys.get_key();
                    else pc -= 2;
                    break;
                case 0x15:
                    // 0xFX15: set the delay timer to the value of VX
                    delay = v[x];
                    break;
                case 0x18:
                    // 0xFX18: set the sound timer to the value of VX
                    sound = v[x];
                    break;
                case 0x1E:
                    // 0xFX1E: add VX to the index register
                    index += v[x];
                    set_flag(index >= 0x1000);
                    break;
                case 0x29:
                    // 0xFX29: set the index register to the address of the
                    // hexadecimal character specified by VX in the builtin
                    // font. Following the behavior of the original chip-8, we
                    // consider just the last nibble of VX
                    index += font_addr + GET_NIB(v[x], 1);
                    break;
                case 0x33: {
                    // 0xFX33: convert the value of VX into BCD and store the
                    // result into the address specified by the index register
                    uint8_t bcd = v[x];
                    ram[index + 2] = bcd % 10; // one's place
                    bcd /= 10;
                    ram[index + 1] = bcd % 10; // ten's place
                    bcd /= 10;
                    ram[index] = bcd; // hundred's place
                    break;
                }
                case 0x55:
                    // 0xFX55: store the values of the registers from V0 to
                    // VX, inclusive, into memory starting from the address
                    // specified by the index register. NOTE: this instruction
                    // behaves differently in different versions of chip-8.
                    // Here we adopt the chip-48 behavior, which does not
                    // change the value of the index register in its operation
                    for(uint8_t i = 0; i <= x; ++i)
                        ram[index + i] = v[i];
                    break;
                case 0x65:
                    // 0xFX65: it does the opposite of 0xFX55, and it's subject
                    // to the same differences of behavior in different 
                    // versions of chip-8. Here we again adopt the chip-48
                    // behavior, for consistency
                    for(uint8_t i = 0; i <= x; ++i)
                        v[i] = ram[index + i];
                    break;
            }
            break;
        default:
            fprintf(stderr, "Unrecognized instruction: %4X\n", inst);
            return 1;
    }
    return 0;
}

#undef GET_BIT

#undef GET_NIB

#undef JOIN_BYTES
