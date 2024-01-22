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

import "log"

// The worst substitute for an enumeration type you have ever seen in your
// life. I like golang, but boy is this the dumbest thing of all time.
// Anyways, this represents all the distinct operations possible in chip8.

type operation uint8

// TODO add missing instructions (key-related)
const (
	ERROR = iota
	CALL  // calls subroutine at address
	RTS   // returns from subroutine
	JUMP  // unconditional jump to address
	JUMP0 // unconditional jump to address + V0

	SKE   // skips if VX equals argument
	SKNE  // skips if VX doesn't equal argument
	SKRE  // skips if VX equals VY
	SKRNE // skips if VX doesn't equal VY

	LOAD  // loads argument into VX
	LOADI // loads address into index
	LOADD // loads the value of VX to delay timer
	LOADS // loads the value of VX to sound timer
	LDCHR // points index to the character sprite of VX
	MOVE  // moves value from VY to VX
	MOVED // moves value from delay timer to VX
	OR    // VY |= VX
	AND   // VY &= VX
	XOR   // VY ^= VX

	ADD  // adds argument to VX
	ADDR // add VY to VX, setting flag on carry
	ADDI // adds VX to index
	SUB  // VX -= VY, clearing flag on borrow
	RSUB // VX = VY - VX, clearing flag on borrow
	SHR  // VX = VY >> 1
	SHL  // VX = VY << 1
	RAND // puts random number into VX

	CLS  // clears the screen
	DRAW // draws stuff to the screen using VX, VY and n

	BCD   // converts VX to BCD and stores it in RAM, where the index points
	STORE // stores registers into memory
	READ  // reads registers from memory
)

type Instruction struct {
	x, y, n, arg uint8
	addr         uint16
	op           operation
}

func Decode(msb, lsb uint8) Instruction {
	fullcode := (uint16(msb) << 8) | uint16(lsb)
	opcode := msb >> 4
	return Instruction{
		x:    msb & 0xF,
		y:    lsb >> 4,
		n:    lsb & 0xF,
		arg:  lsb,
		addr: fullcode & 0xFFF,
		op:   decodeOperation(opcode, fullcode),
	}
}

func decodeOperation(opcode uint8, fullcode uint16) operation {
	// Auxiliary table used to simplify decoding
	var decodeTable = [...]operation{
		0x1: JUMP,
		0x2: CALL,
		0x3: SKE,
		0x4: SKNE,
		0x5: SKRE,
		0x6: LOAD,
		0x7: ADD,
		0x9: SKRNE,
		0xA: LOADI,
		0xB: JUMP0,
		0xD: DRAW,
	}
	// Specific table for logical and arithmetic instructions (opcode: 0x8)
	var aritDecodeTable = [...]operation{
		0x0: MOVE,
		0x1: OR,
		0x2: AND,
		0x3: XOR,
		0x4: ADDR,
		0x5: SUB,
		0x6: SHR,
		0x7: RSUB,
		0xE: SHL,
	}
	// Specific table for misc instructions (opcode: 0xF)
	var miscDecodeTable = [...]operation{
		0x07: MOVED,
		0x15: LOADD,
		0x18: LOADS,
		0x1E: ADDI,

		0x29: LDCHR,
		0x33: BCD,
		0x55: STORE,
		0x65: READ,
	}
	// For most opcodes, a simple lookup to the general decode table suffices
	if opcode != 0x0 && opcode != 0x8 && opcode < 0xE {
		return decodeTable[opcode]
	}
	// The other opcodes each map to multiple operations, so we have to
	// check the full code for the instruction, or a specific other part of it
	if opcode == 0x8 {
		n := fullcode & 0xF
		return aritDecodeTable[n]
	} else if opcode == 0xF {
		halfcode := fullcode & 0xFF
		return miscDecodeTable[halfcode]
	}
	switch fullcode {
	case 0x00E0:
		return CLS
	case 0x00EE:
		return RTS
	}
	// If we get here, we have an error on our hands (0xE is not implemented yet)
	log.Panicf("Could not decode 0x%04X\n", fullcode)
	return 0 // doesn't happen because panic is called
}
