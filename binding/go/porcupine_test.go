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
	"flag"
	"fmt"
	"io/ioutil"
	"math"
	"os"
	"path/filepath"
	"testing"
	"reflect"
)

var (
	pvTestAccessKey string
	p               Porcupine	
)

func TestMain(m *testing.M) {

	flag.StringVar(&pvTestAccessKey, "access_key", "", "AccessKey for testing")
	flag.Parse()

	os.Exit(m.Run())
}

func appendLanguage(s string, language string) string {
	if language == "en" {
		return s;
	}
	return s + "_" + language
}

func getTestModelPath(language string) string {
	modelPath, _ := filepath.Abs(fmt.Sprintf(
		"../../lib/common/%s.pv",
		appendLanguage("porcupine_params", language)))
	return modelPath
}

func getTestKeywordPath(language string, keyword string) string {
	keywordPath, _ := filepath.Abs(fmt.Sprintf(
		"../../resources/%s/%s/%s_%s.ppn",
		appendLanguage("keyword_files", language),
		osName,
		keyword,
		osName))
	return keywordPath
}

func getTestKeywordPaths(language string, keywords []string) []string {
	keywordPaths := []string{}
	for _, keyword := range keywords {
		keywordPaths = append(keywordPaths, getTestKeywordPath(language, keyword))
	}
	return keywordPaths
}

func runTestCase(t *testing.T, audioFileName string, expectedResults []int) {
	err := p.Init()
	if err != nil {
		t.Fatalf("%v", err)
	}
	
	t.Logf("Porcupine Version: %s", Version)
	t.Logf("Frame Length: %d", FrameLength)
	t.Logf("Sample Rate: %d", SampleRate)

	testFile, _ := filepath.Abs(filepath.Join("../../resources/audio_samples", audioFileName))
	data, err := ioutil.ReadFile(testFile)
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

	if !reflect.DeepEqual(results, expectedResults) {
		t.Fatalf("Detections are not correct")
	}

	delErr := p.Delete()
	if delErr != nil {
		t.Fatalf("%v", delErr)
	}
}

func TestSingleKeyword(t *testing.T) {

	p = Porcupine{
		AccessKey:       pvTestAccessKey,
		BuiltInKeywords: []BuiltInKeyword{PORCUPINE}}
	runTestCase(
		t,
		"porcupine.wav",
		[]int{0})
}

func TestMultipleKeywords(t *testing.T) {

	p = Porcupine{
		AccessKey: pvTestAccessKey,
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
		"multiple_keywords.wav",
		[]int{7, 0, 1, 2, 3, 4, 5, 6, 7, 8})
}

func TestSingleKeywordDe(t *testing.T) {

	language := "de"
	keywords := []string{"heuschrecke"}
	p = Porcupine{
		AccessKey: pvTestAccessKey,
		ModelPath: getTestModelPath(language),
		KeywordPaths: getTestKeywordPaths(language, keywords)}
	runTestCase(
		t,
		"heuschrecke.wav",
		[]int{0})
}

func TestMultipleKeywordsDe(t *testing.T) {

	language := "de"
	keywords := []string{"ananas", "heuschrecke", "leguan", "stachelschwein"}

	p = Porcupine{
		AccessKey: pvTestAccessKey,
		ModelPath: getTestModelPath(language),
		KeywordPaths: getTestKeywordPaths(language, keywords)}
	runTestCase(
		t,
		"multiple_keywords_de.wav",
		[]int{0, 1, 2, 3})
}

func TestSingleKeywordEs(t *testing.T) {

	language := "es"
	keywords := []string{"manzana"}
	p = Porcupine{
		AccessKey: pvTestAccessKey,
		ModelPath: getTestModelPath(language),
		KeywordPaths: getTestKeywordPaths(language, keywords)}
	runTestCase(
		t,
		"manzana.wav",
		[]int{0})
}

func TestMultipleKeywordsEs(t *testing.T) {

	language := "es"
	keywords := []string{"emparedado", "leopardo", "manzana"}
	p = Porcupine{
		AccessKey: pvTestAccessKey,
		ModelPath: getTestModelPath(language),
		KeywordPaths: getTestKeywordPaths(language, keywords)}
	runTestCase(
		t,
		"multiple_keywords_es.wav",
		[]int{0, 1, 2})
}

func TestSingleKeywordFr(t *testing.T) {

	language := "fr"
	keywords := []string{"mon chouchou"}
	p = Porcupine{
		AccessKey: pvTestAccessKey,
		ModelPath: getTestModelPath(language),
		KeywordPaths: getTestKeywordPaths(language, keywords)}
	runTestCase(
		t,
		"mon_chouchou.wav",
		[]int{0})
}

func TestMultipleKeywordsFr(t *testing.T) {

	language := "fr"
	keywords := []string{"framboise", "mon chouchou", "parapluie"}
	p = Porcupine{
		AccessKey: pvTestAccessKey,
		ModelPath: getTestModelPath(language),
		KeywordPaths: getTestKeywordPaths(language, keywords)}
	runTestCase(
		t,
		"multiple_keywords_fr.wav",
		[]int{0, 1, 0, 2})
}