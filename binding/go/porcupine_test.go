// Copyright 2021-2023 Picovoice Inc.
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
	"encoding/json"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"math"
	"os"
	"path/filepath"
	"reflect"
	"strings"
	"testing"
)

var (
	testAccessKey                 string
	porcupine                     Porcupine
	singleKeywordTestParameters   []SingleKeywordTestData
	multipleKeywordTestParameters []MultipleKeywordTestData
)

type SingleKeywordTestData struct {
	language      string
	keyword       string
	testAudioFile string
}

type MultipleKeywordTestData struct {
	language      string
	keywords      []string
	testAudioFile string
	groundTruth   []int
}

func TestMain(m *testing.M) {

	flag.StringVar(&testAccessKey, "access_key", "", "AccessKey for testing")
	flag.Parse()

	singleKeywordTestParameters, multipleKeywordTestParameters = loadTestData()
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

func loadTestData() ([]SingleKeywordTestData, []MultipleKeywordTestData) {

	content, err := ioutil.ReadFile("../../resources/.test/test_data.json")
	if err != nil {
		log.Fatalf("Could not read test data json: %v", err)
	}

	var testData struct {
		Tests struct {
			SingleKeyword []struct {
				Language string `json:"language"`
				Keyword  string `json:"wakeword"`
			} `json:"singleKeyword"`
			MultipleKeyword []struct {
				Language    string   `json:"language"`
				Keywords    []string `json:"wakewords"`
				GroundTruth []int    `json:"groundTruth"`
			} `json:"multipleKeyword"`
		} `json:"tests"`
	}
	err = json.Unmarshal(content, &testData)
	if err != nil {
		log.Fatalf("Could not decode test data json: %v", err)
	}

	for _, x := range testData.Tests.SingleKeyword {
		singleKeywordTestData := SingleKeywordTestData{
			language:      x.Language,
			keyword:       x.Keyword,
			testAudioFile: strings.Replace(x.Keyword, " ", "_", -1) + ".wav",
		}

		singleKeywordTestParameters = append(singleKeywordTestParameters, singleKeywordTestData)
	}

	for _, x := range testData.Tests.MultipleKeyword {
		multipleKeywordTestData := MultipleKeywordTestData{
			language:      x.Language,
			keywords:      x.Keywords,
			testAudioFile: appendLanguage("multiple_keywords", x.Language) + ".wav",
			groundTruth:   x.GroundTruth,
		}

		multipleKeywordTestParameters = append(multipleKeywordTestParameters, multipleKeywordTestData)
	}

	return singleKeywordTestParameters, multipleKeywordTestParameters
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
				KeywordPaths: getTestKeywordPaths(tt.language, []string{tt.keyword})}
			runTestCase(
				t,
				&porcupine,
				tt.testAudioFile,
				[]int{0})
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
				tt.testAudioFile,
				tt.groundTruth)
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
