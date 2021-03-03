/*
    Copyright 2020-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include "stm32h7xx_it.h"

extern UART_HandleTypeDef huart;

void NMI_Handler(void) {
}

void HardFault_Handler(void) {
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void) {
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

void BusFault_Handler(void) {
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void) {
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void) {
}

void DebugMon_Handler(void) {
}

void PendSV_Handler(void) {
}

void SysTick_Handler(void) {
  HAL_IncTick();
}

void AUDIO_IN_SAIx_DMAx_IRQHandler() {
  BSP_AUDIO_IN_IRQHandler(0,AUDIO_IN_DEVICE_DIGITAL_MIC);
}

void AUDIO_IN_SAI_PDMx_DMAx_IRQHandler(void) {
  BSP_AUDIO_IN_IRQHandler(1, AUDIO_IN_DEVICE_DIGITAL_MIC);
}
