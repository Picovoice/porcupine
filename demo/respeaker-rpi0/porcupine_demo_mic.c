/*
    Copyright 2018-2020 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <alsa/asoundlib.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "pv_porcupine.h"

static const uint8_t OFF_RGB[3] = {0, 0, 0};
static const uint8_t BLUE_RGB[3] = {0, 0, 255};
static const uint8_t GREEN_RGB[3] = {0, 255, 0};
static const uint8_t ORANGE_RGB[3] = {255, 128, 0};
static const uint8_t PINK_RGB[3] = {255, 51, 153};
static const uint8_t PURPLE_RGB[3] = {128, 0, 128};
static const uint8_t RED_RGB[3] = {255, 0, 0};
static const uint8_t WHITE_RGB[3] = {255, 255, 255};
static const uint8_t YELLOW_RGB[3] = {255, 255, 51};

static volatile bool is_interrupted = false;

/*
// LED SPI Documentation:
https://github.com/torvalds/linux/blob/master/include/uapi/linux/spi/spi.h
https://github.com/torvalds/linux/blob/master/include/uapi/linux/spi/spidev.h
https://cdn-shop.adafruit.com/datasheets/APA102.pdf
*/
static const uint8_t spi_mode = 0;
static const uint8_t spi_BPW = 8;
static const uint32_t spi_speed = 6000000;
static const uint16_t spi_delay = 0;
static int spidev_fd = -1;

static void setup_spi() {
    spidev_fd = open("/dev/spidev0.0", O_RDWR);
    if (spidev_fd < 0) {
        fprintf(stderr, "unable to open SPI device '%s'.\n", strerror(errno));
        exit(1);
    }

    if (ioctl(spidev_fd, SPI_IOC_WR_MODE, &spi_mode) < 0) {
        fprintf(stderr, "failed to change SPI mode '%s'.\n", strerror(errno));
        exit(1);
    }

    if (ioctl(spidev_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_BPW) < 0) {
        fprintf(stderr, "failed to change SPI BPW '%s'.\n", strerror(errno));
        exit(1);
    }

    if (ioctl(spidev_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0) {
        fprintf(stderr, "failed to change SPI speed '%s'.\n", strerror(errno));
        exit(1);
    }
}

static void spi_write_data(unsigned char *data, int len) {
    struct spi_ioc_transfer spi;
    memset(&spi, 0, sizeof(spi));

    spi.tx_buf = (unsigned long) data;
    spi.rx_buf = (unsigned long) data;
    spi.len = len;
    spi.delay_usecs = spi_delay;
    spi.speed_hz = spi_speed;
    spi.bits_per_word = spi_BPW;

    if (ioctl(spidev_fd, SPI_IOC_MESSAGE(1), &spi) < 0) {
        fprintf(stderr, "failed to write to SPI '%s'.\n", strerror(errno));
        exit(1);
    }
}

static void set_color(const uint8_t rgb[3]) {
    for (int32_t i = 0; i < 4; i++) {
        uint8_t zero = 0x00;
        spi_write_data(&zero, 1);
    }

    static const uint32_t BRIGHTNESS = 1;
    for (int32_t i = 0; i < 12; i++) {
        uint8_t led_frame[4];
        led_frame[0] = 0b11100000 | (0b00011111 & BRIGHTNESS);
        led_frame[1] = rgb[2];
        led_frame[2] = rgb[1];
        led_frame[3] = rgb[0];
        spi_write_data(led_frame, 4);
    }

    for (int32_t i = 0; i < 4; i++) {
        uint8_t zero = 0x00;
        spi_write_data(&zero, 1);
    }
}

void interrupt_handler(int _) {
    (void) _;
    is_interrupted = true;
}

int main(int argc, char *argv[]) {
    if (argc != 15) {
        fprintf(stderr,
                "usage : %s access_key library_path model_path sensitivity input_audio_device alexa_keyword_path "
                "computer_keyword_path hey_google_keyword_path hey_siri_keyword_path jarvis_keyword_path "
                "picovoice_keyword_path porcupine_keyword_path bumblebee_keyword_path terminator_keyword_path\n",
                argv[0]);
        exit(1);
    }

    signal(SIGINT, interrupt_handler);

    const char *access_key = argv[1];
    const char *library_path = argv[2];
    const char *model_path = argv[3];
    const float sensitivity = (float) atof(argv[4]);
    const char *input_audio_device = argv[5];
    const char **keyword_paths = (const char **) &argv[6];
    const int32_t num_keywords = 9;

    void *porcupine_library = dlopen(library_path, RTLD_NOW);
    if (!porcupine_library) {
        fprintf(stderr, "failed to open library.\n");
        exit(1);
    }

    char *error = NULL;

    const char *(*pv_status_to_string_func)(pv_status_t) = dlsym(porcupine_library, "pv_status_to_string");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_status_to_string' with '%s'.\n", error);
        exit(1);
    }

    int32_t (*pv_sample_rate_func)() = dlsym(porcupine_library, "pv_sample_rate");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_sample_rate' with '%s'.\n", error);
        exit(1);
    }

    pv_status_t (*pv_porcupine_init_func)(const char *, const char *, int32_t, const char *const *, const float *, pv_porcupine_t **) =
            dlsym(porcupine_library, "pv_porcupine_init");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_porcupine_init' with '%s'.\n", error);
        exit(1);
    }

    void (*pv_porcupine_delete_func)(pv_porcupine_t *) = dlsym(porcupine_library, "pv_porcupine_delete");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_porcupine_delete' with '%s'.\n", error);
        exit(1);
    }

    pv_status_t (*pv_porcupine_process_func)(pv_porcupine_t *, const int16_t *, int32_t *) = dlsym(porcupine_library, "pv_porcupine_process");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_porcupine_process' with '%s'.\n", error);
        exit(1);
    }

    int32_t (*pv_porcupine_frame_length_func)() = dlsym(porcupine_library, "pv_porcupine_frame_length");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "failed to load 'pv_porcupine_frame_length' with '%s'.\n", error);
        exit(1);
    }

    pv_porcupine_t *porcupine = NULL;
    float sensitivities[num_keywords];
    for (int32_t i = 0; i < num_keywords; i++) {
        sensitivities[i] = sensitivity;
    }
    pv_status_t status = pv_porcupine_init_func(access_key, model_path, num_keywords, keyword_paths, sensitivities, &porcupine);
    if (status != PV_STATUS_SUCCESS) {
        fprintf(stderr, "'pv_porcupine_init' failed with '%s'\n", pv_status_to_string_func(status));
        exit(1);
    }

    snd_pcm_t *alsa_handle = NULL;
    int error_code = snd_pcm_open(&alsa_handle, input_audio_device, SND_PCM_STREAM_CAPTURE, 0);
    if (error_code != 0) {
        fprintf(stderr, "'snd_pcm_open' failed with '%s'\n", snd_strerror(error_code));
        exit(1);
    }

    snd_pcm_hw_params_t *hardware_params = NULL;
    error_code = snd_pcm_hw_params_malloc(&hardware_params);
    if (error_code != 0) {
        fprintf(stderr, "'snd_pcm_hw_params_malloc' failed with '%s'\n", snd_strerror(error_code));
        exit(1);
    }

    error_code = snd_pcm_hw_params_any(alsa_handle, hardware_params);
    if (error_code != 0) {
        fprintf(stderr, "'snd_pcm_hw_params_any' failed with '%s'\n", snd_strerror(error_code));
        exit(1);
    }

    error_code = snd_pcm_hw_params_set_access(alsa_handle, hardware_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (error_code != 0) {
        fprintf(stderr, "'snd_pcm_hw_params_set_access' failed with '%s'\n", snd_strerror(error_code));
        exit(1);
    }

    error_code = snd_pcm_hw_params_set_format(alsa_handle, hardware_params, SND_PCM_FORMAT_S16_LE);
    if (error_code != 0) {
        fprintf(stderr, "'snd_pcm_hw_params_set_format' failed with '%s'\n", snd_strerror(error_code));
        exit(1);
    }

    error_code = snd_pcm_hw_params_set_rate(alsa_handle, hardware_params, pv_sample_rate_func(), 0);
    if (error_code != 0) {
        fprintf(stderr, "'snd_pcm_hw_params_set_rate' failed with '%s'\n", snd_strerror(error_code));
        exit(1);
    }

    error_code = snd_pcm_hw_params_set_channels(alsa_handle, hardware_params, 1);
    if (error_code != 0) {
        fprintf(stderr, "'snd_pcm_hw_params_set_channels' failed with '%s'\n", snd_strerror(error_code));
        exit(1);
    }

    error_code = snd_pcm_hw_params(alsa_handle, hardware_params);
    if (error_code != 0) {
        fprintf(stderr, "'snd_pcm_hw_params' failed with '%s'\n", snd_strerror(error_code));
        exit(1);
    }

    snd_pcm_hw_params_free(hardware_params);

    error_code = snd_pcm_prepare(alsa_handle);
    if (error_code != 0) {
        fprintf(stderr, "'snd_pcm_prepare' failed with '%s'\n", snd_strerror(error_code));
        exit(1);
    }

    const int32_t frame_length = pv_porcupine_frame_length_func();

    int16_t *pcm = malloc(frame_length * sizeof(int16_t));
    if (!pcm) {
        fprintf(stderr, "failed to allocate memory for audio buffer\n");
        exit(1);
    }

    setup_spi();
    fprintf(stdout, "[Listening]\n");

    while (!is_interrupted) {
        const int count = snd_pcm_readi(alsa_handle, pcm, frame_length);
        if (count < 0) {
            fprintf(stderr, "'snd_pcm_readi' failed with '%s'\n", snd_strerror(count));
            exit(1);
        } else if (count != frame_length) {
            fprintf(stderr, "read %d frames instead of %d\n", count, frame_length);
            exit(1);
        }

        int32_t keyword_index = -1;
        status = pv_porcupine_process_func(porcupine, pcm, &keyword_index);
        if (status != PV_STATUS_SUCCESS) {
            fprintf(stderr, "'pv_porcupine_process' failed with '%s'\n", pv_status_to_string_func(status));
            exit(1);
        }
        if (keyword_index != -1) {
            static const char *KEYWORDS[] = {
                    "Alexa",
                    "Computer",
                    "Hey Google",
                    "Hey Siri",
                    "Jarvis",
                    "Picovoice",
                    "Porcupine",
                    "Bumblebee",
                    "Terminator",
            };

            fprintf(stdout, "detected '%s'\n", KEYWORDS[keyword_index]);

            static const char *COLORS[] = {"yellow", "white", "red", "purple", "pink", "green", "blue", "orange", "off"};

            switch (keyword_index) {
                case 0:
                    set_color(YELLOW_RGB);
                    break;
                case 1:
                    set_color(WHITE_RGB);
                    break;
                case 2:
                    set_color(RED_RGB);
                    break;
                case 3:
                    set_color(PURPLE_RGB);
                    break;
                case 4:
                    set_color(PINK_RGB);
                    break;
                case 5:
                    set_color(GREEN_RGB);
                    break;
                case 6:
                    set_color(BLUE_RGB);
                    break;
                case 7:
                    set_color(ORANGE_RGB);
                    break;
                case 8:
                    set_color(OFF_RGB);
                    break;
            }
        }
    }

    free(pcm);
    snd_pcm_close(alsa_handle);
    pv_porcupine_delete_func(porcupine);
    dlclose(porcupine_library);
    close(spidev_fd);

    return 0;
}
