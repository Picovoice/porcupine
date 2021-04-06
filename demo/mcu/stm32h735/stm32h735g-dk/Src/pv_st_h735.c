/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <stdbool.h>
#include <string.h>

#include "stm32h7xx_hal.h"

#include "main.h"
#include "pv_st_h735.h"

#define UUID_ADDRESS    (0x1FF1E800)
#define UUID_SIZE       (12)

#define PV_COM                          (USART3)
#define PV_COM_ALT                      (GPIO_AF7_USART3)
#define PV_COM_IRQn                     (USART3_IRQn)
#define PV_COM_TX_Pin                   (GPIO_PIN_8)
#define PV_COM_TX_GPIO_Port             (GPIOD)
#define PV_COM_RX_Pin                   (GPIO_PIN_9)
#define PV_COM_RX_GPIO_Port             (GPIOD)

static uint8_t uuid[UUID_SIZE];

UART_HandleTypeDef huart;


static void CPU_CACHE_Enable(void) {
  SCB_EnableICache();
  SCB_EnableDCache();
}

static pv_status_t pv_clock_config(void) {

     RCC_OscInitTypeDef RCC_OscInitStruct = {0};
     RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
     HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
     __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
     while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
     __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
     RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
     RCC_OscInitStruct.HSEState = RCC_HSE_ON;
     RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
     RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
     RCC_OscInitStruct.PLL.PLLM = 5;
     RCC_OscInitStruct.PLL.PLLN = 110;
     RCC_OscInitStruct.PLL.PLLP = 1;
     RCC_OscInitStruct.PLL.PLLQ = 4;
     RCC_OscInitStruct.PLL.PLLR = 2;
     RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
     RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
     RCC_OscInitStruct.PLL.PLLFRACN = 0;
     if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
          pv_error_handler();
     }
     RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                     |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                     |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
     RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
     RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
     RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
     RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
     RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
     RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
     RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
     if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)     {
          pv_error_handler();
     }

     __HAL_RCC_CSI_ENABLE() ;
     __HAL_RCC_SYSCFG_CLK_ENABLE() ;
     HAL_EnableCompensationCell();

    return PV_STATUS_SUCCESS;
}

const uint8_t *pv_get_uuid(void) {
    return (const uint8_t *) uuid;
}

const uint32_t pv_get_uuid_size(void) {
    return UUID_SIZE;
}

void pv_board_deinit() {
}

static void PeriphCommonClock_Config(void) {
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI4A|RCC_PERIPHCLK_SAI1;
  PeriphClkInitStruct.PLL2.PLL2M = 25;
  PeriphClkInitStruct.PLL2.PLL2N = 344;
  PeriphClkInitStruct.PLL2.PLL2P = 7;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL2;
  PeriphClkInitStruct.Sai4AClockSelection = RCC_SAI4ACLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
       pv_error_handler();
  }
}

static void MX_GPIO_Init(void)
{
     __HAL_RCC_GPIOE_CLK_ENABLE();
     __HAL_RCC_GPIOD_CLK_ENABLE();
     __HAL_RCC_GPIOH_CLK_ENABLE();

}

static void MPU_Config(void)
{
     MPU_Region_InitTypeDef MPU_InitStruct = {0};
     HAL_MPU_Disable();
     MPU_InitStruct.Enable = MPU_REGION_ENABLE;
     MPU_InitStruct.Number = MPU_REGION_NUMBER0;
     MPU_InitStruct.BaseAddress = 0x24000000;
     MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
     MPU_InitStruct.SubRegionDisable = 0x0;
     MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
     MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
     MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
     MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
     MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
     MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

     HAL_MPU_ConfigRegion(&MPU_InitStruct);

     MPU_InitStruct.Number = MPU_REGION_NUMBER1;
     MPU_InitStruct.BaseAddress = 0x38000000;
     MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
     MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;

     HAL_MPU_ConfigRegion(&MPU_InitStruct);
     HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

static pv_status_t pv_uart_init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = PV_COM_TX_Pin | PV_COM_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = PV_COM_ALT;
    HAL_GPIO_Init(PV_COM_TX_GPIO_Port, &GPIO_InitStruct);

    huart.Instance = PV_COM;
    huart.Init.BaudRate = 115200;
    huart.Init.WordLength = UART_WORDLENGTH_8B;
    huart.Init.StopBits = UART_STOPBITS_1;
    huart.Init.Parity = UART_PARITY_NONE;
    huart.Init.Mode = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart) != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    if (HAL_UARTEx_DisableFifoMode(&huart) != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    HAL_NVIC_SetPriority(PV_COM_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(PV_COM_IRQn);
    return PV_STATUS_SUCCESS;
}

pv_status_t pv_message_init(void) {
    if (pv_uart_init() != PV_STATUS_SUCCESS) {
        return PV_STATUS_INVALID_STATE;
    }
     return PV_STATUS_SUCCESS;
}

int __io_putchar (int ch) {
     HAL_UART_Transmit(&huart, (uint8_t *) &ch, 1, 1000);
     return ch;
}

pv_status_t pv_board_init() {
     MPU_Config();
     CPU_CACHE_Enable();
     HAL_Init();
     pv_clock_config();
     PeriphCommonClock_Config();

     MX_GPIO_Init();

    memcpy(uuid, (uint8_t *) UUID_ADDRESS, UUID_SIZE);
    return PV_STATUS_SUCCESS;
}

void pv_error_handler(void) {
     __disable_irq();
    while(true);
}

void assert_failed(uint8_t* file, uint32_t line)
{
    (void) file;
    (void) line;
    pv_error_handler();
}
