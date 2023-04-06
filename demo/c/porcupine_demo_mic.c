/*
    Copyright 2018-2023 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <getopt.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

#else

#include <dlfcn.h>

#endif

#include "pv_porcupine.h"
#include "pv_recorder.h"

static volatile bool is_interrupted = false;

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
        {"show_audio_devices", no_argument,       NULL, 's'},
        {"library_path",       required_argument, NULL, 'l'},
        {"model_path",         required_argument, NULL, 'm'},
        {"keyword_path",       required_argument, NULL, 'k'},
        {"sensitivity",        required_argument, NULL, 't'},
        {"access_key",         required_argument, NULL, 'a'},
        {"audio_device_index", required_argument, NULL, 'd'}
};

static void print_usage(const char *program_name) {
    fprintf(stderr,
            "Usage : %s -l LIBRARY_PATH -m MODEL_PATH -k KEYWORD_PATH -t SENSITIVITY -a ACCESS_KEY -d "
            "AUDIO_DEVICE_INDEX\n"
            "        %s [-s, --show_audio_devices]\n",
            program_name,
            program_name);
}

void interrupt_handler(int _) {
    (void) _;
    is_interrupted = true;
}

void show_audio_devices(void) {
    char **devices = NULL;
    int32_t count = 0;

    pv_recorder_status_t status = pv_recorder_get_audio_devices(&count, &devices);
    if (status != PV_RECORDER_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to get audio devices with: %s.\n", pv_recorder_status_to_string(status));
        exit(1);
    }

    fprintf(stdout, "Printing devices...\n");
    for (int32_t i = 0; i < count; i++) {
        fprintf(stdout, "index: %d, name: %s\n", i, devices[i]);
    }

    pv_recorder_free_device_list(count, devices);
}

int picovoice_main(int argc, char *argv[]) {
    signal(SIGINT, interrupt_handler);

    const char *library_path = NULL;
    const char *model_path = NULL;
    const char *keyword_path = NULL;
    float sensitivity = 0.5f;
    const char *access_key = NULL;
    int32_t device_index = -1;

    int c;
    while ((c = getopt_long(argc, argv, "sl:m:k:t:a:d:", long_options, NULL)) != -1) {
        switch (c) {
            case 's':
                show_audio_devices();
                return 0;
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
            case 'd':
                device_index = (int32_t) strtol(optarg, NULL, 10);
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
        fprintf(stderr, "failed to open library.\n");
        exit(1);
    }

    const char *(*pv_status_to_string_func)(pv_status_t) = load_symbol(porcupine_library, "pv_status_to_string");
    if (!pv_status_to_string_func) {
        print_dl_error("failed to load 'pv_status_to_string'");
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
        print_dl_error("failed to load 'pv_porcupine_frame_length'");
        exit(1);
    }

    const char *(*pv_porcupine_version_func)() = load_symbol(porcupine_library, "pv_porcupine_version");
    if (!pv_porcupine_version_func) {
        print_dl_error("failed to load 'pv_porcupine_version'");
        exit(1);
    }

    pv_porcupine_t *porcupine = NULL;
    pv_status_t porcupine_status = pv_porcupine_init_func(access_key, model_path, 1, &keyword_path, &sensitivity, &porcupine);
    if (porcupine_status != PV_STATUS_SUCCESS) {
        fprintf(stderr, "'pv_porcupine_init' failed with '%s'\n", pv_status_to_string_func(porcupine_status));
        exit(1);
    }

    fprintf(stdout, "V%s\n\n", pv_porcupine_version_func());

    const int32_t frame_length = pv_porcupine_frame_length_func();
    pv_recorder_t *recorder = NULL;
    pv_recorder_status_t recorder_status = pv_recorder_init(device_index, frame_length, 100, true, true, &recorder);
    if (recorder_status != PV_RECORDER_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to initialize device with %s.\n", pv_recorder_status_to_string(recorder_status));
        exit(1);
    }

    const char *selected_device = pv_recorder_get_selected_device(recorder);
    fprintf(stdout, "Selected device: %s.\n", selected_device);

    fprintf(stdout, "Start recording...\n");
    recorder_status = pv_recorder_start(recorder);
    if (recorder_status != PV_RECORDER_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to start device with %s.\n", pv_recorder_status_to_string(recorder_status));
        exit(1);
    }

    int16_t *pcm = malloc(frame_length * sizeof(int16_t));
    if (!pcm) {
        fprintf(stderr, "Failed to allocate pcm memory.\n");
        exit(1);
    }

    while (!is_interrupted) {
        recorder_status = pv_recorder_read(recorder, pcm);
        if (recorder_status != PV_RECORDER_STATUS_SUCCESS) {
            fprintf(stderr, "Failed to read with %s.\n", pv_recorder_status_to_string(recorder_status));
            exit(1);
        }

        int32_t keyword_index = -1;
        porcupine_status = pv_porcupine_process_func(porcupine, pcm, &keyword_index);
        if (porcupine_status != PV_STATUS_SUCCESS) {
            fprintf(stderr, "'pv_porcupine_process' failed with '%s'\n", pv_status_to_string_func(porcupine_status));
            exit(1);
        }

        if (keyword_index != -1) {
            fprintf(stdout, "keyword detected\n");
            fflush(stdout);
        }
    }
    fprintf(stdout, "\n");

    recorder_status = pv_recorder_stop(recorder);
    if (recorder_status != PV_RECORDER_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to stop device with %s.\n", pv_recorder_status_to_string(recorder_status));
        exit(1);
    }

    free(pcm);
    pv_recorder_delete(recorder);
    pv_porcupine_delete_func(porcupine);
    close_dl(porcupine_library);

    return 0;
}

int main(int argc, char *argv[]) {

#if defined(_WIN32) || defined(_WIN64)

#define UTF8_COMPOSITION_FLAG (0)
#define NULL_TERMINATED       (-1)

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
