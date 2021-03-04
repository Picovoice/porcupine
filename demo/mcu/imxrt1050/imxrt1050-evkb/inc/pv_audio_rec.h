/*
    Copyright 2020-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#ifndef PV_AUDIO_REC_H
#define PV_AUDIO_REC_H

#include "picovoice.h"

pv_status_t pv_audio_rec_init(void);
pv_status_t pv_audio_rec_start(void);
const int16_t *pv_audio_rec_get_new_buffer(void);
void pv_audio_rec_deinit(void);

#endif // PV_AUDIO_REC_H
