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

#include "board.h"
#include "clock_config.h"
#include "fsl_codec_adapter.h"
#include "fsl_codec_common.h"
#include "fsl_dmamux.h"
#include "fsl_sai_edma.h"
#include "fsl_wm8960.h"

#include "picovoice.h"

#define PV_SAI                  (SAI1)
#define PV_SAI_CHANNEL          (0)
#define PV_SAI_IRQ              (SAI1_IRQn)
#define PV_SAITxIRQHandler      (SAI1_IRQHandler)
#define PV_SAI_TX_SYNC_MODE     (kSAI_ModeAsync)
#define PV_SAI_RX_SYNC_MODE     (kSAI_ModeSync)
#define PV_SAI_MCLK_OUTPUT      (true)
#define PV_SAI_MASTER_SLAVE     (kSAI_Master)
#define PV_SAI1_CLOCK_SOURCE_SELECT         (2U)
#define PV_SAI1_CLOCK_SOURCE_PRE_DIVIDER    (0U)
#define PV_SAI1_CLOCK_SOURCE_DIVIDER        (63U)
#define PV_SAI_CLK_FREQ                                                        \
    (CLOCK_GetFreq(kCLOCK_AudioPllClk) / (PV_SAI1_CLOCK_SOURCE_DIVIDER + 1U) / \
    (PV_SAI1_CLOCK_SOURCE_PRE_DIVIDER + 1U))
#define PV_SAI_TX_IRQ           (SAI1_IRQn)
#define PV_SAI_RX_IRQ           (SAI1_IRQn)

#define PV_I2C                  (LPI2C1)
#define PV_LPI2C_CLOCK_SOURCE_SELECT        (0U)
#define PV_LPI2C_CLOCK_SOURCE_DIVIDER       (5U)
#define PV_I2C_CLK_FREQ ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (PV_LPI2C_CLOCK_SOURCE_DIVIDER + 1U))

#define PV_AUDIO_REC_AUDIO_FREQUENCY        (kSAI_SampleRate16KHz)
#define PV_AUDIO_REC_CHANNEL_NUMBER         (2U)
#define PV_AUDIO_REC_RECORD_BUFFER_SIZE     (512U)
#define PV_AUDIO_REC_BIT_WIDTH              (kSAI_WordWidth16bits)
#define PV_AUDIO_REC_MASTER_CLOCK           (PV_SAI_CLK_FREQ)
#define AUDIO_IN_PCM_BUFFER_SIZE_INT16      ((uint32_t)(PV_AUDIO_REC_AUDIO_FREQUENCY / 1000 \
                                            * PV_AUDIO_REC_CHANNEL_NUMBER))
#define AUDIO_IN_PCM_BUFFER_SIZE_INT8       (2 * AUDIO_IN_PCM_BUFFER_SIZE_INT16)
#define PV_AUDIO_REC_RECORD_BUFFER_NUMBER   (2U)

#define PV_DMA             (DMA0)
#define PV_DMAMUX          (DMAMUX)
#define PV_TX_EDMA_CHANNEL (0U)
#define PV_RX_EDMA_CHANNEL (1U)
#define PV_SAI_TX_SOURCE   (kDmaRequestMuxSai1Tx)
#define PV_SAI_RX_SOURCE   (kDmaRequestMuxSai1Rx)

AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t record_pcm_buffer[AUDIO_IN_PCM_BUFFER_SIZE_INT8], 16);
AT_NONCACHEABLE_SECTION_ALIGN(static int16_t ping_pong_buffer[PV_AUDIO_REC_RECORD_BUFFER_NUMBER][PV_AUDIO_REC_RECORD_BUFFER_SIZE], 4);

AT_NONCACHEABLE_SECTION_INIT(static sai_edma_handle_t txHandle);
AT_NONCACHEABLE_SECTION_INIT(static sai_edma_handle_t rxHandle);

static edma_handle_t dmaRxHandle = {0};
static codec_handle_t codecHandle = {0};
static sai_transfer_t buffer_config = {0};
static edma_config_t dmaConfig = {0};
static sai_transceiver_t config = {0};

static bool rx_error = false;
static int32_t last_read_index = -1;
volatile static int32_t read_index = 1;
volatile static int32_t write_index = 0;
volatile static int32_t buffer_index = 0;

struct {
    uint32_t channel_number;
    uint32_t audio_frequency;
    uint32_t record_buffer_size;
    uint8_t *record_pcm_buffer;
    bool is_recording;
} pv_audio_rec;

static void rx_callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
{
    if (kStatus_SAI_RxError == status) {
        rx_error = true;
    } else {
        int16_t *record_pcm_buffer_int16 = (int16_t*) record_pcm_buffer;
        for (uint32_t i = 0; i < (AUDIO_IN_PCM_BUFFER_SIZE_INT16 / 2); i++) {
            ping_pong_buffer[write_index][buffer_index++] = record_pcm_buffer_int16[i * 2 + 1];
        }
        if (buffer_index >= PV_AUDIO_REC_RECORD_BUFFER_SIZE) {
            read_index = write_index;
            write_index = 1 - write_index;
            buffer_index = 0;
        }
        if (pv_audio_rec.is_recording) {
            buffer_config.data     = record_pcm_buffer;
            buffer_config.dataSize = AUDIO_IN_PCM_BUFFER_SIZE_INT8;
            SAI_TransferReceiveEDMA(PV_SAI, &rxHandle, &buffer_config);
        }
    }
}

static void BOARD_EnableSaiMclkOutput(bool enable)
{
    if (enable)
    {
        IOMUXC_GPR->GPR1 |= IOMUXC_GPR_GPR1_SAI1_MCLK_DIR_MASK;
    }
    else
    {
        IOMUXC_GPR->GPR1 &= (~IOMUXC_GPR_GPR1_SAI1_MCLK_DIR_MASK);
    }
}

pv_status_t pv_audio_rec_init(void) {

    wm8960_config_t wm8960Config = {
        .i2cConfig = {.codecI2CInstance = BOARD_CODEC_I2C_INSTANCE, .codecI2CSourceClock = BOARD_CODEC_I2C_CLOCK_FREQ},
        .route     = kWM8960_RouteRecord,
        .rightInputSource = kWM8960_InputDifferentialMicInput2,
        .playSource       = kWM8960_PlaySourceDAC,
        .slaveAddress     = WM8960_I2C_ADDR,
        .bus              = kWM8960_BusI2S,
        .format = {.mclk_HZ = 6144000U, .sampleRate = kWM8960_AudioSampleRate16KHz, .bitWidth = kWM8960_AudioBitWidth16bit},
        .master_slave = false,
    };
    codec_config_t boardCodecConfig = {.codecDevType = kCODEC_WM8960, .codecDevConfig = &wm8960Config};

    const clock_audio_pll_config_t audioPllConfig = {
        .loopDivider = 32,
        .postDivider = 1,
        .numerator   = 77,
        .denominator = 100,
    };

    CLOCK_InitAudioPll(&audioPllConfig);

    CLOCK_SetMux(kCLOCK_Lpi2cMux, PV_LPI2C_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, PV_LPI2C_CLOCK_SOURCE_DIVIDER);
    CLOCK_SetMux(kCLOCK_Sai1Mux, PV_SAI1_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_Sai1PreDiv, PV_SAI1_CLOCK_SOURCE_PRE_DIVIDER);
    CLOCK_SetDiv(kCLOCK_Sai1Div, PV_SAI1_CLOCK_SOURCE_DIVIDER);
    BOARD_EnableSaiMclkOutput(true);
    DMAMUX_Init(PV_DMAMUX);
    DMAMUX_SetSource(PV_DMAMUX, PV_RX_EDMA_CHANNEL, (uint8_t)PV_SAI_RX_SOURCE);
    DMAMUX_EnableChannel(PV_DMAMUX, PV_RX_EDMA_CHANNEL);

    EDMA_GetDefaultConfig(&dmaConfig);
    EDMA_Init(PV_DMA, &dmaConfig);
    EDMA_CreateHandle(&dmaRxHandle, PV_DMA, PV_RX_EDMA_CHANNEL);

    SAI_Init(PV_SAI);
    SAI_TransferRxCreateHandleEDMA(PV_SAI, &rxHandle, rx_callback, NULL, &dmaRxHandle);
    SAI_GetClassicI2SConfig(&config, PV_AUDIO_REC_BIT_WIDTH, kSAI_Stereo, 1U << PV_SAI_CHANNEL);
    config.syncMode    = PV_SAI_TX_SYNC_MODE;
    config.masterSlave = PV_SAI_MASTER_SLAVE;
    SAI_TransferTxSetConfigEDMA(PV_SAI, &txHandle, &config);
    config.syncMode = PV_SAI_RX_SYNC_MODE;
    SAI_TransferRxSetConfigEDMA(PV_SAI, &rxHandle, &config);
    SAI_TxSetBitClockRate(PV_SAI, PV_AUDIO_REC_MASTER_CLOCK, PV_AUDIO_REC_AUDIO_FREQUENCY, PV_AUDIO_REC_BIT_WIDTH,
                          PV_AUDIO_REC_CHANNEL_NUMBER);
    SAI_RxSetBitClockRate(PV_SAI, PV_AUDIO_REC_MASTER_CLOCK, PV_AUDIO_REC_AUDIO_FREQUENCY, PV_AUDIO_REC_BIT_WIDTH,
                          PV_AUDIO_REC_CHANNEL_NUMBER);

    if (!CODEC_Init(&codecHandle, &boardCodecConfig)) {
        return PV_STATUS_INVALID_STATE;
    }

    pv_audio_rec.channel_number = PV_AUDIO_REC_CHANNEL_NUMBER;
    pv_audio_rec.audio_frequency = PV_AUDIO_REC_AUDIO_FREQUENCY;
    pv_audio_rec.record_buffer_size = PV_AUDIO_REC_RECORD_BUFFER_SIZE;
    pv_audio_rec.record_pcm_buffer = record_pcm_buffer;
    return PV_STATUS_SUCCESS;
}

pv_status_t pv_audio_rec_start(void) {
    buffer_config.data     = record_pcm_buffer;
    buffer_config.dataSize = AUDIO_IN_PCM_BUFFER_SIZE_INT8;
    if (kStatus_Success == SAI_TransferReceiveEDMA(PV_SAI, &rxHandle, &buffer_config)) {
        pv_audio_rec.is_recording = true;
        return PV_STATUS_SUCCESS;
    } else {
        return PV_STATUS_INVALID_STATE;
    }
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

void pv_audio_rec_deinit(void) {
}

