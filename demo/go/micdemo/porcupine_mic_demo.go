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
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"runtime"
	"strconv"
	"strings"

	. "github.com/Picovoice/porcupine/binding/go"
	"github.com/gen2brain/malgo"
	"github.com/go-audio/wav"
)

func main() {
	keywordsArg := flag.String("keywords", "", fmt.Sprintf("Comma-separated list of built-in keywords. Available options are: %+q", BuiltInKeywords))
	keywordPathsArg := flag.String("keyword_paths", "", "Comma-separated list of paths to keyword model files. "+
		"If not set it will be populated from -keywords argument")
	modelPathArg := flag.String("model_path", "", "Path to Porcupine model file")
	sensitivitiesArg := flag.String("sensitivities", "", "Comma-separated list of sensitivity values for detecting keywords."+
		"Each value should be a number within [0, 1]. A higher "+
		"sensitivity results in fewer misses at the cost of increasing the false alarm rate. "+
		"If not set 0.5 will be used.")
	audioDeviceIndex := flag.Int("audio_device_index", -1, "Index of capture device to use.")
	outputPathArg := flag.String("output_path", "", "Path to recorded audio (for debugging)")
	showAudioDevices := flag.Bool("show_audio_devices", false, "Display all available capture devices")
	flag.Parse()

	if *showAudioDevices {
		printAudioDevices()
		return
	}

	var outputWav *wav.Encoder
	if *outputPathArg != "" {
		outputFilePath, _ := filepath.Abs(*outputPathArg)
		outputFile, err := os.Create(outputFilePath)
		if err != nil {
			log.Fatalf("Failed to create output audio at path %s", outputFilePath)
		}
		defer outputFile.Close()

		outputWav = wav.NewEncoder(outputFile, SampleRate, 16, 1, 1)
		defer outputWav.Close()
	}

	p := Porcupine{}

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
			builtInKeyword := BuiltInKeyword(k)
			if !builtInKeyword.IsValid() {
				log.Fatalf("'%s' is not a valid built-in keyword. Available options are: %+q", k, BuiltInKeywords)
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
				log.Fatalf("Senstivity value of '%s' is invalid. Must be a float32 between [0, 1].", sensitivityStr)
			}
			p.Sensitivities = append(p.Sensitivities, float32(sensitivityFloat))
		}
	}

	var backends []malgo.Backend = nil
	if runtime.GOOS == "windows" {
		backends = []malgo.Backend{malgo.BackendWinmm}
	} else if runtime.GOOS == "linux" {
		backends = []malgo.Backend{malgo.BackendAlsa}
	}

	context, err := malgo.InitContext(backends, malgo.ContextConfig{}, func(message string) {
		fmt.Printf("%v\n", message)
	})
	if err != nil {
		log.Fatal(err)
	}
	defer func() {
		_ = context.Uninit()
		context.Free()
	}()

	deviceConfig := malgo.DefaultDeviceConfig(malgo.Duplex)
	deviceConfig.Capture.Format = malgo.FormatS16
	deviceConfig.Capture.Channels = 1
	deviceConfig.SampleRate = 16000

	if *audioDeviceIndex >= 0 {
		infos, err := context.Devices(malgo.Capture)
		if err != nil {
			log.Fatal(err)
		}

		if *audioDeviceIndex > len(infos)-1 {
			fmt.Printf("Audio device at index %d does not exist. Using default capture device.\n", *audioDeviceIndex)
		} else {
			deviceConfig.Capture.DeviceID = infos[*audioDeviceIndex].ID.Pointer()
		}
	}

	err = p.Init()
	if err != nil {
		log.Fatal(err)
	}
	defer p.Delete()

	var shortBufIndex, shortBufOffset int
	shortBuf := make([]int16, FrameLength)

	onRecvFrames := func(pSample2, pSample []byte, framecount uint32) {
		for i := 0; i < len(pSample); i += 2 {
			shortBuf[shortBufIndex+shortBufOffset] = int16(binary.LittleEndian.Uint16(pSample[i : i+2]))
			shortBufOffset++

			if shortBufIndex+shortBufOffset == FrameLength {
				shortBufIndex = 0
				shortBufOffset = 0
				keywordIndex, err := p.Process(shortBuf)
				if err != nil {
					log.Fatal(err)
				}
				if keywordIndex >= 0 {
					fmt.Printf("Keyword %d detected\n", keywordIndex)
				}

				// write to debug file
				if outputWav != nil {
					for outputBufIndex := range shortBuf {
						outputWav.WriteFrame(shortBuf[outputBufIndex])
					}
				}
			}
		}
		shortBufIndex += shortBufOffset
		shortBufOffset = 0
	}

	captureCallbacks := malgo.DeviceCallbacks{
		Data: onRecvFrames,
	}
	device, err := malgo.InitDevice(context.Context, deviceConfig, captureCallbacks)
	if err != nil {
		log.Fatal(err)
	}

	err = device.Start()
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Press Enter to stop recording.")
	fmt.Println("Listening...")
	fmt.Scanln()

	device.Uninit()
}

func printAudioDevices() {
	context, err := malgo.InitContext(nil, malgo.ContextConfig{}, nil)
	if err != nil {
		log.Fatal(err)
	}
	defer func() {
		_ = context.Uninit()
		context.Free()
	}()

	// Capture devices.
	infos, err := context.Devices(malgo.Capture)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Capture Devices")
	for i, info := range infos {
		fmt.Printf("    %d: %s\n", i, strings.Replace(info.Name(), "\x00", "", -1))
	}
}
