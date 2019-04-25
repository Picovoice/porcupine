/*
    Copyright 2018 Picovoice Inc.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

            http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef PV_PORCUPINE_H
#define PV_PORCUPINE_H

#include <stdbool.h>
#include <stdint.h>

#include "picovoice.h"

#ifdef __cplusplus

extern "C"
{

#endif

/**
 * Forward declaration for keyword spotting object (a.k.a. Porcupine).
 * The object detects utterances of a given keyword(s) within incoming stream of audio in real-time.
 * It processes incoming audio in consecutive frames (chucks) and for each frame emits result of detecting the
 * keyword(s) ending at that frame. The number of samples per frame can be attained by calling
 * 'pv_porcupine_frame_length()'. The incoming audio needs to have a sample rate equal to 'pv_sample_rate()' and be
 * 16-bit linearly-encoded. Furthermore, Porcupine operates on single channel audio.
 */
typedef struct pv_porcupine_object pv_porcupine_object_t;

/**
 * Constructor.
 *
 * @param model_file_path Absolute path to file containing model parameters.
 * @param keyword_file_path Absolute path to keyword file.
 * @param sensitivity Sensitivity for detecting keyword. It should be a floating-point number within
 * [0, 1]. A higher sensitivity value results in fewer misses at the cost of increasing the false alarm rate.
 * @param[out] object Constructed keyword spotting object.
 * @return Status code. Returns 'PV_STATUS_INVALID_ARGUMENT', 'PV_STATUS_OUT_OF_MEMORY', or 'PV_STATUS_IO_ERROR' on
 * failure.
 */
PV_API pv_status_t pv_porcupine_init(
        const char *model_file_path,
        const char *keyword_file_path,
        float sensitivity,
        pv_porcupine_object_t **object);

/**
 * Constructor for multiple keyword detection.
 *
 * @param model_file_path Absolute path to file containing model parameters.
 * @param number_keywords Number of different keywords to monitor for.
 * @param keyword_file_paths Absolute paths to keyword files.
 * @param sensitivities Sensitivities for detecting different keywords. For more information about this parameter refer
 * to documentation of 'pv_porcupine_init()'.
 * @param[out] object Constructed keyword spotting object.
 * @return Status code. Returns 'PV_STATUS_INVALID_ARGUMENT', 'PV_STATUS_OUT_OF_MEMORY', or 'PV_STATUS_IO_ERROR' on
 * failure.
 */
PV_API pv_status_t pv_porcupine_multiple_keywords_init(
        const char *model_file_path,
        int32_t number_keywords,
        const char *const *keyword_file_paths,
        const float *sensitivities,
        pv_porcupine_object_t **object);

/**
 * Destructor.
 *
 * @param object Keyword spotting object.
 */
PV_API void pv_porcupine_delete(pv_porcupine_object_t *object);

/**
 * Monitors incoming audio stream for a given keyword.
 *
 * @param object Keyword spotting object.
 * @param pcm A frame of audio samples. The number of samples per frame can be attained by calling
 * 'pv_porcupine_frame_length()'. The incoming audio needs to have a sample rate equal to 'pv_sample_rate()' and be
 * 16-bit linearly-encoded. Furthermore, porcupine operates on single channel audio.
 * @param[out] result Flag indicating if the keyword has been observed ending at the current frame.
 * @return Status code. Returns 'PV_STATUS_INVALID_ARGUMENT' or 'PV_STATUS_OUT_OF_MEMORY' on failure.
 */
PV_API pv_status_t pv_porcupine_process(pv_porcupine_object_t *object, const int16_t *pcm, bool *result);

/**
 * Monitors incoming audio stream for multiple keywords.
 *
 * @param object Keyword spotting object.
 * @param pcm A frame of audio samples. For more information about required audio properties refer to documentation of
 * 'pv_porcupine_process()'.
 * @param[out] keyword_index Index of observed keyword at the end of current frame. Indexing is 0-based and matches the
 * ordering of 'keyword_file_paths' passed to 'pv_porcupine_multiple_keywords_init()'. If no keyword is detected it is
 * set to -1.
 * @return Status code. Returns 'PV_STATUS_INVALID_ARGUMENT' or 'PV_STATUS_OUT_OF_MEMORY' on failure.
 */
PV_API pv_status_t pv_porcupine_multiple_keywords_process(
        pv_porcupine_object_t *object,
        const int16_t *pcm,
        int32_t *keyword_index);

/**
 * Getter for version string.
 *
 * @return Version.
 */
PV_API const char *pv_porcupine_version(void);

/**
 * Getter for length (number of audio samples) per frame.
 *
 * @return frame length.
 */
PV_API int32_t pv_porcupine_frame_length(void);

#ifdef __cplusplus

}

#endif

#endif // PV_PORCUPINE_H
