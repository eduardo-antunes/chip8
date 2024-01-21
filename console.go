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

type Console struct {
	proc   *Processor
	screen *Screen
}

func NewConsole() *Console {
	return &Console{
		proc:   NewProcessor(),
		screen: NewScreen(),
	}
}

func (con *Console) Close() {
	con.screen.Close()
}

// Pretty stupid skeleton
func (con *Console) Run() {
	quit := 100_000
	con.screen.RequestRefresh()
	for quit > 0 {
		quit--
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
