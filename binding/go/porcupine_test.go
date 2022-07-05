// Copyright 2021-2022 Picovoice Inc.
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
	"flag"
	"fmt"
	"io/ioutil"
	"math"
	"os"
	"path/filepath"
	"reflect"
	"testing"
)

var (
	testAccessKey string
	porcupine     Porcupine
)

var singleKeywordTestParameters = []struct {
	language        string
	keywords        []string
	expectedResults []int
	testAudioFile   string
}{
	{"en", []string{"porcupine"}, []int{0}, "porcupine.wav"},
	{"es", []string{"manzana"}, []int{0}, "manzana.wav"},
	{"de", []string{"heuschrecke"}, []int{0}, "heuschrecke.wav"},
	{"fr", []string{"mon chouchou"}, []int{0}, "mon_chouchou.wav"},
}

var multipleKeywordTestParameters = []struct {
	language        string
	keywords        []string
	expectedResults []int
}{
	{
		"en",
		[]string{"americano", "blueberry", "bumblebee", "grapefruit", "grasshopper", "picovoice", "porcupine", "terminator"},
		[]int{6, 0, 1, 2, 3, 4, 5, 6, 7},
	},
	{
		"es",
		[]string{"emparedado", "leopardo", "manzana"},
		[]int{0, 1, 2},
	},
	{
		"de",
		[]string{"ananas", "heuschrecke", "leguan", "stachelschwein"},
		[]int{0, 1, 2, 3},
	},
	{
		"fr",
		[]string{"framboise", "mon chouchou", "parapluie"},
		[]int{0, 1, 0, 2},
	},
	{
		"it",
		[]string{"espresso", "cameriere", "porcospino"},
		[]int{2, 0, 1},
	},
	{
		"ja",
		[]string{"ninja", "bushi", "ringo"},
		[]int{2, 1, 0},
	},
	{
		"ko",
		[]string{"aiseukeulim", "bigseubi", "koppulso"},
		[]int{1, 2, 0},
	},
	{
		"pt",
		[]string{"abacaxi", "fenomeno", "formiga"},
		[]int{0, 2, 1},
	},
}

func TestMain(m *testing.M) {

	flag.StringVar(&testAccessKey, "access_key", "", "AccessKey for testing")
	flag.Parse()

	os.Exit(m.Run())
}

func appendLanguage(s string, language string) string {
	if language == "en" {
		return s
	}
	return s + "_" + language
}

func getTestModelPath(language string) string {
	modelRelPath := fmt.Sprintf(
		"../../lib/common/%s.pv",
		appendLanguage("porcupine_params", language))
	modelPath, _ := filepath.Abs(modelRelPath)
	return modelPath
}

func getTestKeywordPath(language string, keyword string) string {
	keywordRelPath := fmt.Sprintf(
		"../../resources/%s/%s/%s_%s.ppn",
		appendLanguage("keyword_files", language),
		osName,
		keyword,
		osName)
	keywordPath, _ := filepath.Abs(keywordRelPath)
	return keywordPath
}

func getTestKeywordPaths(language string, keywords []string) []string {
	keywordPaths := []string{}
	for _, keyword := range keywords {
		keywordPaths = append(keywordPaths, getTestKeywordPath(language, keyword))
	}
	return keywordPaths
}

func runTestCase(
	t *testing.T,
	porcupine *Porcupine,
	audioFileName string,
	expectedResults []int) {

	err := porcupine.Init()
	if err != nil {
		t.Fatalf("%v", err)
	}

	fmt.Printf("Porcupine Version: %s\n", Version)
	fmt.Printf("Frame Length: %d\n", FrameLength)
	fmt.Printf("Sample Rate: %d\n", SampleRate)

	testAudioPath, _ := filepath.Abs(filepath.Join("../../resources/audio_samples", audioFileName))
	data, err := ioutil.ReadFile(testAudioPath)
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

		result, err := porcupine.Process(sampleBuffer)
		if err != nil {
			t.Fatalf("Could not read test file: %v", err)
		}
		if result >= 0 {
			fmt.Printf("Keyword %d triggered at %f\n", result, float64((i+1)*FrameLength)/float64(SampleRate))
			results = append(results, result)
		}
	}

	if !reflect.DeepEqual(results, expectedResults) {
		t.Fatalf("Detections are not correct")
	}
}

func TestSingleBuiltIn(t *testing.T) {

	porcupine = Porcupine{
		AccessKey:       testAccessKey,
		BuiltInKeywords: []BuiltInKeyword{PORCUPINE}}

	runTestCase(
		t,
		&porcupine,
		"porcupine.wav",
		[]int{0})

	delErr := porcupine.Delete()
	if delErr != nil {
		t.Fatalf("%v", delErr)
	}
}

func TestSingleKeyword(t *testing.T) {
	for _, tt := range singleKeywordTestParameters {
		t.Run(tt.language+" single keyword", func(t *testing.T) {
			porcupine = Porcupine{
				AccessKey:    testAccessKey,
				ModelPath:    getTestModelPath(tt.language),
				KeywordPaths: getTestKeywordPaths(tt.language, tt.keywords)}
			runTestCase(
				t,
				&porcupine,
				tt.testAudioFile,
				tt.expectedResults)
			delErr := porcupine.Delete()
			if delErr != nil {
				t.Fatalf("%v", delErr)
			}
		})
	}
}

func TestMultipleBuiltIns(t *testing.T) {

	porcupine = Porcupine{
		AccessKey: testAccessKey,
		BuiltInKeywords: []BuiltInKeyword{
			ALEXA,
			AMERICANO,
			BLUEBERRY,
			BUMBLEBEE,
			GRAPEFRUIT,
			GRASSHOPPER,
			PICOVOICE,
			PORCUPINE,
			TERMINATOR}}
	runTestCase(
		t,
		&porcupine,
		"multiple_keywords.wav",
		[]int{7, 0, 1, 2, 3, 4, 5, 6, 7, 8})
	delErr := porcupine.Delete()
	if delErr != nil {
		t.Fatalf("%v", delErr)
	}
}

func TestMultipleKeywords(t *testing.T) {
	for _, tt := range multipleKeywordTestParameters {
		t.Run(tt.language+" multiple keywords", func(t *testing.T) {
			porcupine = Porcupine{
				AccessKey:    testAccessKey,
				ModelPath:    getTestModelPath(tt.language),
				KeywordPaths: getTestKeywordPaths(tt.language, tt.keywords)}
			runTestCase(
				t,
				&porcupine,
				fmt.Sprintf("%s.wav", appendLanguage("multiple_keywords", tt.language)),
				tt.expectedResults)
			delErr := porcupine.Delete()
			if delErr != nil {
				t.Fatalf("%v", delErr)
			}
		})
	}
}

func TestWithNonAsciiModelName(t *testing.T) {
	language := "es"
	keywords := []string{"murciélago"}
	porcupine = Porcupine{
		AccessKey:    testAccessKey,
		ModelPath:    getTestModelPath(language),
		KeywordPaths: getTestKeywordPaths(language, keywords)}
	runTestCase(
		t,
		&porcupine,
		"murciélago.wav",
		[]int{0, 0})
	delErr := porcupine.Delete()
	if delErr != nil {
		t.Fatalf("%v", delErr)
	}
}
