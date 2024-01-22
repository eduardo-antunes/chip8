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

// typedef unsigned char Uint8;
// void squareWave(void *userdata, Uint8 *stream, int len);
import "C"
import (
	"log"
	"unsafe"

	"github.com/veandco/go-sdl2/sdl"
)

// Audio-related constants
const (
	numChannels = 2
	sampleRate  = 44_100 // sample rate of the beep
	volume      = 127    // volume/amplitude of the beep
	tone        = 441    // frequency of the beep
)

// I'm using global variables here just to avoid the pain of dealing with the
// userdata parameter for now; that will change in the future though
var (
	currentVolume = C.Uint8(volume)
	count         = 0
)

//export squareWave
func squareWave(u unsafe.Pointer, rawBuf *C.Uint8, length C.int) {
	n := int(length)
	buf := unsafe.Slice(rawBuf, n)
	// You may ask why I made this code generic over the number of channels,
	// if there's no need for it to be something other than 2 99% of the time.
	// This is a great question
	for i := 0; i < n; i += numChannels {
		for j := 0; j < numChannels; j++ {
			buf[i+j] = currentVolume
		}
		if count++; count > sampleRate/(tone*2) {
			// Flip between the maximum volume and 0 for each duty cycle, where
			// each such cycle takes up about half of the sample frame. This
			// will produce a nice square wave
			currentVolume = volume - currentVolume
			count = 0
		}
	}
}

// Opens the audio channel through SDL2
func OpenAudio() {
	audioSpec := &sdl.AudioSpec{
		Samples:  512,
		Freq:     sampleRate,
		Channels: numChannels,
		Format:   sdl.AUDIO_U8,
		Callback: sdl.AudioCallback(C.squareWave),
	}
	if err := sdl.OpenAudio(audioSpec, nil); err != nil {
		log.Panicln("Could not open audio")
	}
}
