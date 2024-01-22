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
	"os"

	"github.com/veandco/go-sdl2/sdl"
)

func main() {
	err := sdl.Init(sdl.INIT_EVERYTHING)
	if err != nil {
		log.Panicln("Could not setup SDL2. Is it installed?")
	}
	defer sdl.Quit()

	prog, err := os.ReadFile(os.Args[1])
	if err != nil {
		log.Panicf("Could not read file %s\n", os.Args[1])
	}
	octo := NewConsole()
	defer octo.Close()
	octo.proc.LoadProg(prog)
	octo.Run()
}
