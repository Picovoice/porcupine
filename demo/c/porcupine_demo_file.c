/*
    Copyright 2018 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "pv_porcupine.h"

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "usage : %s library_path model_path keyword_path sensitivity wav_path\n", argv[0]);
        exit(1);
    }

    const char *library_path = argv[1];
    const char *model_path = argv[2];
    const char *keyword_path = argv[3];
    const float sensitivity = (float) atof(argv[4]);
    const char *wav_path = argv[5];

    void *porcupine_library = dlopen(library_path, RTLD_NOW);
    if (!porcupine_library) {
        fprintf(stderr, "failed to open library\n");
        exit(1);
    }

    char *error;

    const char *(*pv_status_to_string)(pv_status_t);
    pv_status_to_string = dlsym(porcupine_library, "pv_status_to_string");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_status_to_string' with '%s'.\n", error);
        exit(1);
    }

    int32_t (*pv_sample_rate)();
    pv_sample_rate = dlsym(porcupine_library, "pv_sample_rate");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_sample_rate' with '%s'.\n", error);
        exit(1);
    }

    pv_status_t (*pv_porcupine_init)(const char *, int32_t, const char *const *, const float *, pv_porcupine_t **);
    pv_porcupine_init = dlsym(porcupine_library, "pv_porcupine_init");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_porcupine_init' with '%s'.\n", error);
        exit(1);
    }

    void (*pv_porcupine_delete)(pv_porcupine_t *);
    pv_porcupine_delete = dlsym(porcupine_library, "pv_porcupine_delete");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_porcupine_delete' with '%s'.\n", error);
        exit(1);
    }

    pv_status_t (*pv_porcupine_process)(pv_porcupine_t *, const int16_t *, int32_t *);
    pv_porcupine_process = dlsym(porcupine_library, "pv_porcupine_process");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_porcupine_process' with '%s'.\n", error);
        exit(1);
    }

    int32_t (*pv_porcupine_frame_length)();
    pv_porcupine_frame_length = dlsym(porcupine_library, "pv_porcupine_frame_length");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_porcupine_frame_length' with '%s'.\n", error);
        exit(1);
    }

    pv_porcupine_t *porcupine;
    pv_status_t status = pv_porcupine_init(model_path, 1, &keyword_path, &sensitivity, &porcupine);
    if (status != PV_STATUS_SUCCESS) {
        fprintf(stderr, "'pv_porcupine_init' failed with '%s'\n", pv_status_to_string(status));
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

    const int32_t frame_length = pv_porcupine_frame_length();

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

        int32_t keyword_index;
        status = pv_porcupine_process(porcupine, pcm, &keyword_index);
        if (status != PV_STATUS_SUCCESS) {
            fprintf(stderr, "'pv_porcupine_process' failed with '%s'\n", pv_status_to_string(status));
            exit(1);
        }

        struct timeval after;
        gettimeofday(&after, NULL);

        if (keyword_index != -1) {
            fprintf(stdout, "detected at %.1f seconds\n", (double) frame_index * frame_length / pv_sample_rate());
        }

        total_cpu_time_usec +=
                (double) (after.tv_sec - before.tv_sec) * 1e6 + (double) (after.tv_usec - before.tv_usec);
        total_processed_time_usec += (frame_length * 1e6) / pv_sample_rate();
        frame_index++;
    }

    const double real_time_factor = total_cpu_time_usec / total_processed_time_usec;
    fprintf(stdout, "real time factor : %.3f\n", real_time_factor);

    free(pcm);
    fclose(wav);
    pv_porcupine_delete(porcupine);
    dlclose(porcupine_library);

    return 0;
}
