/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "pdm2pcm.h"

#include "picovoice.h"

#include "main.h"

#define PV_AUDIO_REC_AUDIO_FREQUENCY (16000U)
#define PV_AUDIO_REC_CHANNEL_NUMBER (1)
#define PV_AUDIO_REC_RECORD_BUFFER_SIZE (512)
#define PV_AUDIO_REC_VOLUME_LEVEL (80)

#define AUDIO_IN_PCM_BUFFER_SIZE    (MIC_CAPTURE_IT_MS * 16)

static int16_t ping_pong_buffer[2][PV_AUDIO_REC_RECORD_BUFFER_SIZE];

static int32_t last_read_index = -1;
static int32_t read_index = 1;
static int32_t write_index = 0;
static int32_t buffer_index = 0;

CRC_HandleTypeDef hcrc;

UART_HandleTypeDef huart;
SAI_HandleTypeDef hsai_BlockA4;
DMA_HandleTypeDef hdma_sai4_a;

#define PV_AUDIO_DUMP_DURATION_SEC      (3)
#define PV_AUDIO_REC_AUDIO_FREQUENCY    (16000U)
#define PV_AUDIO_DUMP_BUFFER_SIZE   ((uint32_t)(PV_AUDIO_DUMP_DURATION_SEC * PV_AUDIO_REC_AUDIO_FREQUENCY))

struct {
    uint32_t channel_number;
    uint32_t audio_frequency;
    uint32_t record_buffer_size;
    uint16_t *record_pcm_buffer;
    bool is_recording;
} pv_audio_rec;

static void MX_SAI4_Init(void)
{

  hsai_BlockA4.Instance = SAI4_Block_A;
  hsai_BlockA4.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockA4.Init.AudioMode = SAI_MODEMASTER_RX;
  hsai_BlockA4.Init.DataSize = SAI_DATASIZE_8;
  hsai_BlockA4.Init.FirstBit = SAI_FIRSTBIT_LSB;
  hsai_BlockA4.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
  hsai_BlockA4.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA4.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA4.Init.NoDivider = SAI_MASTERDIVIDER_DISABLE;
  hsai_BlockA4.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;
  hsai_BlockA4.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_MCKDIV;
  hsai_BlockA4.Init.Mckdiv = 6;
  hsai_BlockA4.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA4.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA4.Init.PdmInit.Activation = ENABLE;
  hsai_BlockA4.Init.PdmInit.MicPairsNbr = 2;
  hsai_BlockA4.Init.PdmInit.ClockEnable = SAI_PDM_CLOCK2_ENABLE;
  hsai_BlockA4.FrameInit.FrameLength = 32;
  hsai_BlockA4.FrameInit.ActiveFrameLength = 1;
  hsai_BlockA4.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
  hsai_BlockA4.FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
  hsai_BlockA4.FrameInit.FSOffset = SAI_FS_FIRSTBIT;
  hsai_BlockA4.SlotInit.FirstBitOffset = 0;
  hsai_BlockA4.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  hsai_BlockA4.SlotInit.SlotNumber = 4;
  hsai_BlockA4.SlotInit.SlotActive = 0x00000004;
  if (HAL_SAI_Init(&hsai_BlockA4) != HAL_OK) {
       pv_error_handler();
  }

}

static void MX_BDMA_Init(void)
{
  __HAL_RCC_BDMA_CLK_ENABLE();
  NVIC_SetPriority(BDMA_Channel0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(BDMA_Channel0_IRQn);

}

static void MX_DMA_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}

static void MX_CRC_Init(void)
{

  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK) {
       pv_error_handler();
  }
  __HAL_CRC_DR_RESET(&hcrc);

}

pv_status_t pv_audio_rec_init(void) {

     MX_BDMA_Init();
     MX_DMA_Init();
     MX_SAI4_Init();
     MX_CRC_Init();
     MX_PDM2PCM_Init();

    return PV_STATUS_SUCCESS;
}

pv_status_t pv_audio_rec_start(void) {
    mic_capture_start();
    pv_audio_rec.is_recording = true;
    return PV_STATUS_SUCCESS;
}

pv_status_t pv_audio_rec_stop(void) {
    pv_audio_rec.is_recording = false;
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

void pv_pcm_process(int16_t *record_pcm_buffer) {

    int32_t temp_buffer_size = PV_AUDIO_REC_RECORD_BUFFER_SIZE - buffer_index;
    if (temp_buffer_size < AUDIO_IN_PCM_BUFFER_SIZE)  {
        for (uint32_t i = 0; i < temp_buffer_size; i++) {
            ping_pong_buffer[write_index][buffer_index++] = *record_pcm_buffer++;
        }
        read_index = write_index;
        write_index = 1 - write_index;
        buffer_index = 0;
        temp_buffer_size = AUDIO_IN_PCM_BUFFER_SIZE - temp_buffer_size;
    } else {
        temp_buffer_size = AUDIO_IN_PCM_BUFFER_SIZE;
    }

    for (uint32_t i = 0; i < temp_buffer_size; i++) {
        ping_pong_buffer[write_index][buffer_index++] = *record_pcm_buffer++;
    }

    if (buffer_index >= PV_AUDIO_REC_RECORD_BUFFER_SIZE) {
        read_index = write_index;
        write_index = 1 - write_index;
        buffer_index = 0;
    }
}
