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

#include "stm32469i_discovery.h"

#include "pv_stm32f469.h"

#define UUID_ADDRESS (0x1FFF7A10)
#define UUID_SIZE (12)

#define PV_COM                          (USART3)
#define PV_COM_ALT                      (GPIO_AF7_USART3)
#define PV_COM_IRQn                     (USART3_IRQn)
#define PV_COM_TX_Pin                   (GPIO_PIN_11)
#define PV_COM_TX_GPIO_Port             (GPIOB)
#define PV_COM_RX_Pin                   (GPIO_PIN_10)
#define PV_COM_RX_GPIO_Port             (GPIOB)

static uint8_t uuid[UUID_SIZE];
UART_HandleTypeDef huart;

static pv_status_t pv_clock_config(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN       = 360;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 7;
    RCC_OscInitStruct.PLL.PLLR       = 6;
    if ( HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    if ( HAL_PWREx_EnableOverDrive() != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    RCC_ClkInitStruct.ClockType =  (RCC_CLOCKTYPE_SYSCLK |
            RCC_CLOCKTYPE_HCLK   |
            RCC_CLOCKTYPE_PCLK1  |
            RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    ;
    if ( HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    return PV_STATUS_SUCCESS;
}

static pv_status_t pv_uart_init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
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
    return pv_uart_init();
}

const uint8_t *pv_get_uuid(void) {
    return (const uint8_t *) uuid;
}

const uint32_t pv_get_uuid_size(void) {
    return UUID_SIZE;
}

pv_status_t pv_board_init() {
    if (HAL_Init() != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    if (pv_clock_config() != PV_STATUS_SUCCESS) {
        return PV_STATUS_INVALID_STATE;
    }
    memcpy(uuid, (uint8_t *) UUID_ADDRESS, UUID_SIZE);

    BSP_LED_Init(LED1);
    BSP_LED_Init(LED2);
    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);

    return PV_STATUS_SUCCESS;
}

void pv_board_deinit() {
}

static void pv_error_handler(void) {
    while(true);
}

void assert_failed(uint8_t* file, uint32_t line)
{
    (void) file;
    (void) line;
    pv_error_handler();
}

int __io_putchar (int ch) {
    HAL_UART_Transmit(&huart, (uint8_t *) &ch, 1, 1000);
    return ch;
}

