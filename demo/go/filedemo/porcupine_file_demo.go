// Copyright 2021 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is
// located in the "LICENSE" file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the
// License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied. See the License for the specific language governing permissions and
// limitations under the License.

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"time"

	porcupine "github.com/Picovoice/porcupine/binding/go/v2"
	"github.com/go-audio/audio"
	"github.com/go-audio/wav"
)

func main() {
	inputAudioPathArg := flag.String("input_audio_path", "", "Path to input audio file (mono, WAV, 16-bit, 16kHz)")
	accessKeyArg := flag.String("access_key", "", "AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)")
	keywordsArg := flag.String("keywords", "", fmt.Sprintf("Comma-separated list of built-in keywords. Available options are: %+q", porcupine.BuiltInKeywords))
	keywordPathsArg := flag.String("keyword_paths", "", "Comma-separated list of paths to keyword model files. "+
		"If not set it will be populated from -keywords argument")
	libraryPathArg := flag.String("library_path", "", "Path to Porcupine dynamic library file")
	modelPathArg := flag.String("model_path", "", "Path to Porcupine model file")
	sensitivitiesArg := flag.String("sensitivities", "", "Comma-separated list of sensitivity values for detecting keywords."+
		"Each value should be a number within [0, 1]. A higher "+
		"sensitivity results in fewer misses at the cost of increasing the false alarm rate. "+
		"If not set 0.5 will be used.")
	flag.Parse()

	// validate input audio
	if *inputAudioPathArg == "" {
		log.Fatal("No input audio file provided.")
	}
	inputAudioPath, _ := filepath.Abs(*inputAudioPathArg)
	f, err := os.Open(inputAudioPath)
	if err != nil {
		log.Fatalf("Unable to find or open input audio at %s", inputAudioPath)
	}
	defer f.Close()

	wavFile := wav.NewDecoder(f)
	if !wavFile.IsValidFile() || wavFile.BitDepth != 16 || wavFile.SampleRate != 16000 || wavFile.NumChans != 1 {
		log.Fatal("Invalid WAV file. File must contain mono, 16-bit, 16kHz linearly encoded PCM.")
	}

	p := porcupine.Porcupine{}
	defer func() {
		err := p.Delete()
		if err != nil {
			log.Fatalf("Failed to release resources: %s", err)
		}
	}()

	if *accessKeyArg == "" {
		log.Fatalf("AccessKey is required.")
	}
	p.AccessKey = *accessKeyArg

	// validate library path
	if *libraryPathArg != "" {
		libraryPath, _ := filepath.Abs(*libraryPathArg)
		if _, err := os.Stat(libraryPath); os.IsNotExist(err) {
			log.Fatalf("Could not find library file at %s", libraryPath)
		}

		p.LibraryPath = libraryPath
	}

	// validate model
	if *modelPathArg != "" {
		modelPath, _ := filepath.Abs(*modelPathArg)
		if _, err := os.Stat(modelPath); os.IsNotExist(err) {
			log.Fatalf("Could not find model file at %s", modelPath)
		}

		p.ModelPath = modelPath
	}

	// validate keyword arguments
	keywordPathsSplit := strings.Split(*keywordPathsArg, ",")
	if *keywordPathsArg == "" || len(keywordPathsSplit) == 0 {
		keywordsSplit := strings.Split(*keywordsArg, ",")
		if *keywordsArg == "" || len(keywordsSplit) == 0 {
			log.Fatal("No built-in keywords or keyword model files were provided.")
		}

		for _, k := range keywordsSplit {
			builtInKeyword := porcupine.BuiltInKeyword(k)
			if !builtInKeyword.IsValid() {
				log.Fatalf("'%s' is not a valid built-in keyword.", k)
			}

			p.BuiltInKeywords = append(p.BuiltInKeywords, builtInKeyword)
		}
	} else {
		for _, k := range keywordPathsSplit {
			keywordPath, _ := filepath.Abs(k)
			if _, err := os.Stat(keywordPath); os.IsNotExist(err) {
				log.Fatalf("Could not find keyword file at %s", keywordPath)
			}
			p.KeywordPaths = append(p.KeywordPaths, keywordPath)
		}
	}

	// validate sensitivities
	sensitivitiesSplit := strings.Split(*sensitivitiesArg, ",")
	if *sensitivitiesArg == "" || len(sensitivitiesSplit) == 0 {
		for range p.KeywordPaths {
			p.Sensitivities = append(p.Sensitivities, 0.5)
		}
	} else {
		for _, sensitivityStr := range sensitivitiesSplit {
			sensitivityFloat, err := strconv.ParseFloat(sensitivityStr, 32)
			if err != nil || sensitivityFloat < 0 || sensitivityFloat > 1 {
				log.Fatalf("Sensitivity value of '%s' is invalid. Must be a float32 between [0, 1].", sensitivityStr)
			}
			p.Sensitivities = append(p.Sensitivities, float32(sensitivityFloat))
		}
	}

	err = p.Init()
	if err != nil {
		log.Fatal(err)
	}

	buf := &audio.IntBuffer{
		Format: &audio.Format{
			NumChannels: 1,
			SampleRate:  16000,
		},
		Data:           make([]int, porcupine.FrameLength),
		SourceBitDepth: 16,
	}

	shortBuf := make([]int16, porcupine.FrameLength)
	var n int
	totalRead := 0
	keywordsDetected := false
	start := time.Now()
	for err == nil {
		n, err = wavFile.PCMBuffer(buf)
		if err != nil {
			log.Fatal("Failed to read from WAV file.", err)
		}

		if n == 0 {
			break
		}

		totalRead += n
		for i := range buf.Data {
			shortBuf[i] = int16(buf.Data[i])
		}

		keywordIndex, err := p.Process(shortBuf)
		if err != nil {
			log.Fatal(err)
		}

		if keywordIndex >= 0 {
			keywordsDetected = true
			fmt.Printf("Keyword %d detected at %f seconds.\n", keywordIndex, float64(totalRead)/float64(porcupine.SampleRate))
		}
	}
	if !keywordsDetected {
		fmt.Println("None of the provided keywords were found in the input audio file.")
	}

	audioLen := float64(totalRead) / float64(porcupine.SampleRate)
	elapsed := time.Since(start)
	realtimeFactor := audioLen / elapsed.Seconds()
	fmt.Printf("Realtime factor: %fx\n", realtimeFactor)
}
