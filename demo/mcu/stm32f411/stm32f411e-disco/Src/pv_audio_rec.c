/*
    Copyright 2020-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "stm32f411e_discovery_audio.h"

#include "picovoice.h"

#define PV_AUDIO_REC_AUDIO_FREQUENCY (16000U)
#define PV_AUDIO_REC_CHANNEL_NUMBER (1)
#define PV_AUDIO_REC_RECORD_BUFFER_SIZE (512)
#define PV_AUDIO_REC_VOLUME_LEVEL (100)

#define AUDIO_IN_PCM_BUFFER_SIZE    ((uint32_t)(PV_AUDIO_REC_AUDIO_FREQUENCY / 1000 * PV_AUDIO_REC_CHANNEL_NUMBER))
#define AUDIO_IN_PDM_BUFFER_SIZE    ((uint32_t)(128 * PV_AUDIO_REC_AUDIO_FREQUENCY / 16000 * PV_AUDIO_REC_CHANNEL_NUMBER))

static uint16_t record_pdm_buffer[AUDIO_IN_PDM_BUFFER_SIZE];
static uint16_t record_pcm_buffer[2 * AUDIO_IN_PCM_BUFFER_SIZE];
static int16_t ping_pong_buffer[2][PV_AUDIO_REC_RECORD_BUFFER_SIZE];

static int32_t last_read_index = -1;
static int32_t read_index = 1;
static int32_t write_index = 0;
static int32_t buffer_index = 0;

struct {
    uint32_t channel_number;
    uint32_t audio_frequency;
    uint32_t record_buffer_size;
    uint16_t *record_pcm_buffer;
    bool is_recording;
} pv_audio_rec;

pv_status_t pv_audio_rec_init(void) {
    pv_audio_rec.channel_number = PV_AUDIO_REC_CHANNEL_NUMBER;
    pv_audio_rec.audio_frequency = PV_AUDIO_REC_AUDIO_FREQUENCY;
    pv_audio_rec.record_buffer_size = PV_AUDIO_REC_RECORD_BUFFER_SIZE;
    pv_audio_rec.record_pcm_buffer = record_pcm_buffer;

    if (BSP_AUDIO_IN_Init(PV_AUDIO_REC_AUDIO_FREQUENCY, DEFAULT_AUDIO_IN_BIT_RESOLUTION, PV_AUDIO_REC_CHANNEL_NUMBER) != AUDIO_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    if (BSP_AUDIO_IN_SetVolume(PV_AUDIO_REC_VOLUME_LEVEL) != AUDIO_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    return PV_STATUS_SUCCESS;
}

pv_status_t pv_audio_rec_start(void) {
    if (BSP_AUDIO_IN_Record((uint16_t*)(record_pdm_buffer), AUDIO_IN_PDM_BUFFER_SIZE) != AUDIO_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    pv_audio_rec.is_recording = true;
    return PV_STATUS_SUCCESS;
}

const int16_t *pv_audio_rec_get_new_buffer(void) {
    if (read_index == -1) {
        return NULL;
    }

    if (last_read_index == read_index) {
        return NULL;
    }

    last_read_index = read_index;

    return ping_pong_buffer[read_index];
}

void BSP_AUDIO_IN_TransferComplete_CallBack(void) {
    BSP_AUDIO_IN_PDMToPCM((uint16_t*) &record_pdm_buffer[AUDIO_IN_PDM_BUFFER_SIZE / 2],
            record_pcm_buffer);
    for (uint32_t i = 0; i < AUDIO_IN_PCM_BUFFER_SIZE; i++) {
        ping_pong_buffer[write_index][buffer_index++] = record_pcm_buffer[i * 2];
    }
    if (buffer_index >= PV_AUDIO_REC_RECORD_BUFFER_SIZE) {
        read_index = write_index;
        write_index = 1 - write_index;
        buffer_index = 0;
    }
}

void BSP_AUDIO_IN_HalfTransfer_CallBack(void) {
    BSP_AUDIO_IN_PDMToPCM((uint16_t*) &record_pdm_buffer[0],
            record_pcm_buffer);
    for (uint32_t i = 0; i < AUDIO_IN_PCM_BUFFER_SIZE; i++) {
        ping_pong_buffer[write_index][buffer_index++] = record_pcm_buffer[i * 2];
    }
    if (buffer_index >= PV_AUDIO_REC_RECORD_BUFFER_SIZE) {
        read_index = write_index;
        write_index = 1 - write_index;
        buffer_index = 0;
    }
}

void pv_audio_rec_deinit(void) {
    BSP_AUDIO_IN_Stop();
}
