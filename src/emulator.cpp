/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

#include "emulator.hpp"

using namespace chip8;

Emulator::Emulator(void) {
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
        mem[i + 0x50] = font[i];
}

void Emulator::load_code(const std::vector<uint8_t> &rom) {
    // load code into memory, starting from address 0x200
    uint16_t addr = 0x200;
    for(auto byte : rom) {
        mem[addr] = byte;
        ++addr;
    }
}

int Emulator::run(void) {
    display.draw();
    bool running = true;
    while(running) {
        int status = step();
        if(status != 0) return status;
        if(draw_flag) {
            draw_flag = false;
            display.draw();
        }
    }
    return 0;
}

int Emulator::run_debug(void) {
    display.draw();
    int status = 0;
    char debug_inst;
    bool running = true;
    while(running) {
        std::cin >> debug_inst;
        switch(debug_inst) {
            case 'i':
                std::cout << "Next instruction: ";
                show_current_instruction();
                break;
            case 'm':
                show_mem();
                break;
            case 'n':
                std::cout << "Just executed: ";
                show_current_instruction();
                status = step();
                if(status != 0) 
                    return status;
                if(draw_flag) {
                    draw_flag = false;
                    display.draw();
                }
                break;
            case 'r':
                show_regs();
                break;
            case 'q':
                std::cout << "Exiting..." << std::endl;
                return 0;
            default:
                std::cout << "Please input a debug instruction" << std::endl;
        }
    }
    return 0;
}

// Join two bytes into a single 16-bit value
#define JOIN_BYTES(b1, b2) (((b1) << 8) | b2)

// Get the nth bit of a byte
#define GET_BIT(byte, n) (((byte) & (1 << (7 - (n)))) ? 1 : 0)

// Get the nth half (nibble) of a byte
#define GET_NIB(byte, n) ((n) ? ((byte) & 0x0F) : (((byte) & 0xF0) >> 4))

int Emulator::step(void) {
    // Fetch an instruction, break it and update the PC
    uint16_t inst = JOIN_BYTES(mem[pc], mem[pc + 1]);
    uint8_t op    = GET_NIB(mem[pc], 0);
    uint8_t x     = GET_NIB(mem[pc], 1);
    uint8_t y     = GET_NIB(mem[pc + 1], 0);
    uint8_t n     = GET_NIB(mem[pc + 1], 1);
    uint8_t nn    = mem[pc + 1];
    uint16_t nnn  = inst & 0x0FFF;
    pc += 2;
    // Decode and execute the instruction
    switch(op) {
        case 0:
            // 0x00E0: clear the display
            if(inst == 0x00E0)
                display.clear();
            break;
        case 1:
            // 0x1NNN: unconditional jump to NNN
            pc = nnn;
            break;
        case 6:
            // 0x6XNN: set register V[X] to NN
            v[x] = nn;
            break;
        case 7:
            // 0x7XNN: add NN to register V[X], without setting any flags
            v[x] += nn;
            break;
        case 10:
            // 0xANNN: set index register to NNN
            index_reg = nnn;
            break;
        case 13: {
            // 0xDXYN: draw N pixels tall sprite, found at the memory address
            // specified by the index register, to the position (V[X], V[Y]) of
            // the display, setting a flag if any pixel is erased during this
            clear_flag();
            uint8_t x_pos = v[x] & (display.width - 1);  // % display.width()
            uint8_t y_pos = v[y] & (display.height - 1); // % display.height()

            // Sprites are stored in memory as a series of bytes, where each
            // byte represents the operations that must be done to draw a line
            // of the sprite to the display. Each operation is represented by
            // a single bit. Each bit is associated with a particular pixel in
            // the line; if the bit is set, the pixel's value must be flipped
            // (i.e. if it's on it must be erased, if it is off it must be
            // filled). Otherwise, the pixel is kept as is.
            for(int i = 0; i < n; ++i) {
                uint8_t line = mem[index_reg + i];
                for(int j = 0; j < 8; ++j) {
                    int pixel = GET_BIT(line, j);
                    if(pixel) {
                        if(display.is_on(x_pos, y_pos)) {
                            display.erase(x_pos, y_pos);
                            set_flag();
                        } else {
                            display.fill(x_pos, y_pos);
                        }
                    }
                    ++x_pos;
                    if(x_pos >= display.width) break;
                }
                ++y_pos;
                if(y_pos >= display.height) break;
                x_pos = v[x] & (display.width - 1); // reset x position
            }
            draw_flag = true; // refresh graphics
            break;
        }
        default:
            fprintf(stderr, "Unrecognized instruction: %04X\n", inst);
            return 1;
    }
    return 0;
}

#undef GET_BIT
#undef GET_NIB

// Debugging functions

void Emulator::show_mem(void) const {
    // had to resort to printf a bit
    for(int i = 0; i < 256; ++i) {
        printf("%03X\t", i * 16);
        for(int j = 0; j < 16; ++j) {
            int n = static_cast<int>(mem[i*16 + j]);
            printf("%02X ", n);
        }
        std::cout << std::endl;
    }
}

void Emulator::show_regs(void) const {
    printf("PC = %04X\n", pc);
    for(int i = 0; i < 16; ++i)
        printf("V%X = %d\n", i, v[i]);
    std::cout << "I = " << index_reg << std::endl;
}

void Emulator::show_current_instruction(void) const {
    uint16_t opcode = JOIN_BYTES(mem[pc], mem[pc + 1]);
    printf("%04X\n", opcode);
}

#undef JOIN_BYTES
