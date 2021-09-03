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
    "os/signal"

    . "github.com/Picovoice/porcupine/binding/go"
    pvrecorder "github.com/Picovoice/pvrecorder/sdk/go"
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

    err := p.Init()
    if err != nil {
        log.Fatal(err)
    }
    defer p.Delete()

    recorder := pvrecorder.PvRecorder{
        DeviceIndex: *audioDeviceIndex,
        FrameLength: FrameLength,
        BufferSizeMSec: 1000,
        LogOverflow: 0,
    }

    if err := recorder.Init(); err != nil {
        log.Fatalf("Error: %s.\n", err.Error())
    }
    defer recorder.Delete()

    log.Printf("Using device: %s", recorder.GetSelectedDevice())

    if err := recorder.Start(); err != nil {
        log.Fatalf("Error: %s.\n", err.Error())
    }

    signalCh := make(chan os.Signal, 1)
    waitCh := make(chan struct{})
    signal.Notify(signalCh, os.Interrupt)

    go func () {
        <- signalCh
        close(waitCh)
    }()
    
    waitLoop:
    for {
        select {
        case <- waitCh:
            log.Println("Stopping...")
            break waitLoop
        default:
            pcm, err := recorder.Read()
            if err != nil {
                log.Fatalf("Error: %s.\n", err.Error())
            }
            keywordIndex, err := p.Process(pcm)
            if err != nil {
                log.Fatal(err)
            }
            if keywordIndex >= 0 {
                fmt.Printf("Keyword %d detected\n", keywordIndex)
            }
            // write to debug file
            if outputWav != nil {
                for outputBufIndex := range pcm {
                    outputWav.WriteFrame(pcm[outputBufIndex])
                }
            }
        }
    }
}

func printAudioDevices() {
    if devices, err := pvrecorder.GetAudioDevices(); err != nil {
        log.Fatalf("Error: %s.\n", err.Error())
    } else {
        for i, device := range devices {
            log.Printf("index: %d, device name: %s\n", i, device)
        }
    }
}
