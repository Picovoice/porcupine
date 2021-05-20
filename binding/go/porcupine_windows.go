// Copyright 2021 Picovoice Inc.
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

// +build windows

package porcupine

//#include <stdlib.h>
import "C"

import (
	"unsafe"

	"golang.org/x/sys/windows"
)

// private vars
var (
	lib               = windows.NewLazyDLL(libName)
	init_func         = lib.NewProc("pv_porcupine_init")
	process_func      = lib.NewProc("pv_porcupine_process")
	sample_rate_func  = lib.NewProc("pv_sample_rate")
	version_func      = lib.NewProc("pv_porcupine_version")
	frame_length_func = lib.NewProc("pv_porcupine_frame_length")
	delete_func       = lib.NewProc("pv_porcupine_delete")
)

func (np nativePorcupineType) nativeInit(porcupine *Porcupine) (status PvStatus) {
	var (
		modelPathC  = C.CString(porcupine.ModelPath)
		numKeywords = len(porcupine.KeywordPaths)
		keywordsC   = make([]*C.char, numKeywords)
	)
	defer C.free(unsafe.Pointer(modelPathC))

	for i, s := range porcupine.KeywordPaths {
		keywordsC[i] = C.CString(s)
		defer C.free(unsafe.Pointer(keywordsC[i]))
	}

	ret, _, _ := init_func.Call(
		uintptr(unsafe.Pointer(modelPathC)),
		uintptr(numKeywords),
		uintptr(unsafe.Pointer(&keywordsC[0])),
		uintptr(unsafe.Pointer(&porcupine.Sensitivities[0])),
		uintptr(unsafe.Pointer(&porcupine.handle)))

	return PvStatus(ret)
}

func (np nativePorcupineType) nativeDelete(porcupine *Porcupine) {
	delete_func.Call(porcupine.handle)
}

func (np nativePorcupineType) nativeProcess(porcupine *Porcupine, pcm []int16) (status PvStatus, keywordIndex int) {

	var index int32
	ret, _, _ := process_func.Call(
		porcupine.handle,
		uintptr(unsafe.Pointer(&pcm[0])),
		uintptr(unsafe.Pointer(&index)))
	return PvStatus(ret), int(index)
}

func (np nativePorcupineType) nativeSampleRate() (sampleRate int) {
	ret, _, _ := sample_rate_func.Call()
	return int(ret)
}

func (np nativePorcupineType) nativeFrameLength() (frameLength int) {
	ret, _, _ := frame_length_func.Call()
	return int(ret)
}

func (np nativePorcupineType) nativeVersion() (version string) {
	ret, _, _ := version_func.Call()
	return C.GoString((*C.char)(unsafe.Pointer(ret)))
}
