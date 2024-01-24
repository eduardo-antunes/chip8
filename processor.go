/*
 * Copyright 2024 Eduardo Antunes dos Santos Vieira
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

package main

import (
	"log"
	"math/rand"

	"github.com/veandco/go-sdl2/sdl"
)

// Hardcoded important addresses
const (
	font_start = 0x0050
	prog_start = 0x0200
)

type Processor struct {
	con          *Console    // reference to the outside world
	ram          [4096]uint8 // 4KiB of memory for program data and code
	pc           uint16      // points at the next instruction
	index        uint16      // points at stuff in memory
	stack        [16]uint16  // stack for subroutine control
	stack_count  uint8       // counts how many things are on the stack
	delay, sound uint8       // delay and sound timers
	v            [16]uint8   // variable registers (general-purpose)
}

// Initializes a new processor with the built-in fontset loaded into memory
func NewProcessor(console *Console) *Processor {
	var fontset = [...]uint8{
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
		0xF0, 0x80, 0xF0, 0x80, 0x80, // F
	}
	proc := &Processor{
		con: console,
		pc:  prog_start,
	}
	// Load built-in font into memory
	var addr uint16 = font_start
	for _, font_byte := range fontset {
		proc.ram[addr] = font_byte
		addr++
	}
	return proc
}

// Loads program code into memory
func (proc *Processor) LoadProg(prog []uint8) {
	var addr uint16 = prog_start
	for _, prog_byte := range prog {
		proc.ram[addr] = prog_byte
		addr++
	}
}

// Single step through the code
func (proc *Processor) SingleStep() {
	// Fetch the 16 bits of an instruction (big endian) and advance the PC
	msb := proc.ram[proc.pc+0]
	lsb := proc.ram[proc.pc+1]
	proc.pc += 2
	// Decode and execute the instruction
	inst := Decode(msb, lsb)
	switch inst.op {
	case CALL:
		// Calls subroutine at address
		proc.stack[proc.stack_count] = proc.pc
		proc.stack_count++
		proc.pc = inst.addr
	case RTS:
		// Returns from subroutine
		proc.stack_count--
		proc.pc = proc.stack[proc.stack_count]
	case JUMP:
		// Jumps to the given address
		proc.pc = inst.addr
	case JUMP0:
		// Jumps to the given address + V0
		proc.pc = inst.addr + uint16(proc.v[0])

	case SKE:
		// Skips if VX equals argument
		if proc.v[inst.x] == inst.arg {
			proc.pc += 2
		}
	case SKNE:
		// Skips if VX doesn't equal argument
		if proc.v[inst.x] != inst.arg {
			proc.pc += 2
		}
	case SKRE:
		// Skips if VX equals VY
		if proc.v[inst.x] == proc.v[inst.y] {
			proc.pc += 2
		}
	case SKRNE:
		// Skips if VX doesn't equal VY
		if proc.v[inst.x] != proc.v[inst.y] {
			proc.pc += 2
		}
	case SKPR:
		// Skips if key corresponding to VX is pressed
		if proc.con.IsPressed(proc.v[inst.x] & 0xF) {
			proc.pc += 2
		}
	case SKNPR:
		// Skips if key corresponding to VX is not pressed
		if !proc.con.IsPressed(proc.v[inst.x] & 0xF) {
			proc.pc += 2
		}
	case KEYD:
		// Waits for a key press, then stores it in VX
		key, isPressed := proc.con.GetPressed()
		if !isPressed {
			proc.pc -= 2
		} else {
			proc.v[inst.x] = key
		}

	case LOAD:
		// Loads the given value into VX
		proc.v[inst.x] = inst.arg
	case LOADI:
		// Loads the given address into the index register
		proc.index = inst.addr
	case LOADD:
		// Loads the value of VX to delay timer
		proc.delay = proc.v[inst.x]
	case LOADS:
		// Loads the value of VX to sound timer
		proc.sound = proc.v[inst.x]
	case LDCHR:
		// Points index to the character sprite of VX
		chr := proc.v[inst.x] & 0xF
		proc.index = font_start + uint16(chr)*5
	case MOVE:
		// Copies VY to VX
		proc.v[inst.x] = proc.v[inst.y]
	case MOVED:
		// Copies delay timer to VX
		proc.v[inst.x] = proc.delay
	case OR:
		// VX |= VY
		proc.v[inst.x] |= proc.v[inst.y]
	case AND:
		// VX &= VY
		proc.v[inst.x] &= proc.v[inst.y]
	case XOR:
		// VX ^= VY
		proc.v[inst.x] ^= proc.v[inst.y]

	case ADD:
		// Adds the given value to VX (no flags set)
		proc.v[inst.x] += inst.arg
	case ADDR:
		// Adds VY to VX, setting flag on carry
		proc.setFlag(proc.v[inst.y] > 255-proc.v[inst.x])
		proc.v[inst.x] += proc.v[inst.y]
	case ADDI:
		// Adds VX to the index register
		proc.index += uint16(proc.v[inst.x])
	case SUB:
		// VX -= VY, setting flag on !borrow
		proc.setFlag(proc.v[inst.x] > proc.v[inst.y])
		proc.v[inst.x] -= proc.v[inst.y]
	case RSUB:
		// VX = VY - VX, setting flag on !borrow
		proc.setFlag(proc.v[inst.y] > proc.v[inst.x])
		proc.v[inst.x] = proc.v[inst.y] - proc.v[inst.x]
	case SHR:
		// Shifts VX to the right
		proc.setFlag(proc.v[inst.x]&0x01 > 0)
		proc.v[inst.x] >>= 1
	case SHL:
		// Shifts VX to the left
		proc.setFlag(proc.v[inst.x]&0x80 > 0)
		proc.v[inst.x] <<= 1
	case RAND:
		// Puts a random number, ands it with the argument, and sets VX to it
		proc.v[inst.x] = uint8(rand.Int() & int(inst.arg))

	case CLS:
		// Clears the screen
		proc.con.screen.Clear()
	case DRAW:
		// Probably the most complicated instruction, it draws a sprite from
		// the location pointed to by the index register
		proc.setFlag(false)
		x := uint32(proc.v[inst.x]) & (W - 1)
		y := uint32(proc.v[inst.y]) & (H - 1)
		prevx := x
		// Sprites are stored in memory as a series of bytes, where each
		// byte represents the operations that must be done to draw a row
		// of the sprite to the display. Each operation is represented by
		// a single bit. Each bit is associated with a particular pixel in
		// the row; if the bit is set, the pixel's value must be flipped
		// (i.e. if it's on it must be erased, if it is off it must be
		// filled). Otherwise, the pixel is kept as is.
		for i := uint16(0); i < uint16(inst.n); i++ {
			spriteRow := proc.ram[proc.index+i]
			for j := 0; j < 8; j++ {
				pixel := getBit(spriteRow, j)
				if pixel {
					if proc.con.screen.GetPixel(x, y) {
						proc.con.screen.SetPixel(x, y, false)
						proc.setFlag(true)
					} else {
						proc.con.screen.SetPixel(x, y, true)
					}
				}
				x++
				// don't wrap around the edges of the screen
				if x >= W {
					break
				}
			}
			y++
			// don't wrap around the edges of the screen
			if y >= H {
				break
			}
			x = prevx // reset the x position
		}
		proc.con.screen.RequestRefresh() // done with the graphics

	case BCD:
		// Converts VX to BCD and stores it in RAM, where the index points
		proc.ram[proc.index+2] = proc.v[inst.x] % 10        // 1s
		proc.ram[proc.index+1] = (proc.v[inst.x] / 10) % 10 // 10s
		proc.ram[proc.index+0] = proc.v[inst.x] / 100       // 100s
	case STORE:
		// Stores registers into memory
		for i := uint16(0); i <= uint16(inst.x); i++ {
			proc.ram[proc.index+i] = proc.v[i]
		}
	case READ:
		// Reads registers from memory
		for i := uint16(0); i <= uint16(inst.x); i++ {
			proc.v[i] = proc.ram[proc.index+i]
		}
	default:
		log.Panicf("Could not execute 0x%02X%02X\n", msb, lsb)
	}
}

// Set or the clear the flag register, VF
func (proc *Processor) setFlag(state bool) {
	if state {
		proc.v[0xF] = 1
	} else {
		proc.v[0xF] = 0
	}
}

// Update the hardware timers (should be called at 60Hz)
func (proc *Processor) UpdateTimers() {
	// The delay timer simply descreases while it's above zero
	if proc.delay > 0 {
		proc.delay--
	}
	// The sound timer does the same, but triggers a beeping sound to be made
	// while it has a positive value
	if proc.sound > 0 {
		proc.sound--
		sdl.PauseAudio(false)
	} else {
		sdl.PauseAudio(true)
	}
}

// Helper function to get the nth bit of a byte as a boolean
func getBit(b byte, n int) bool {
	return (b & (1 << (7 - n))) != 0
}
