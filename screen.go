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

	"github.com/veandco/go-sdl2/sdl"
)

// Graphics-related constants
const (
	W          = 64  // screen width
	H          = 32  // screen height
	scale      = 10  // scale factor for each pixel
	lerpFactor = 0.6 // lerp fading factor
)

// RGBA color representation
type color struct {
	red, green, blue, alpha byte
}

// Representation of the CHIP-8 screen, with the SDL objects needed to render it
type Screen struct {
	window      *sdl.Window
	renderer    *sdl.Renderer
	display     [H * W]bool
	pixels      [H * W]color
	refreshFlag bool
}

var colorOn = color{
	red:   0xFF,
	green: 0xFF,
	blue:  0xFF,
	alpha: 0xFF,
}

var colorOff = color{
	red:   0x00,
	green: 0x00,
	blue:  0x00,
	alpha: 0xFF,
}

// Initializes a new screen
func NewScreen() *Screen {
	// Create window though SDL
	win, err := sdl.CreateWindow("CHIP-8 emulator", sdl.WINDOWPOS_UNDEFINED,
		sdl.WINDOWPOS_UNDEFINED, W*scale, H*scale, sdl.WINDOW_SHOWN)
	if err != nil {
		log.Panicln("Could not create the graphical window")
	}
	// Create SDL renderer for the window
	ren, err := sdl.CreateRenderer(win, -1, sdl.RENDERER_ACCELERATED)
	if err != nil {
		log.Panicln("Could not setup a renderer for the window")
	}
	// Return the initialized screen
	return &Screen{
		window:   win,
		renderer: ren,
	}
}

// Closes ("destroys") the screen
func (scr *Screen) Close() {
	scr.window.Destroy()
}

// Set state of a pixel (on/off)
func (scr *Screen) SetPixel(x, y uint32, on bool) {
	scr.display[y*W+x] = on
}

// Get state of a pixel (on/off)
func (scr *Screen) GetPixel(x, y uint32) bool {
	return scr.display[y*W+x]
}

// Marks any changes to the screen as complete, triggering graphics to be
// refreshed at the end of the next cycle
func (scr *Screen) RequestRefresh() {
	scr.refreshFlag = true
}

// Clear the contents of the screen
func (scr *Screen) Clear() {
	for i := 0; i < H*W; i++ {
		scr.display[i] = false
	}
	scr.refreshFlag = true
}

// Refresh graphics if, and only if it was requested
func (scr *Screen) Refresh() {
	if !scr.refreshFlag {
		return
	}
	scr.refreshFlag = false
	// Map the conceptual model to the actual pixel colors
	for i := 0; i < H*W; i++ {
		if scr.display[i] && scr.pixels[i] != colorOn {
			// instantly transition to the "on" color
			scr.pixels[i] = colorOn
		} else if !scr.display[i] && scr.pixels[i] != colorOff {
			// progressively fade to the "off" color
			scr.pixels[i] = lerpColor(scr.pixels[i], colorOff, lerpFactor)
		}
	}
	// Now draw the actual pixel colors to the screen
	rect := sdl.Rect{W: scale, H: scale}
	for y := int32(0); y < H; y++ {
		for x := int32(0); x < W; x++ {
			rect.X = x * scale
			rect.Y = y * scale
			scr.setDrawColor(scr.pixels[y*W+x])
			scr.renderer.FillRect(&rect)
		}
	}
	scr.renderer.Present()
}

// Helper function to use the color structure in the renderer
func (scr *Screen) setDrawColor(c color) {
	scr.renderer.SetDrawColor(c.red, c.green, c.blue, c.alpha)
}

// Linearly interpolate two colors by the given t factor
func lerpColor(c0, c1 color, t float64) color {
	return color{
		// Linear interpolation of the individual components
		red:   byte((1-t)*float64(c0.red) + t*float64(c1.red)),
		green: byte((1-t)*float64(c0.green) + t*float64(c1.green)),
		blue:  byte((1-t)*float64(c0.blue) + t*float64(c1.blue)),
		alpha: byte((1-t)*float64(c0.alpha) + t*float64(c1.alpha)),
	}
}
