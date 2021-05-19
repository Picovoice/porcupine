package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	. "github.com/Picovoice/porcupine/binding/go"
	"github.com/go-audio/audio"
	"github.com/go-audio/wav"
)

func main() {
	inputAudioPathArg := flag.String("input_audio_path", "", "Path to input audio file (mono, WAV, 16-bit, 16kHz)")
	keywordsArg := flag.String("keywords", "", "Comma-separated list of built-in keywords")
	keywordPathsArg := flag.String("keyword_paths", "", "Comma-separated list of paths to keyword model files. "+
		"If not set it will be populated from `--keywords` argument")
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

	p := Porcupine{}
	if *modelPathArg != "" {
		modelPath, _ := filepath.Abs(*modelPathArg)
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
			if err := builtInKeyword.IsValid(); err != nil {
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
				log.Fatalf("Senstivity value of '%s' is invalid. Must be a float32 between [0, 1].", sensitivityStr)
			}
			p.Sensitivities = append(p.Sensitivities, float32(sensitivityFloat))
		}
	}

	status, err := p.Init()
	if status != 0 || err != nil {
		log.Fatal(status, err)
	}

	buf := &audio.IntBuffer{
		Format: &audio.Format{
			NumChannels: 1,
			SampleRate:  16000,
		},
		Data:           make([]int, FrameLength),
		SourceBitDepth: 16,
	}

	shortBuf := make([]int16, FrameLength)
	var n int
	totalRead := 0
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
			fmt.Printf("Keyword %d detected at %f seconds.\n", keywordIndex, float64(totalRead)/float64(SampleRate))
		}
	}

	p.Delete()
}
