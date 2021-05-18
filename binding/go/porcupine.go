package porcupine

import (
	"C"
	"embed"
	"fmt"
	"log"
	"runtime"
	"syscall"
	"unsafe"
)
import (
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
)

//go:embed embedded
var embeddedFS embed.FS

type PvStatus int

const (
	SUCCESS          PvStatus = 0
	OUT_OF_MEMORY    PvStatus = 1
	IO_ERROR         PvStatus = 2
	INVALID_ARGUMENT PvStatus = 3
)

type BuiltInKeyword string

const (
	ALEXA       BuiltInKeyword = "Alexa"
	AMERICANO   BuiltInKeyword = "Americano"
	BLUEBERRY   BuiltInKeyword = "Blueberry"
	BUMBLEBEE   BuiltInKeyword = "Bumblebee"
	COMPUTER    BuiltInKeyword = "Computer"
	GRAPEFRUIT  BuiltInKeyword = "Grapefruit"
	GRASSHOPPER BuiltInKeyword = "Grasshopper"
	HEY_BARISTA BuiltInKeyword = "Hey Barista"
	HEY_GOOGLE  BuiltInKeyword = "Hey Google"
	HEY_SIRI    BuiltInKeyword = "Hey Siri"
	JARVIS      BuiltInKeyword = "Jarvis"
	OK_GOOGLE   BuiltInKeyword = "OK Google"
	PICO_CLOCK  BuiltInKeyword = "Pico Clock"
	PICOVOICE   BuiltInKeyword = "Picovoice"
	PORCUPINE   BuiltInKeyword = "Porcupine"
	TERMINATOR  BuiltInKeyword = "Terminator"
)

type Porcupine struct {
	handle          uintptr
	ModelPath       string
	Sensitivities   []float32
	BuiltInKeywords []BuiltInKeyword
	KeywordPaths    []string
}

var (
	osName        = getOS()
	extractionDir = filepath.Join(os.TempDir(), "porcupine")

	defaultModelFile = extractDefaultModel()
	builtinKeywords  = extractKeywordFiles()
	libName          = extractLib()

	lib, _               = syscall.LoadLibrary(libName)
	init_func, _         = syscall.GetProcAddress(lib, "pv_porcupine_init")
	process_func, _      = syscall.GetProcAddress(lib, "pv_porcupine_process")
	sample_rate_func, _  = syscall.GetProcAddress(lib, "pv_sample_rate")
	version_func, _      = syscall.GetProcAddress(lib, "pv_porcupine_version")
	frame_length_func, _ = syscall.GetProcAddress(lib, "pv_porcupine_frame_length")
	delete_func, _       = syscall.GetProcAddress(lib, "pv_porcupine_delete")

	FrameLength = getFrameLength()
	SampleRate  = getSampleRate()
	Version     = getVersion()
)

func (porcupine *Porcupine) Init() (status PvStatus, err error) {
	if porcupine.ModelPath == "" {
		porcupine.ModelPath = defaultModelFile
	}

	if _, err := os.Stat(porcupine.ModelPath); os.IsNotExist(err) {
		return INVALID_ARGUMENT, fmt.Errorf("Specified model file could not be found at %s", porcupine.ModelPath)
	}

	if porcupine.BuiltInKeywords != nil && len(porcupine.BuiltInKeywords) > 0 {
		for _, keyword := range porcupine.BuiltInKeywords {
			keywordStr := strings.ToLower(string(keyword))
			porcupine.KeywordPaths = append(porcupine.KeywordPaths, builtinKeywords[keywordStr])
		}
	}

	if porcupine.KeywordPaths == nil || len(porcupine.KeywordPaths) == 0 {
		return INVALID_ARGUMENT, fmt.Errorf("No valid keywords were provided.")
	}

	if porcupine.Sensitivities == nil {
		porcupine.Sensitivities = make([]float32, len(porcupine.KeywordPaths))
		for i := range porcupine.KeywordPaths {
			porcupine.Sensitivities[i] = 0.5
		}
	}

	if len(porcupine.KeywordPaths) != len(porcupine.Sensitivities) {
		return INVALID_ARGUMENT, fmt.Errorf("Keyword array is not the same size as sensitivites array")
	}

	// args for lib call
	var (
		modelPathC  = C.CString(porcupine.ModelPath)
		numKeywords = len(porcupine.KeywordPaths)
		keywordsC   = make([]*C.char, numKeywords)
	)

	// convert keywords to c strings
	for i, s := range porcupine.KeywordPaths {
		keywordsC[i] = C.CString(s)
	}

	// call lib init
	ret, _, e := syscall.Syscall6(init_func, 5,
		uintptr(unsafe.Pointer(modelPathC)),
		uintptr(numKeywords),
		uintptr(unsafe.Pointer(&keywordsC[0])),
		uintptr(unsafe.Pointer(&porcupine.Sensitivities[0])),
		uintptr(unsafe.Pointer(&porcupine.handle)),
		0)
	if e != 0 {
		return IO_ERROR, fmt.Errorf("%v\n", e)
	}

	return PvStatus(ret), nil
}

func (porcupine *Porcupine) Delete() error {
	if porcupine.handle == 0 {
		return fmt.Errorf("Porcupine has not been initialized or has already been deleted.")
	}

	_, _, e := syscall.Syscall(delete_func, 1,
		porcupine.handle,
		0, 0)

	if e != 0 {
		return fmt.Errorf("%v\n", e)
	}

	return nil
}

// pv_porcupine_process
func (porcupine *Porcupine) Process(pcm []int16) (int, error) {

	if porcupine.handle == 0 {
		return -1, fmt.Errorf("Porcupine has not been initialized or has been deleted.")
	}

	if len(pcm) != FrameLength {
		return -1, fmt.Errorf("Input data frame is wrong size")
	}

	var index int32
	ret, _, e := syscall.Syscall6(process_func, 3,
		porcupine.handle,
		uintptr(unsafe.Pointer(&pcm[0])),
		uintptr(unsafe.Pointer(&index)),
		0, 0, 0)
	if e != 0 {
		return -1, fmt.Errorf("%v\n", e)
	}

	if PvStatus(ret) != SUCCESS {
		return -1, fmt.Errorf("Process audio frame failed with PvStatus: %d", ret)
	}

	return int(index), nil
}

func getSampleRate() int {
	ret, _, _ := syscall.Syscall(sample_rate_func, 0, 0, 0, 0)
	return int(ret)
}

func getFrameLength() int {
	ret, _, _ := syscall.Syscall(frame_length_func, 0, 0, 0, 0)
	return int(ret)
}

func getVersion() string {
	ret, _, _ := syscall.Syscall(version_func, 0, 0, 0, 0)
	return C.GoString((*C.char)(unsafe.Pointer(ret)))
}

func getOS() string {
	switch os := runtime.GOOS; os {
	case "darwin":
		return "mac"
	case "linux":
		return "linux"
	case "windows":
		return "windows"
	default:
		log.Fatalf("%s is not a supported OS", os)
		return ""
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
		libPath = fmt.Sprintf("embedded/lib/%s/x86_64/libpv_porcupine.dylib", osName)
	case "linux":
		libPath = fmt.Sprintf("embedded/lib/%s/x86_64/libpv_porcupine.so", osName)
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

	extractedFilepath := filepath.Join(dstDir, srcFile)
	os.MkdirAll(filepath.Dir(extractedFilepath), 0777)
	writeErr := ioutil.WriteFile(extractedFilepath, bytes, 0777)
	if writeErr != nil {
		log.Fatalf("%v", writeErr)
	}
	return extractedFilepath
}
