// Copyright 2021 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is
// located in the "LICENSE" file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the
// License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied. See the License for the specific language governing permissions and
// limitations under the License.

package porcupine

import (
	"encoding/binary"
	"io/ioutil"
	"math"
	"path/filepath"
	"testing"
)

func TestProcess(t *testing.T) {

	test_file, _ := filepath.Abs("../../resources/audio_samples/porcupine.wav")

	p := Porcupine{BuiltInKeywords: []BuiltInKeyword{PORCUPINE}}
	err := p.Init()
	if err != nil {
		t.Fatalf("%v", err)
	}

	t.Logf("Porcupine Version: %s", Version)
	t.Logf("Frame Length: %d", FrameLength)
	t.Logf("Sample Rate: %d", SampleRate)

	data, err := ioutil.ReadFile(test_file)
	if err != nil {
		t.Fatalf("Could not read test file: %v", err)
	}
	data = data[44:] // skip header

	frameLenBytes := FrameLength * 2
	frameCount := int(math.Floor(float64(len(data)) / float64(frameLenBytes)))
	sampleBuffer := make([]int16, FrameLength)
	var results []int
	for i := 0; i < frameCount; i++ {
		start := i * frameLenBytes

		for j := 0; j < FrameLength; j++ {
			dataOffset := start + (j * 2)
			sampleBuffer[j] = int16(binary.LittleEndian.Uint16(data[dataOffset : dataOffset+2]))
		}

		result, err := p.Process(sampleBuffer)
		if err != nil {
			t.Fatalf("Could not read test file: %v", err)
		}

		if result == 0 {
			results = append(results, result)
			t.Logf("Keyword triggered at %f", float64((i+1)*FrameLength)/float64(SampleRate))
		}
	}

	if len(results) != 1 || results[0] != 0 {
		t.Fatalf("Failed to find keyword '%s.'", p.BuiltInKeywords[0])
	}

	delErr := p.Delete()
	if delErr != nil {
		t.Fatalf("%v", delErr)
	}
}

func TestMultiple(t *testing.T) {

	test_file, _ := filepath.Abs("../../resources/audio_samples/multiple_keywords.wav")

	p := Porcupine{BuiltInKeywords: []BuiltInKeyword{
		ALEXA, AMERICANO, BLUEBERRY, BUMBLEBEE,
		GRAPEFRUIT, GRASSHOPPER, PICOVOICE, PORCUPINE,
		TERMINATOR}}
	expectedResults := []BuiltInKeyword{PORCUPINE, ALEXA, AMERICANO, BLUEBERRY, BUMBLEBEE, GRAPEFRUIT, GRASSHOPPER, PICOVOICE, PORCUPINE, TERMINATOR}

	err := p.Init()
	if err != nil {
		t.Fatalf("%v", err)
	}

	t.Logf("Porcupine Version: %s", Version)
	t.Logf("Frame Length: %d", FrameLength)
	t.Logf("Samples Rate: %d", SampleRate)

	data, err := ioutil.ReadFile(test_file)
	if err != nil {
		t.Fatalf("Could not read test file: %v", err)
	}
	data = data[44:] // skip header

	var results []int
	frameLenBytes := FrameLength * 2
	frameCount := int(math.Floor(float64(len(data)) / float64(frameLenBytes)))
	sampleBuffer := make([]int16, FrameLength)
	for i := 0; i < frameCount; i++ {
		start := i * frameLenBytes

		for j := 0; j < FrameLength; j++ {
			dataOffset := start + (j * 2)
			sampleBuffer[j] = int16(binary.LittleEndian.Uint16(data[dataOffset : dataOffset+2]))
		}

		result, err := p.Process(sampleBuffer)
		if err != nil {
			t.Fatalf("Could not read test file: %v", err)
		}
		if result >= 0 {
			t.Logf("Keyword %d triggered at %f", result, float64((i+1)*FrameLength)/float64(SampleRate))
			results = append(results, result)
		}
	}

	for i := range results {
		detectedKeyword := p.BuiltInKeywords[results[i]]
		if detectedKeyword != expectedResults[i] {
			t.Fatalf("Expected keyword %s, but %s was detected.", expectedResults[i], detectedKeyword)
		}
	}

	delErr := p.Delete()
	if delErr != nil {
		t.Fatalf("%v", delErr)
	}
}
