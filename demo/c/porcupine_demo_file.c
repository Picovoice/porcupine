/*
    Copyright 2018-2023 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

#define UTF8_COMPOSITION_FLAG (0)
#define NULL_TERMINATED       (-1)

#else

#include <dlfcn.h>

#endif

#define DR_WAV_IMPLEMENTATION

#include "dr_wav.h"

#include "pv_porcupine.h"

static void *open_dl(const char *dl_path) {

#if defined(_WIN32) || defined(_WIN64)

    return LoadLibrary(dl_path);

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

static void close_dl(void *handle) {

#if defined(_WIN32) || defined(_WIN64)

    FreeLibrary((HMODULE) handle);

#else

    dlclose(handle);

#endif
}

static void print_dl_error(const char *message) {

#if defined(_WIN32) || defined(_WIN64)

    fprintf(stderr, "%s with code '%lu'.\n", message, GetLastError());

#else

    fprintf(stderr, "%s with '%s'.\n", message, dlerror());

#endif
}

static struct option long_options[] = {
        {"library_path", required_argument, NULL, 'l'},
        {"model_path",   required_argument, NULL, 'm'},
        {"keyword_path", required_argument, NULL, 'k'},
        {"sensitivity",  required_argument, NULL, 't'},
        {"access_key",   required_argument, NULL, 'a'},
        {"wav_path",     required_argument, NULL, 'w'},
};

void print_usage(const char *program_name) {
    fprintf(stderr, "Usage : %s -l LIBRARY_PATH -m MODEL_PATH -k KEYWORD_PATH -t SENSITIVITY -a ACCESS_KEY -w WAV_PATH\n", program_name);
}

int picovoice_main(int argc, char *argv[]) {
    const char *library_path = NULL;
    const char *model_path = NULL;
    const char *keyword_path = NULL;
    float sensitivity = 0.5f;
    const char *access_key = NULL;
    const char *wav_path = NULL;

    int c;
    while ((c = getopt_long(argc, argv, "l:m:k:t:a:w:", long_options, NULL)) != -1) {
        switch (c) {
            case 'l':
                library_path = optarg;
                break;
            case 'm':
                model_path = optarg;
                break;
            case 'k':
                keyword_path = optarg;
                break;
            case 't':
                sensitivity = strtof(optarg, NULL);
                break;
            case 'a':
                access_key = optarg;
                break;
            case 'w':
                wav_path = optarg;
                break;
            default:
                exit(1);
        }
    }

    if (!library_path || !model_path || !keyword_path || !access_key) {
        print_usage(argv[0]);
        exit(1);
    }

    void *porcupine_library = open_dl(library_path);
    if (!porcupine_library) {
        fprintf(stderr, "failed to open library\n");
        exit(1);
    }

    const char *(*pv_status_to_string_func)(pv_status_t) = load_symbol(porcupine_library, "pv_status_to_string");
    if (!pv_status_to_string_func) {
        print_dl_error("failed to load 'pv_status_to_string' with '%s'");
        exit(1);
    }

    int32_t (*pv_sample_rate_func)() = load_symbol(porcupine_library, "pv_sample_rate");
    if (!pv_sample_rate_func) {
        print_dl_error("failed to load 'pv_sample_rate'");
        exit(1);
    }

    pv_status_t (*pv_porcupine_init_func)(const char *, const char *, int32_t, const char *const *, const float *, pv_porcupine_t **) =
            load_symbol(porcupine_library, "pv_porcupine_init");
    if (!pv_porcupine_init_func) {
        print_dl_error("failed to load 'pv_porcupine_init'");
        exit(1);
    }

    void (*pv_porcupine_delete_func)(pv_porcupine_t *) = load_symbol(porcupine_library, "pv_porcupine_delete");
    if (!pv_porcupine_delete_func) {
        print_dl_error("failed to load 'pv_porcupine_delete'");
        exit(1);
    }

    pv_status_t (*pv_porcupine_process_func)(pv_porcupine_t *, const int16_t *, int32_t *) = load_symbol(porcupine_library, "pv_porcupine_process");
    if (!pv_porcupine_process_func) {
        print_dl_error("failed to load 'pv_porcupine_process'");
        exit(1);
    }

    int32_t (*pv_porcupine_frame_length_func)() = load_symbol(porcupine_library, "pv_porcupine_frame_length");
    if (!pv_porcupine_frame_length_func) {
        print_dl_error("failed to load 'pv_porcupine_frame_length' with '%s'.\n");
        exit(1);
    }

    const char *(*pv_porcupine_version_func)() = load_symbol(porcupine_library, "pv_porcupine_version");
    if (!pv_porcupine_version_func) {
        print_dl_error("failed to load 'pv_porcupine_version'");
        exit(1);
    }

    drwav f;

#if defined(_WIN32) || defined(_WIN64)

    int wav_path_wchars_num = MultiByteToWideChar(CP_UTF8, UTF8_COMPOSITION_FLAG, wav_path, NULL_TERMINATED, NULL, 0);
    wchar_t wav_path_w[wav_path_wchars_num];
    MultiByteToWideChar(CP_UTF8, UTF8_COMPOSITION_FLAG, wav_path, NULL_TERMINATED, wav_path_w, wav_path_wchars_num);
    const int drwav_init_file_status = drwav_init_file_w(&f, wav_path_w, NULL);

#else

    const int drwav_init_file_status = drwav_init_file(&f, wav_path, NULL);

#endif

    if (!drwav_init_file_status) {
        fprintf(stderr, "failed to open wav file at '%s'.", wav_path);
        exit(1);
    }

    if (f.sampleRate != (uint32_t) pv_sample_rate_func()) {
        fprintf(stderr, "audio sample rate should be %d\n.", pv_sample_rate_func());
        exit(1);
    }

    if (f.bitsPerSample != 16) {
        fprintf(stderr, "audio format should be 16-bit\n.");
        exit(1);
    }

    if (f.channels != 1) {
        fprintf(stderr, "audio should be single-channel.\n");
        exit(1);
    }

    int16_t *pcm = calloc(pv_porcupine_frame_length_func(), sizeof(int16_t));
    if (!pcm) {
        fprintf(stderr, "failed to allocate memory for audio frame.\n");
        exit(1);
    }

    pv_porcupine_t *porcupine = NULL;
    pv_status_t status = pv_porcupine_init_func(access_key, model_path, 1, &keyword_path, &sensitivity, &porcupine);
    if (status != PV_STATUS_SUCCESS) {
        fprintf(stderr, "'pv_porcupine_init' failed with '%s'\n", pv_status_to_string_func(status));
        exit(1);
    }

    fprintf(stdout, "V%s\n\n", pv_porcupine_version_func());

    double total_cpu_time_usec = 0;
    double total_processed_time_usec = 0;
    int32_t frame_index = 0;

    while ((int32_t) drwav_read_pcm_frames_s16(&f, pv_porcupine_frame_length_func(), pcm) == pv_porcupine_frame_length_func()) {
        struct timeval before;
        gettimeofday(&before, NULL);

        int32_t keyword_index = -1;
        status = pv_porcupine_process_func(porcupine, pcm, &keyword_index);
        if (status != PV_STATUS_SUCCESS) {
            fprintf(stderr, "'pv_porcupine_process' failed with '%s'\n", pv_status_to_string_func(status));
            exit(1);
        }

        struct timeval after;
        gettimeofday(&after, NULL);

        total_cpu_time_usec += (double) (after.tv_sec - before.tv_sec) * 1e6 + (double) (after.tv_usec - before.tv_usec);
        total_processed_time_usec += (pv_porcupine_frame_length_func() * 1e6) / pv_sample_rate_func();

        if (keyword_index != -1) {
            fprintf(stdout, "detected at %.1f seconds\n", (double) frame_index * pv_porcupine_frame_length_func() / pv_sample_rate_func());
        }
        frame_index++;
    }

    const double real_time_factor = total_cpu_time_usec / total_processed_time_usec;
    fprintf(stdout, "real time factor : %.3f\n", real_time_factor);

    free(pcm);
    drwav_uninit(&f);
    pv_porcupine_delete_func(porcupine);
    close_dl(porcupine_library);

    return 0;
}

int main(int argc, char *argv[]) {

#if defined(_WIN32) || defined(_WIN64)

    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (wargv == NULL) {
        fprintf(stderr, "CommandLineToArgvW failed\n");
        exit(1);
    }

    char *utf8_argv[argc];

    for (int i = 0; i < argc; ++i) {
        // WideCharToMultiByte: https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
        int arg_chars_num = WideCharToMultiByte(CP_UTF8, UTF8_COMPOSITION_FLAG, wargv[i], NULL_TERMINATED, NULL, 0, NULL, NULL);
        utf8_argv[i] = (char *) malloc(arg_chars_num * sizeof(char));
        if (!utf8_argv[i]) {
            fprintf(stderr, "failed to to allocate memory for converting args");
        }
        WideCharToMultiByte(CP_UTF8, UTF8_COMPOSITION_FLAG, wargv[i], NULL_TERMINATED, utf8_argv[i], arg_chars_num, NULL, NULL);
    }

    LocalFree(wargv);
    argv = utf8_argv;

#endif

    int result = picovoice_main(argc, argv);

#if defined(_WIN32) || defined(_WIN64)

    for (int i = 0; i < argc; ++i) {
        free(utf8_argv[i]);
    }

#endif

    return result;
}
