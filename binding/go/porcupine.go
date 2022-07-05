// Copyright 2021-2022 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is
// located in the "LICENSE" file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the
// License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied. See the License for the specific language governing permissions and
// limitations under the License.
//

// Go binding for Porcupine wake word engine. It detects utterances of given keywords within an incoming stream of
// audio in real-time. It processes incoming audio in consecutive frames and for each frame emits the detection result.
// The number of samples per frame can be attained by calling `.FrameLength`. The incoming audio needs to have a
// sample rate equal to `.SampleRate` and be 16-bit linearly-encoded. Porcupine operates on single-channel audio.

package porcupine

import (
	"C"
	"crypto/sha256"
	"embed"
	"encoding/hex"
	"errors"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
)
import "unsafe"

//go:embed embedded
var embeddedFS embed.FS

// PvStatus type
type PvStatus int

// Possible status return codes from the Porcupine library
const (
	SUCCESS                  PvStatus = 0
	OUT_OF_MEMORY            PvStatus = 1
	IO_ERROR                 PvStatus = 2
	INVALID_ARGUMENT         PvStatus = 3
	STOP_ITERATION           PvStatus = 4
	KEY_ERROR                PvStatus = 5
	INVALID_STATE            PvStatus = 6
	RUNTIME_ERROR            PvStatus = 7
	ACTIVATION_ERROR         PvStatus = 8
	ACTIVATION_LIMIT_REACHED PvStatus = 9
	ACTIVATION_THROTTLED     PvStatus = 10
	ACTIVATION_REFUSED       PvStatus = 11
)

type PorcupineError struct {
	StatusCode PvStatus
	Message    string
}

func (e *PorcupineError) Error() string {
	return fmt.Sprintf("%s: %s", pvStatusToString(e.StatusCode), e.Message)
}

func pvStatusToString(status PvStatus) string {
	switch status {
	case SUCCESS:
		return "SUCCESS"
	case OUT_OF_MEMORY:
		return "OUT_OF_MEMORY"
	case IO_ERROR:
		return "IO_ERROR"
	case INVALID_ARGUMENT:
		return "INVALID_ARGUMENT"
	case STOP_ITERATION:
		return "STOP_ITERATION"
	case KEY_ERROR:
		return "KEY_ERROR"
	case INVALID_STATE:
		return "INVALID_STATE"
	case RUNTIME_ERROR:
		return "RUNTIME_ERROR"
	case ACTIVATION_ERROR:
		return "ACTIVATION_ERROR"
	case ACTIVATION_LIMIT_REACHED:
		return "ACTIVATION_LIMIT_REACHED"
	case ACTIVATION_THROTTLED:
		return "ACTIVATION_THROTTLED"
	case ACTIVATION_REFUSED:
		return "ACTIVATION_REFUSED"
	default:
		return fmt.Sprintf("Unknown error code: %d", status)
	}
}

// BuiltInKeyword Type
type BuiltInKeyword string

// Available built-in wake words constants
const (
	ALEXA       BuiltInKeyword = "alexa"
	AMERICANO   BuiltInKeyword = "americano"
	BLUEBERRY   BuiltInKeyword = "blueberry"
	BUMBLEBEE   BuiltInKeyword = "bumblebee"
	COMPUTER    BuiltInKeyword = "computer"
	GRAPEFRUIT  BuiltInKeyword = "grapefruit"
	GRASSHOPPER BuiltInKeyword = "grasshopper"
	HEY_GOOGLE  BuiltInKeyword = "hey google"
	HEY_SIRI    BuiltInKeyword = "hey siri"
	JARVIS      BuiltInKeyword = "jarvis"
	OK_GOOGLE   BuiltInKeyword = "ok google"
	PICOVOICE   BuiltInKeyword = "picovoice"
	PORCUPINE   BuiltInKeyword = "porcupine"
	TERMINATOR  BuiltInKeyword = "terminator"
)

// List of available built-in wake words
var BuiltInKeywords = []BuiltInKeyword{
	ALEXA, AMERICANO, BLUEBERRY, BUMBLEBEE, COMPUTER, GRAPEFRUIT, GRASSHOPPER,
	HEY_GOOGLE, HEY_SIRI, JARVIS, OK_GOOGLE, PICOVOICE, PORCUPINE, TERMINATOR,
}

// Checks if a given BuiltInKeyword is valid
func (k BuiltInKeyword) IsValid() bool {
	for _, b := range BuiltInKeywords {
		if k == b {
			return true
		}
	}
	return false
}

// Porcupine struct
type Porcupine struct {
	// handle for porcupine instance in C
	handle unsafe.Pointer

	// AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
	AccessKey string

	// Absolute path to Porcupine's dynamic library.
	LibraryPath string

	// Absolute path to the file containing model parameters.
	ModelPath string

	// Sensitivity values for detecting keywords. Each value should be a number within [0, 1]. A
	// higher sensitivity results in fewer misses at the cost of increasing the false alarm rate.
	Sensitivities []float32

	// List of built-in keywords to use.
	BuiltInKeywords []BuiltInKeyword

	// Absolute paths to keyword model files.
	KeywordPaths []string
}

// private vars
var (
	osName, cpu   = getOS()
	extractionDir = filepath.Join(os.TempDir(), "porcupine")

	defaultModelFile = extractDefaultModel()
	builtinKeywords  = extractKeywordFiles()
	defaultLibPath   = extractLib()
	nativePorcupine  = nativePorcupineType{}
)

var (
	// Number of audio samples per frame.
	FrameLength int

	// Audio sample rate accepted by Picovoice.
	SampleRate int

	// Porcupine version
	Version string
)

// Init function for Porcupine. Must be called before attempting process
func (porcupine *Porcupine) Init() error {
	if porcupine.AccessKey == "" {
		return &PorcupineError{
			INVALID_ARGUMENT,
			"No AccessKey provided to Porcupine"}
	}

	if porcupine.LibraryPath == "" {
		porcupine.LibraryPath = defaultLibPath
	}

	if _, err := os.Stat(porcupine.LibraryPath); os.IsNotExist(err) {
		return &PorcupineError{
			INVALID_ARGUMENT,
			fmt.Sprintf("Specified library file could not be found at %s", porcupine.LibraryPath)}
	}

	if porcupine.ModelPath == "" {
		porcupine.ModelPath = defaultModelFile
	}

	if _, err := os.Stat(porcupine.ModelPath); os.IsNotExist(err) {
		return &PorcupineError{
			INVALID_ARGUMENT,
			fmt.Sprintf("Specified model file could not be found at %s", porcupine.ModelPath)}
	}

	if porcupine.BuiltInKeywords != nil && len(porcupine.BuiltInKeywords) > 0 {
		for _, keyword := range porcupine.BuiltInKeywords {
			if !keyword.IsValid() {
				return &PorcupineError{
					INVALID_ARGUMENT,
					fmt.Sprintf("'%s' is not a valid built-in keyword", keyword)}
			}
			keywordStr := string(keyword)
			porcupine.KeywordPaths = append(porcupine.KeywordPaths, builtinKeywords[keywordStr])
		}
	}

	if porcupine.KeywordPaths == nil || len(porcupine.KeywordPaths) == 0 {
		return &PorcupineError{
			INVALID_ARGUMENT,
			"No valid keywords were provided"}
	}

	for _, k := range porcupine.KeywordPaths {
		if _, err := os.Stat(k); os.IsNotExist(err) {
			return &PorcupineError{
				INVALID_ARGUMENT,
				fmt.Sprintf("Keyword file could not be found at %s", k)}
		}
	}

	if porcupine.Sensitivities == nil {
		porcupine.Sensitivities = make([]float32, len(porcupine.KeywordPaths))
		for i := range porcupine.KeywordPaths {
			porcupine.Sensitivities[i] = 0.5
		}
	} else {
		for _, s := range porcupine.Sensitivities {
			if s < 0 || s > 1 {
				return &PorcupineError{
					INVALID_ARGUMENT,
					fmt.Sprintf("Sensitivity value of %f is invalid. Must be between [0, 1]", s)}
			}
		}
	}

	if len(porcupine.KeywordPaths) != len(porcupine.Sensitivities) {
		return &PorcupineError{
			INVALID_ARGUMENT,
			fmt.Sprintf("Number of keywords (%d) is not the same as number of sensitivities (%d)",
				len(porcupine.KeywordPaths),
				len(porcupine.Sensitivities))}
	}

	ret := nativePorcupine.nativeInit(porcupine)
	if PvStatus(ret) != SUCCESS {
		return &PorcupineError{
			PvStatus(ret),
			"Porcupine init failed."}
	}

	FrameLength = nativePorcupine.nativeFrameLength()
	SampleRate = nativePorcupine.nativeSampleRate()
	Version = nativePorcupine.nativeVersion()

	return nil
}

// Releases resources acquired by Porcupine.
func (porcupine *Porcupine) Delete() error {
	if porcupine.handle == nil {
		return &PorcupineError{
			INVALID_STATE,
			"Porcupine has not been initialized or has already been deleted."}
	}

	nativePorcupine.nativeDelete(porcupine)
	return nil
}

// Processes a frame of the incoming audio stream and emits the detection result.
// Frame of audio The number of samples per frame can be attained by calling
// `.FrameLength`. The incoming audio needs to have a sample rate equal to `.Sample` and be 16-bit
// linearly-encoded. Porcupine operates on single-channel audio.
// Returns a 0 based index if keyword was detected in frame. Returns -1 if no detection was made.
func (porcupine *Porcupine) Process(pcm []int16) (keywordIndex int, err error) {

	if porcupine.handle == nil {
		return -1, &PorcupineError{
			INVALID_STATE,
			"Porcupine has not been initialized or has already been deleted."}
	}

	if len(pcm) != FrameLength {
		return -1, &PorcupineError{
			INVALID_ARGUMENT,
			fmt.Sprintf("Input data frame size (%d) does not match required size of %d", len(pcm), FrameLength)}
	}

	// call process
	ret, index := nativePorcupine.nativeProcess(porcupine, pcm)
	if PvStatus(ret) != SUCCESS {
		return -1, &PorcupineError{
			PvStatus(ret),
			"Porcupine process failed."}
	}

	return index, nil
}

func getOS() (string, string) {
	switch os := runtime.GOOS; os {
	case "darwin":
		return "mac", getMacArch()
	case "linux":
		osName, cpu := getLinuxDetails()
		return osName, cpu
	case "windows":
		return "windows", "amd64"
	default:
		log.Fatalf("%s is not a supported OS", os)
		return "", ""
	}
}

func getMacArch() string {
	if runtime.GOARCH == "arm64" {
		return "arm64"
	} else {
		return "x86_64"
	}
}

func getLinuxDetails() (string, string) {
	var archInfo = ""

	if runtime.GOARCH == "amd64" {
		return "linux", "x86_64"
	} else if runtime.GOARCH == "arm64" {
		archInfo = "-aarch64"
	}

	cmd := exec.Command("cat", "/proc/cpuinfo")
	cpuInfo, err := cmd.Output()

	if err != nil {
		log.Fatalf("Failed to get CPU details: %s", err.Error())
	}

	var cpuPart = ""
	for _, line := range strings.Split(string(cpuInfo), "\n") {
		if strings.Contains(line, "CPU part") {
			split := strings.Split(line, " ")
			cpuPart = strings.ToLower(split[len(split)-1])
			break
		}
	}

	switch cpuPart {
	case "0xb76":
		return "raspberry-pi", "arm11" + archInfo
	case "0xc07":
		return "raspberry-pi", "cortex-a7" + archInfo
	case "0xd03":
		return "raspberry-pi", "cortex-a53" + archInfo
	case "0xd07":
		return "jetson", "cortex-a57" + archInfo
	case "0xd08":
		return "raspberry-pi", "cortex-a72" + archInfo
	case "0xc08":
		return "beaglebone", ""
	default:
		log.Fatalf("Unsupported CPU:\n%s", cpuPart)
		return "", ""
	}
}

func extractDefaultModel() string {
	modelPath := "embedded/lib/common/porcupine_params.pv"
	return extractFile(modelPath, extractionDir)
}

func extractKeywordFiles() map[string]string {
	keywordDirPath := "embedded/resources/keyword_files/" + osName
	keywordFiles, err := embeddedFS.ReadDir(keywordDirPath)
	if err != nil {
		log.Fatalf("%v", err)
	}

	extractedKeywords := make(map[string]string)
	for _, keywordFile := range keywordFiles {
		keywordPath := keywordDirPath + "/" + keywordFile.Name()
		keywordName := strings.Split(keywordFile.Name(), "_")[0]
		extractedKeywords[keywordName] = extractFile(keywordPath, extractionDir)
	}
	return extractedKeywords
}

func extractLib() string {
	var libPath string
	switch os := runtime.GOOS; os {
	case "darwin":
		libPath = fmt.Sprintf("embedded/lib/%s/%s/libpv_porcupine.dylib", osName, cpu)
	case "linux":
		if cpu == "" {
			libPath = fmt.Sprintf("embedded/lib/%s/libpv_porcupine.so", osName)
		} else {
			libPath = fmt.Sprintf("embedded/lib/%s/%s/libpv_porcupine.so", osName, cpu)
		}
	case "windows":
		libPath = fmt.Sprintf("embedded/lib/%s/amd64/libpv_porcupine.dll", osName)
	default:
		log.Fatalf("%s is not a supported OS", os)
	}

	return extractFile(libPath, extractionDir)
}

func extractFile(srcFile string, dstDir string) string {
	bytes, readErr := embeddedFS.ReadFile(srcFile)
	if readErr != nil {
		log.Fatalf("%v", readErr)
	}

	srcHash := sha256sumBytes(bytes)
	hashedDstDir := filepath.Join(dstDir, srcHash)
	extractedFilepath := filepath.Join(hashedDstDir, srcFile)

	if _, err := os.Stat(extractedFilepath); errors.Is(err, os.ErrNotExist) {
		mkErr := os.MkdirAll(filepath.Dir(extractedFilepath), 0764)
		if mkErr != nil {
			log.Fatalf("%v", mkErr)
		}

		writeErr := ioutil.WriteFile(extractedFilepath, bytes, 0764)
		if writeErr != nil {
			log.Fatalf("%v", writeErr)
		}
	}

	return extractedFilepath
}

func sha256sumBytes(bytes []byte) string {
	sum := sha256.Sum256(bytes)
	return hex.EncodeToString(sum[:])
}
