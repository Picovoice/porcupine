/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#if !defined(_WIN32) && !defined(_WIN64)

#include <dlfcn.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

#endif

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

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "usage : %s library_path model_path keyword_path sensitivity wav_path\n", argv[0]);
        exit(1);
    }

    const char *library_path = argv[1];
    const char *model_path = argv[2];
    const char *keyword_path = argv[3];
    const float sensitivity = strtod(argv[4], NULL);
    const char *wav_path = argv[5];

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

    pv_status_t (*pv_porcupine_init_func)(const char *, int32_t, const char *const *, const float *, pv_porcupine_t **) =
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

    pv_status_t (*pv_porcupine_process_func)(pv_porcupine_t *, const int16_t *, int32_t *) =
            load_symbol(porcupine_library, "pv_porcupine_process");
    if (!pv_porcupine_process_func) {
        print_dl_error("failed to load 'pv_porcupine_process'");
        exit(1);
    }

    int32_t (*pv_porcupine_frame_length_func)() = load_symbol(porcupine_library, "pv_porcupine_frame_length");
    if (!pv_porcupine_frame_length_func) {
        print_dl_error("failed to load 'pv_porcupine_frame_length' with '%s'.\n");
        exit(1);
    }

    pv_porcupine_t *porcupine = NULL;
    pv_status_t status = pv_porcupine_init_func(model_path, 1, &keyword_path, &sensitivity, &porcupine);
    if (status != PV_STATUS_SUCCESS) {
        fprintf(stderr, "'pv_porcupine_init' failed with '%s'\n", pv_status_to_string_func(status));
        exit(1);
    }

    FILE *wav = fopen(wav_path, "rb");
    if (!wav) {
        fprintf(stderr, "failed to open wav file\n");
        exit(1);
    }

    if (fseek(wav, 44, SEEK_SET) != 0) {
        fprintf(stderr, "failed to skip the wav header\n");
        exit(1);
    }

    const int32_t frame_length = pv_porcupine_frame_length_func();

    int16_t *pcm = malloc(sizeof(int16_t) * frame_length);
    if (!pcm) {
        fprintf(stderr, "failed to allocate memory for audio buffer\n");
        exit(1);
    }

    double total_cpu_time_usec = 0;
    double total_processed_time_usec = 0;
    int32_t frame_index = 0;

    while (fread(pcm, sizeof(int16_t), frame_length, wav) == (size_t) frame_length) {
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

        if (keyword_index != -1) {
            fprintf(stdout, "detected at %.1f seconds\n", (double) frame_index * frame_length / pv_sample_rate_func());
        }

        total_cpu_time_usec +=
                (double) (after.tv_sec - before.tv_sec) * 1e6 + (double) (after.tv_usec - before.tv_usec);
        total_processed_time_usec += (frame_length * 1e6) / pv_sample_rate_func();
        frame_index++;
    }

    const double real_time_factor = total_cpu_time_usec / total_processed_time_usec;
    fprintf(stdout, "real time factor : %.3f\n", real_time_factor);

    free(pcm);
    fclose(wav);
    pv_porcupine_delete_func(porcupine);
    close_dl(porcupine_library);

    return 0;
}
