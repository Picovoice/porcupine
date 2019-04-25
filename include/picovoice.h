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

#ifndef PICOVOICE_H
#define PICOVOICE_H

#ifdef __cplusplus

extern "C"
{

#endif

#define PV_API __attribute__((visibility ("default")))

/**
 * Audio sample rate accepted by Picovoice.
 */
PV_API int pv_sample_rate(void);

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
} pv_status_t;

/**
 * Provides string representations of status codes.
 *
 * @param status Status code.
 * @return String representation.
 */
const char *pv_status_to_string(pv_status_t status);

#ifdef __cplusplus

}

#endif

#endif // PICOVOICE_H
