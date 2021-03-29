/*
    Copyright 2021-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#ifndef STM32F4xx_IT_H
#define STM32F4xx_IT_H

#include "stm32469i_discovery_audio.h"

void AUDIO_I2Sx_DMAx_IRQHandler(void);
void BusFault_Handler(void);
void DebugMon_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void NMI_Handler(void);
void PendSV_Handler(void);
void SVC_Handler(void);
void SysTick_Handler(void);
void UsageFault_Handler(void);

#endif // STM32F4xx_IT_H
