/*
    Copyright 2020-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <stdbool.h>
#include <string.h>

#include "stm32f4_discovery.h"

#include "pv_st_f407.h"

#define UUID_ADDRESS (0x1FFF7A10)
#define UUID_SIZE    (12)

static uint8_t uuid[UUID_SIZE];

static pv_status_t pv_clock_config(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    if (HAL_GetREVID() == 0x1001) {
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
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
    if (HAL_Init() != HAL_OK) {
        return PV_STATUS_INVALID_STATE;
    }
    if (pv_clock_config() != PV_STATUS_SUCCESS) {
        return PV_STATUS_INVALID_STATE;
    }

    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);
    BSP_LED_Init(LED5);
    BSP_LED_Init(LED6);

    memcpy(uuid, (uint8_t *) UUID_ADDRESS, UUID_SIZE);
    return PV_STATUS_SUCCESS;
}

void pv_board_deinit() {
}

void pv_error_handler(void) {
    __disable_irq();
    while (true) {}
}

void assert_failed(uint8_t *file, uint32_t line) {
    (void) file;
    (void) line;
    pv_error_handler();
}

int __io_putchar(uint8_t ch) {
    ITM_SendChar(ch);
    return 0;
}
