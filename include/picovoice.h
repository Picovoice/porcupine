/*
    Copyright 2018-2022 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of
   the license is located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
   WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
   License for the specific language governing permissions and limitations under
   the License.
*/

#ifndef PICOVOICE_H
#define PICOVOICE_H

#include <stdint.h>

#ifdef __cplusplus

extern "C" {

#endif

#define PV_API __attribute__((visibility("default")))

/**
 * Audio sample rate accepted by Picovoice.
 */
PV_API int32_t pv_sample_rate(void);

/**
 * Status codes.
 */
typedef enum {
    PV_STATUS_SUCCESS = 0,
    PV_STATUS_OUT_OF_MEMORY,
    PV_STATUS_IO_ERROR,
    PV_STATUS_INVALID_ARGUMENT,
    PV_STATUS_STOP_ITERATION,
    PV_STATUS_KEY_ERROR,
    PV_STATUS_INVALID_STATE,
    PV_STATUS_RUNTIME_ERROR,
    PV_STATUS_ACTIVATION_ERROR,
    PV_STATUS_ACTIVATION_LIMIT_REACHED,
    PV_STATUS_ACTIVATION_THROTTLED,
    PV_STATUS_ACTIVATION_REFUSED
} pv_status_t;

/**
 * Provides string representations of status codes.
 *
 * @param status Status code.
 * @return String representation.
 */
PV_API const char *pv_status_to_string(pv_status_t status);

#ifdef __cplusplus
}

#endif

#endif // PICOVOICE_H
