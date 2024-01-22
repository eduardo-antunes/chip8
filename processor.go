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

const font_start = 0x0050
const prog_start = 0x0200

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
	msb := proc.ram[proc.pc]
	lsb := proc.ram[proc.pc+1]
	proc.pc += 2
	// Decode and execute the instruction
	inst := Decode(msb, lsb)
	switch inst.op {
	case JUMP:
		// Jumps to the given address
		proc.pc = inst.addr
	case LOAD:
		// Loads the given value into VX
		proc.v[inst.x] = inst.arg
	case ADD:
		// Adds the given value to VX (no flags set)
		proc.v[inst.x] += inst.arg
	case LOADI:
		// Loads the given address into the index register
		proc.index = inst.addr
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
				if x >= W {
					break // don't wrap around the edges of the screen
				}
			}
			y++
			if y >= H {
				break // don't wrap around the edges of the screen
			}
			x = prevx // reset the x position
		}
		proc.con.screen.RequestRefresh() // done with the graphics
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

// Helper function to get the nth bit of a byte as a boolean
func getBit(b byte, n int) bool {
	return (b & (1 << (7 - n))) != 0
}
