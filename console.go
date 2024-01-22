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
	"github.com/veandco/go-sdl2/sdl"
)

// Representation of the chip8 "console"
type Console struct {
	proc   *Processor
	screen *Screen
}

// Initializes a new console, connecting the processor to itself
func NewConsole() *Console {
	con := &Console{
		screen: NewScreen(),
	}
	con.proc = NewProcessor(con)
	return con
}

// Close the console
func (con *Console) Close() {
	con.screen.Close()
}

// Runs the program currently loaded into memory
func (con *Console) Run() {
	quit := false
	con.screen.RequestRefresh() // display window from the start
	for {
		for event := sdl.PollEvent(); event != nil; event = sdl.PollEvent() {
			switch event.(type) {
			case *sdl.QuitEvent:
				quit = true
			}
		}
		if quit {
			break
		}

		before := sdl.GetPerformanceCounter()
		for i := 0; i < 12; i++ {
			con.proc.SingleStep()
		}
		after := sdl.GetPerformanceCounter()
		elapsed := (after - before) * 1000 / sdl.GetPerformanceFrequency()
		sdl.Delay(uint32(elapsed))
		con.screen.Refresh()
	}
}
