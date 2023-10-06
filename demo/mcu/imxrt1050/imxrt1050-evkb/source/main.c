/*
    Copyright 2020-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "pv_porcupine_mcu.h"

#include "pv_audio_rec.h"
#include "pv_params.h"
#include "pv_imxrt1050.h"

#define MEMORY_BUFFER_SIZE (50 * 1024)

static const char* ACCESS_KEY = "${ACCESS_KEY}"; //AccessKey string obtained from Picovoice Console (https://picovoice.ai/console/)

static int8_t memory_buffer[MEMORY_BUFFER_SIZE] __attribute__((aligned(16)));

static const int32_t KEYWORD_MODEL_SIZES = sizeof(DEFAULT_KEYWORD_ARRAY);
static const void *KEYWORD_MODELS = DEFAULT_KEYWORD_ARRAY;
static const float SENSITIVITY = 0.75f;

static void wake_word_callback(void) {
    printf("[wake word]\n");
}

static void error_handler(void) {
    while(true);
}

void print_error_message(char **message_stack, int32_t message_stack_depth) {
    for (int32_t i = 0; i < message_stack_depth; i++) {
        printf("[%ld] %s\n", i, message_stack[i]);
    }
}

int main(void) {

    pv_status_t status = pv_board_init();
    if (status != PV_STATUS_SUCCESS) {
        error_handler();
    }

    status = pv_message_init();
    if (status != PV_STATUS_SUCCESS) {
        error_handler();
    }

    const uint8_t *board_uuid = pv_get_uuid();
    printf("UUID: ");
    for (uint32_t i = 0; i < pv_get_uuid_size(); i++) {
        printf(" %.2x", board_uuid[i]);
    }
    printf("\r\n");

    status = pv_audio_rec_init();
    if (status != PV_STATUS_SUCCESS) {
        printf("Audio init failed with '%s'", pv_status_to_string(status));
        error_handler();
    }

    status = pv_audio_rec_start();
    if (status != PV_STATUS_SUCCESS) {
        printf("Recording audio failed with '%s'", pv_status_to_string(status));
        error_handler();
    }

    pv_porcupine_t *handle = NULL;

    char **message_stack = NULL;
    int32_t message_stack_depth = 0;
    pv_status_t error_status;

    status = pv_porcupine_init(
            ACCESS_KEY,
            MEMORY_BUFFER_SIZE,
            memory_buffer,
            1,
            &KEYWORD_MODEL_SIZES,
            &KEYWORD_MODELS,
            &SENSITIVITY,
            &handle);

    if (status != PV_STATUS_SUCCESS) {
        printf("Porcupine init failed with '%s':\n", pv_status_to_string(status));

        error_status = pv_get_error_stack(&message_stack, &message_stack_depth);
        if (error_status != PV_STATUS_SUCCESS) {
            printf("Unable to get Porcupine error state with '%s':\n", pv_status_to_string(error_status));
            error_handler();
        }

        print_error_message(message_stack, message_stack_depth);
        pv_free_error_stack(message_stack);

        error_handler();
    }

    while (true) {
        const int16_t *buffer = pv_audio_rec_get_new_buffer();
        if (buffer) {
            int32_t keyword_index;
            const pv_status_t status = pv_porcupine_process(handle, buffer, &keyword_index);
            if (status != PV_STATUS_SUCCESS) {
                printf("Porcupine process failed with '%s'", pv_status_to_string(status));
                error_handler();
            }
            if (keyword_index != -1) {
                wake_word_callback();
            }
        }

    }
    pv_board_deinit();
    pv_audio_rec_deinit();
    pv_porcupine_delete(handle);
}
