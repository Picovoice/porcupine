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

#include "stm32f769i_discovery.h"

#include "pv_st_f769.h"

#define UUID_ADDRESS (0x1FF0F420)
#define UUID_SIZE    (12)


#define PV_COM              (USART1)
#define PV_COM_ALT          (GPIO_AF7_USART1)
#define PV_COM_IRQn         (USART1_IRQn)
#define PV_COM_TX_Pin       (GPIO_PIN_10)
#define PV_COM_TX_GPIO_Port (GPIOA)
#define PV_COM_RX_Pin       (GPIO_PIN_9)
#define PV_COM_RX_GPIO_Port (GPIOA)

static uint8_t uuid[UUID_SIZE];
UART_HandleTypeDef huart;

static pv_status_t pv_clock_config(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 400;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    RCC_OscInitStruct.PLL.PLLR = 7;
    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if (ret != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    ret = HAL_PWREx_EnableOverDrive();
    if (ret != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
    if (ret != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    return PV_STATUS_SUCCESS;
}

static pv_status_t pv_uart_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
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

const uint8_t *pv_get_uuid(void) {
    return (const uint8_t *) uuid;
}

const uint32_t pv_get_uuid_size(void) {
    return UUID_SIZE;
}

pv_status_t pv_board_init() {
    SCB_EnableICache();
    SCB_EnableDCache();
    if (HAL_Init() != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    if (pv_clock_config() != PV_STATUS_SUCCESS) {
        return PV_STATUS_INVALID_STATE;
    }

    BSP_LED_Init(LED1);
    BSP_LED_Init(LED2);

    memcpy(uuid, (uint8_t *) UUID_ADDRESS, UUID_SIZE);
    return PV_STATUS_SUCCESS;
}

void pv_board_deinit() {
}

void pv_error_handler(void) {
    while (true) {}
}

void assert_failed(uint8_t *file, uint32_t line) {
    (void) file;
    (void) line;
    pv_error_handler();
}

int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart, (uint8_t *) &ch, 1, 1000);
    return ch;
}
