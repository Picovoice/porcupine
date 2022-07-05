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

/*
#cgo linux LDFLAGS: -ldl
#cgo darwin LDFLAGS: -ldl
#include <stdint.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)

	#include <windows.h>

#else

	#include <dlfcn.h>

#endif

static void *open_dl(const char *dl_path) {

#if defined(_WIN32) || defined(_WIN64)

    return LoadLibrary((LPCSTR) dl_path);

#else

    return dlopen(dl_path, RTLD_NOW);

#endif

}

static void *load_symbol(void *handle, const char *symbol) {

#if defined(_WIN32) || defined(_WIN64)

    return GetProcAddress((HMODULE) handle, symbol);

#else

    return dlsym(handle, symbol);

#endif

}

typedef int32_t (*pv_porcupine_sample_rate_func)();

int32_t pv_porcupine_sample_rate_wrapper(void *f) {
     return ((pv_porcupine_sample_rate_func) f)();
}

typedef int32_t (*pv_porcupine_frame_length_func)();

int32_t pv_porcupine_frame_length_wrapper(void* f) {
     return ((pv_porcupine_frame_length_func) f)();
}

typedef char* (*pv_porcupine_version_func)();

char* pv_porcupine_version_wrapper(void* f) {
     return ((pv_porcupine_version_func) f)();
}

typedef int32_t (*pv_porcupine_init_func)(
	const char *access_key,
	const char *model_path,
	int32_t num_keywords,
	const char * const *keyword_paths,
	const float *sensitivities,
	void **object);

int32_t pv_porcupine_init_wrapper(
	void *f,
	const char *access_key,
	const char *model_path,
	int32_t num_keywords,
	const char * const *keyword_paths,
	const float *sensitivities,
	void **object) {
	return ((pv_porcupine_init_func) f)(
		access_key,
		model_path,
		num_keywords,
		keyword_paths,
		sensitivities,
		object);
}

typedef int32_t (*pv_porcupine_process_func)(
	void * object,
	const int16_t *pcm,
	int32_t *keyword_index);

int32_t pv_porcupine_process_wrapper(
	void *f,
	void *object,
	const int16_t *pcm,
	int32_t *keyword_index) {
	return ((pv_porcupine_process_func) f)(
		object,
		pcm,
		keyword_index);
}

typedef void (*pv_porcupine_delete_func)(void *);

void pv_porcupine_delete_wrapper(void *f, void *object) {
	return ((pv_porcupine_delete_func) f)(object);
}

*/
import "C"

import (
	"unsafe"
)

// private vars
type nativePorcupineInterface interface {
	nativeInit(*Porcupine)
	nativeProcess(*Porcupine, []int)
	nativeDelete(*Porcupine)
	nativeSampleRate()
	nativeFrameLength()
	nativeVersion()
}

type nativePorcupineType struct {
	libraryHandle                 unsafe.Pointer
	pv_porcupine_init_ptr         unsafe.Pointer
	pv_porcupine_process_ptr      unsafe.Pointer
	pv_sample_rate_ptr            unsafe.Pointer
	pv_porcupine_version_ptr      unsafe.Pointer
	pv_porcupine_frame_length_ptr unsafe.Pointer
	pv_porcupine_delete_ptr       unsafe.Pointer
}

func (np *nativePorcupineType) nativeInit(porcupine *Porcupine) (status PvStatus) {
	var (
		accessKeyC   = C.CString(porcupine.AccessKey)
		libraryPathC = C.CString(porcupine.LibraryPath)
		modelPathC   = C.CString(porcupine.ModelPath)
		numKeywords  = len(porcupine.KeywordPaths)
		keywordsC    = make([]*C.char, numKeywords)
	)

	defer C.free(unsafe.Pointer(accessKeyC))
	defer C.free(unsafe.Pointer(libraryPathC))
	defer C.free(unsafe.Pointer(modelPathC))

	np.libraryHandle = C.open_dl(libraryPathC)
	np.pv_porcupine_init_ptr = C.load_symbol(np.libraryHandle, C.CString("pv_porcupine_init"))
	np.pv_porcupine_process_ptr = C.load_symbol(np.libraryHandle, C.CString("pv_porcupine_process"))
	np.pv_sample_rate_ptr = C.load_symbol(np.libraryHandle, C.CString("pv_sample_rate"))
	np.pv_porcupine_version_ptr = C.load_symbol(np.libraryHandle, C.CString("pv_porcupine_version"))
	np.pv_porcupine_frame_length_ptr = C.load_symbol(np.libraryHandle, C.CString("pv_porcupine_frame_length"))
	np.pv_porcupine_delete_ptr = C.load_symbol(np.libraryHandle, C.CString("pv_porcupine_delete"))

	for i, s := range porcupine.KeywordPaths {
		keywordsC[i] = C.CString(s)
		defer C.free(unsafe.Pointer(keywordsC[i]))
	}

	var ret = C.pv_porcupine_init_wrapper(
		np.pv_porcupine_init_ptr,
		accessKeyC,
		modelPathC,
		(C.int32_t)(numKeywords),
		(**C.char)(unsafe.Pointer(&keywordsC[0])),
		(*C.float)(unsafe.Pointer(&porcupine.Sensitivities[0])),
		&porcupine.handle)

	return PvStatus(ret)
}

func (np *nativePorcupineType) nativeDelete(porcupine *Porcupine) {
	C.pv_porcupine_delete_wrapper(np.pv_porcupine_delete_ptr,
		porcupine.handle)
}

func (np *nativePorcupineType) nativeProcess(porcupine *Porcupine, pcm []int16) (status PvStatus, keywordIndex int) {

	var index int32
	var ret = C.pv_porcupine_process_wrapper(np.pv_porcupine_process_ptr,
		porcupine.handle,
		(*C.int16_t)(unsafe.Pointer(&pcm[0])),
		(*C.int32_t)(unsafe.Pointer(&index)))
	return PvStatus(ret), int(index)
}

func (np *nativePorcupineType) nativeSampleRate() (sampleRate int) {
	return int(C.pv_porcupine_sample_rate_wrapper(np.pv_sample_rate_ptr))
}

func (np *nativePorcupineType) nativeFrameLength() (frameLength int) {
	return int(C.pv_porcupine_frame_length_wrapper(np.pv_porcupine_frame_length_ptr))
}

func (np *nativePorcupineType) nativeVersion() (version string) {
	return C.GoString(C.pv_porcupine_version_wrapper(np.pv_porcupine_version_ptr))
}
