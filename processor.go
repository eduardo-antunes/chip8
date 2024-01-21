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

import "fmt"

const font_start = 0x0050
const prog_start = 0x0200

type Processor struct {
	ram          [4096]uint8 // 4KiB of memory for program data and code
	pc           uint16      // points at the next instruction
	index        uint16      // points at stuff in memory
	stack        [16]uint16  // stack for subroutine control
	stack_count  uint8       // counts how many things are on the stack
	delay, sound uint8       // delay and sound timers
	v            [16]uint8   // variable registers (general-purpose)
}

// Initializes a new processor with the built-in fontset loaded into memory
func NewProcessor() *Processor {
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
    proc := &Processor{pc: prog_start}
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

func (proc *Processor) setFlag(state bool) {
}

func (proc *Processor) SingleStep() {
	// Fetch the 16 bits of an instruction (big endian) and advance the PC
	msb := proc.ram[proc.pc]
	lsb := proc.ram[proc.pc+1]
    fmt.Printf("%X %X %X\n", proc.pc, msb, lsb)
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
	}
}
