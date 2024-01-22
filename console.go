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

import "github.com/veandco/go-sdl2/sdl"

// Representation of the CHIP-8 "console"
type Console struct {
	proc   *Processor
	screen *Screen
	keys   [16]bool // states for every key, true for pressed
}

// Initializes a new console, connecting the processor to itself
func NewConsole() *Console {
	OpenAudio()
	con := &Console{
		screen: NewScreen(),
	}
	con.proc = NewProcessor(con)
	return con
}

// Close the console
func (con *Console) Close() {
	con.screen.Close()
	sdl.CloseAudio()
}

// Runs the program currently loaded into memory
func (con *Console) Run() {
	con.screen.RequestRefresh() // display window from the start
outer:
	for {
		for event := sdl.PollEvent(); event != nil; event = sdl.PollEvent() {
			switch ev := event.(type) {
			case *sdl.QuitEvent:
				break outer
			case *sdl.KeyboardEvent:
				con.handleKeypad(ev)
			}
		}
		start := sdl.GetPerformanceCounter()
		for i := 0; i < 10; i++ {
			con.proc.SingleStep()
		}
		con.proc.UpdateTimers()
		con.screen.Refresh()
		end := sdl.GetPerformanceCounter()
		sleep(start, end)
	}
}

// Handles keypad input
func (con *Console) handleKeypad(event *sdl.KeyboardEvent) {
	value := (event.State == sdl.PRESSED)
	switch event.Keysym.Scancode {
	case sdl.SCANCODE_1:
		con.keys[0x1] = value
	case sdl.SCANCODE_2:
		con.keys[0x2] = value
	case sdl.SCANCODE_3:
		con.keys[0x3] = value
	case sdl.SCANCODE_4:
		con.keys[0xC] = value
	case sdl.SCANCODE_Q:
		con.keys[0x4] = value
	case sdl.SCANCODE_W:
		con.keys[0x5] = value
	case sdl.SCANCODE_E:
		con.keys[0x6] = value
	case sdl.SCANCODE_R:
		con.keys[0xD] = value
	case sdl.SCANCODE_A:
		con.keys[0x7] = value
	case sdl.SCANCODE_S:
		con.keys[0x8] = value
	case sdl.SCANCODE_D:
		con.keys[0x9] = value
	case sdl.SCANCODE_F:
		con.keys[0xe] = value
	case sdl.SCANCODE_Z:
		con.keys[0xA] = value
	case sdl.SCANCODE_X:
		con.keys[0x0] = value
	case sdl.SCANCODE_C:
		con.keys[0xB] = value
	case sdl.SCANCODE_V:
		con.keys[0xF] = value
	}
}

// Checks if a particular key is pressed, used in skip if (not) pressed
func (con *Console) IsPressed(k uint8) bool {
	if k >= 16 {
		return false
	}
	return con.keys[k]
}

// If a key is pressed, return it, used in get key instruction
func (con *Console) GetPressed() (uint8, bool) {
	for k := uint8(0); k < 16; k++ {
		if con.keys[k] {
			return k, true
		}
	}
	return 0, false
}

// Sleep for the correct amout of time to guarantee 60FPS
func sleep(start, end uint64) {
	freq := float64(sdl.GetPerformanceFrequency())
	elapsed := float64(end-start) / freq
	if elapsed < 16.667 {
		sleepTime := uint32(16.667 - elapsed)
		sdl.Delay(sleepTime)
	}
}
