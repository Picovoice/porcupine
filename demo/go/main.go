package main

import (
	"bufio"
	"bytes"
	"encoding/binary"
	"errors"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"os/signal"
	"strconv"
	"strings"

	porcupine "github.com/charithe/porcupine-go"
)

type keywordFlags []*porcupine.Keyword

func (kf *keywordFlags) Set(v string) error {
	parts := strings.Split(v, ":")
	if len(parts) != 3 {
		return errors.New("expected flag value to contain the keyword, filepath and sensitivity separated by colon charcters")
	}

	sensitivity, err := strconv.ParseFloat(parts[2], 32)
	if err != nil {
		return err
	}

	*kf = append(*kf, &porcupine.Keyword{Value: parts[0], FilePath: parts[1], Sensitivity: float32(sensitivity)})
	return nil
}

func (kf *keywordFlags) String() string {
	var sb strings.Builder
	for _, k := range *kf {
		sb.WriteString(fmt.Sprintf("%s:%s:%f", k.Value, k.FilePath, k.Sensitivity))
		sb.WriteString(", ")
	}
	return sb.String()
}

func main() {
	var input string
	var modelPath string
	var keywords keywordFlags

	flag.StringVar(&input, "input", "-", "Path to read input audio from (PCM 16-bit LE)")
	flag.StringVar(&modelPath, "model_path", "", "Path to the Porcupine model")
	flag.Var(&keywords, "keyword", "Colon separated keyword, data file and sensitivity values (Eg. pineapple:pineapple_linux.ppn:0.5)")
	flag.Parse()

	if input == "" || modelPath == "" || len(keywords) == 0 {
		fmt.Fprintln(os.Stderr, "Usage: ./demo -input=<path_to_audio_input_data> -model_path=<path_to_model> -keyword=<keyword:path_to_data_file:sensitivity>")
		os.Exit(2)
	}

	p, err := porcupine.New(modelPath, keywords...)
	if err != nil {
		log.Fatalf("failed to initialize porcupine: %+v", err)
	}
	defer p.Close()

	var audio io.Reader
	if input == "-" {
		audio = bufio.NewReader(os.Stdin)
	} else {
		f, err := os.Open(input)
		if err != nil {
			log.Fatalf("failed to open input [%s]: %+v", input, err)
		}
		defer f.Close()

		audio = bufio.NewReader(f)
	}

	shutdownChan := make(chan os.Signal, 1)
	signal.Notify(shutdownChan, os.Interrupt)

	listen(p, audio, shutdownChan)
}

func listen(p porcupine.Porcupine, audio io.Reader, shutdownChan <-chan os.Signal) {
	frameSize := porcupine.FrameLength()
	audioFrame := make([]int16, frameSize)
	buffer := make([]byte, frameSize*2)

	log.Printf("listening...")

	for {
		select {
		case <-shutdownChan:
			log.Printf("shutting down")
			return
		default:
			if err := readAudioFrame(audio, buffer, audioFrame); err != nil {
				log.Printf("error: %+v", err)
				return
			}

			word, err := p.Process(audioFrame)
			if err != nil {
				log.Printf("error: %+v", err)
				continue
			}

			if word != "" {
				log.Printf("detected word: \"%s\"", word)
			}
		}
	}
}

func readAudioFrame(src io.Reader, buffer []byte, audioFrame []int16) error {
	_, err := io.ReadFull(src, buffer)
	if err != nil {
		return err
	}

	buf := bytes.NewBuffer(buffer)
	for i := 0; i < len(audioFrame); i++ {
		if err := binary.Read(buf, binary.LittleEndian, &audioFrame[i]); err != nil {
			return err
		}
	}

	return nil
}
