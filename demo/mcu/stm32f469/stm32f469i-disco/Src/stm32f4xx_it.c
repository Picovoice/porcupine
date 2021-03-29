/*
    Copyright 2020-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include "stm32f4xx_it.h"

extern I2S_HandleTypeDef haudio_in_i2s;
extern UART_HandleTypeDef huart;

void NMI_Handler(void) {
}

void SVC_Handler(void) {
}

void DebugMon_Handler(void) {
}

void PendSV_Handler(void) {
}

void AUDIO_I2Sx_DMAx_IRQHandler(void) {
  HAL_DMA_IRQHandler(haudio_in_i2s.hdmarx);
}

void SysTick_Handler(void) {
    HAL_IncTick();
}

void HardFault_Handler(void) {
  while (1);
}

void MemManage_Handler(void) {
  while (1);
}

void BusFault_Handler(void) {
  while (1);
}

void UsageFault_Handler(void) {
  while (1);
}
