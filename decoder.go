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

const (
	CLR   = iota // clears the screen
	JUMP         // unconditional jump to address
	LOAD         // load argument into VX
	ADD          // add argument to VX
	LOADI        // load address into index
	DRAW         // draw stuff to the screen using VX, VY and n
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
		0x6: LOAD,
		0x7: ADD,
		0xA: LOADI,
		0xD: DRAW,
	}
	// For most opcodes, a simple lookup to the decode table suffices
	if opcode != 0x0 && opcode != 0x8 && opcode < 0xE {
		return decodeTable[opcode]
	}
	// The other opcodes each map to multiple operations, so we have to
	// check the full code for the instruction
	switch fullcode {
	case 0x00E0:
		return CLR
	}
	// If we get here, we have an error on our hands
	log.Panicf("Weird instruction 0x%X\n", fullcode)
	return 0 // doesn't happen because panic is called
}
