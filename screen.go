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

const (
	w          = 64  // screen width
	h          = 32  // screen height
	scale      = 10  // scale factor for each pixel
	lerpFactor = 0.5 // lerp fading factor
)

// RGBA color representation
type color struct {
	red, green, blue, alpha byte
}

// Representation of the chip8 screen, with the SDL objects needed to render it
type Screen struct {
	window      *sdl.Window
	renderer    *sdl.Renderer
	texture     *sdl.Texture
	display     [h * w]bool
	pixels      [h * w * 4]byte
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
	win, err := sdl.CreateWindow("Chip8 emulator", sdl.WINDOWPOS_UNDEFINED,
		sdl.WINDOWPOS_UNDEFINED, w*scale, h*scale, sdl.WINDOW_SHOWN)
	if err != nil {
		log.Panicln("Could not create the graphical window")
	}
	// Create SDL renderer for the window
	ren, err := sdl.CreateRenderer(win, -1, sdl.RENDERER_ACCELERATED)
	if err != nil {
		log.Panicln("Could not setup a renderer for the window")
	}
	ren.SetLogicalSize(w, h)
	// Create a texture to render things to
	tex, err := ren.CreateTexture(sdl.PIXELFORMAT_RGBA8888,
		sdl.TEXTUREACCESS_STREAMING, w, h)
	if err != nil {
		log.Panicln("Could not create the main texture")
	}
	// Return the initialized screen
	return &Screen{
		window:   win,
		renderer: ren,
		texture:  tex,
	}
}

// Closes ("destroys") the screen
func (scr *Screen) Close() {
	scr.window.Destroy()
}

// Set state of a pixel (on/off)
func (scr *Screen) SetPixel(x, y byte, on bool) {
    scr.display[y*w+x] = on;
}

// Get state of a pixel (on/off)
func (scr *Screen) GetPixel(x, y byte) bool {
    return scr.display[y*w+x]
}

// Marks any changes to the screen as complete, triggering graphics to be
// refreshed at the end of the next cycle
func (scr *Screen) RequestRefresh() {
	scr.refreshFlag = true
}

// Clear the contents of the screen
func (scr *Screen) Clear() {
	for i := 0; i < h*w; i++ {
		scr.display[i] = false
	}
	scr.refreshFlag = true
}

// Refresh graphics if, and only if it was requested
func (scr *Screen) Refresh() {
    // NOTE this is kind of garbage. The approach I used in the C version is
    // judged insecure by go, so I had to adapt. I honestly like the general
    // idea of this new approach a lot better, as it feels less "magical". At
    // the same time though, the concrete implementation needes more polish.
    // I'm pretty sure I've hardcoded little endianess into this thing, and
    // the meat of the mapping feels clumsy to read and edit.
    if !scr.refreshFlag {
        return
    }
    scr.refreshFlag = false
    // Map the conceptual model to the raw pixel bytes
    for i := 0; i < h*w; i++ {
        if scr.display[i] && scr.pixels[i] != colorOn.red {
            // Instantly transition to the "on" color
            scr.pixels[i] = colorOn.red
            scr.pixels[i + 1] = colorOn.blue
            scr.pixels[i + 2] = colorOn.green
            scr.pixels[i + 3] = colorOn.alpha
        } else if !scr.display[i] && scr.pixels[i] != colorOff.red {
            // Progressively fade to the "off" color via lerping
            currentColor := color{
                red: scr.pixels[i],
                blue: scr.pixels[i + 1],
                green: scr.pixels[i + 2],
                alpha: scr.pixels[i + 3],
            }
            fadeColor := lerpColor(currentColor, colorOff, lerpFactor)
            scr.pixels[i] = fadeColor.red
            scr.pixels[i + 1] = fadeColor.blue
            scr.pixels[i + 2] = fadeColor.green
            scr.pixels[i + 3] = fadeColor.alpha
        }
    }
	// Do texture locking shananigans to render the thing, I'm not really sure
	// if this is the best way to do it, but it sure does work
	pixels, _, err := scr.texture.Lock(nil)
	if err != nil {
		log.Panicln("Could not lock the main texture for rendering")
	}
	copy(pixels, scr.pixels[:])
	scr.texture.Unlock()
	scr.renderer.Copy(scr.texture, nil, nil)
	scr.renderer.Present()
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
