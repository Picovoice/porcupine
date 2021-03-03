/**
  ******************************************************************************
  * @file    stm32h747i_discovery.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for STM32H747I_DISCO:
  *          LEDs
  *          push-buttons
  *          COM ports
  *          hardware resources.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32H747I_DISCO_H
#define STM32H747I_DISCO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h747i_discovery_conf.h"
#include "stm32h747i_discovery_errno.h"

 #if (USE_BSP_COM_FEATURE > 0)
  #if (USE_COM_LOG > 0)
    #ifndef __GNUC__
      #include "stdio.h"
    #endif
  #endif
#endif
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32H747I_DISCO
  * @{
  */

/** @addtogroup STM32H747I_DISCO_LOW_LEVEL
  * @{
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_Exported_Types Exported Types
 * @{
 */

typedef enum
{
  LED1 = 0U,
  LED_GREEN = LED1,
  LED2 = 1U,
  LED_ORANGE = LED2,
  LED3 = 2U,
  LED_RED = LED3,
  LED4 = 3U,
  LED_BLUE = LED4,
  LEDn
} Led_TypeDef;

typedef enum
{
  BUTTON_WAKEUP = 0U,
  BUTTONn
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0U,
  BUTTON_MODE_EXTI = 1U
}ButtonMode_TypeDef;

#if (USE_BSP_COM_FEATURE > 0)
typedef enum
{
  COM1 = 0U,
  COMn
}COM_TypeDef;

typedef enum
{
  COM_STOPBITS_1     =   UART_STOPBITS_1,
  COM_STOPBITS_2     =   UART_STOPBITS_2,
}COM_StopBitsTypeDef;

typedef enum
{
  COM_PARITY_NONE     =  UART_PARITY_NONE,
  COM_PARITY_EVEN     =  UART_PARITY_EVEN,
  COM_PARITY_ODD      =  UART_PARITY_ODD,
}COM_ParityTypeDef;

typedef enum
{
  COM_HWCONTROL_NONE    =  UART_HWCONTROL_NONE,
  COM_HWCONTROL_RTS     =  UART_HWCONTROL_RTS,
  COM_HWCONTROL_CTS     =  UART_HWCONTROL_CTS,
  COM_HWCONTROL_RTS_CTS =  UART_HWCONTROL_RTS_CTS,
}COM_HwFlowCtlTypeDef;

typedef enum
{
  COM_WORDLENGTH_7B = UART_WORDLENGTH_7B,
  COM_WORDLENGTH_8B = UART_WORDLENGTH_8B,
  COM_WORDLENGTH_9B = UART_WORDLENGTH_9B,
}COM_WordLengthTypeDef;

typedef struct
{
  uint32_t              BaudRate;
  COM_WordLengthTypeDef WordLength;
  COM_StopBitsTypeDef   StopBits;
  COM_ParityTypeDef     Parity;
  COM_HwFlowCtlTypeDef  HwFlowCtl;
}COM_InitTypeDef;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
typedef struct
{
  void (* pMspInitCb)(UART_HandleTypeDef *);
  void (* pMspDeInitCb)(UART_HandleTypeDef *);
}BSP_COM_Cb_t;
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1) */
#endif

typedef enum
{
  JOY_MODE_GPIO = 0U,
  JOY_MODE_EXTI = 1U
}JOYMode_TypeDef;

typedef enum
{
  JOY1 = 0U,
  JOYn
}JOY_TypeDef;

typedef enum
{
 JOY_NONE  = 0x00U,
 JOY_SEL   = 0x01U,
 JOY_DOWN  = 0x02U,
 JOY_LEFT  = 0x04U,
 JOY_RIGHT = 0x08U,
 JOY_UP    = 0x10U,
 JOY_ALL   = 0x1FU
}JOYPin_TypeDef;
/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_Exported_Constants Exported Constants
  * @{
  */

/**
  * @brief  Define for STM32H747I_DISCO board
  */
#if !defined (USE_STM32H747I_DISCO)
 #define USE_STM32H747I_DISCO
#endif
/**
 * @brief STM32H747I Discovery BSP Driver version number V3.1.0
 */
#define STM32H747I_DISCO_BSP_VERSION_MAIN   (0x03) /*!< [31:24] main version */
#define STM32H747I_DISCO_BSP_VERSION_SUB1   (0x01) /*!< [23:16] sub1 version */
#define STM32H747I_DISCO_BSP_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define STM32H747I_DISCO_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define STM32H747I_DISCO_BSP_VERSION        ((STM32H747I_DISCO_BSP_VERSION_MAIN << 24)\
                                            |(STM32H747I_DISCO_BSP_VERSION_SUB1 << 16)\
                                            |(STM32H747I_DISCO_BSP_VERSION_SUB2 << 8 )\
                                            |(STM32H747I_DISCO_BSP_VERSION_RC))
#define STM32H747I_DISCO_BSP_BOARD_NAME  "STM32H747I-DISCO";
#define STM32H747I_DISCO_BSP_BOARD_ID    "MB12481D";

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_LED EVAL LOW LEVEL LED
  * @{
  */
#define LED1_GPIO_PORT                   GPIOI
#define LED1_PIN                         GPIO_PIN_12

#define LED2_GPIO_PORT                   GPIOI
#define LED2_PIN                         GPIO_PIN_13

#define LED3_GPIO_PORT                   GPIOI
#define LED3_PIN                         GPIO_PIN_14

#define LED4_GPIO_PORT                   GPIOI
#define LED4_PIN                         GPIO_PIN_15

#define LEDx_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOI_CLK_ENABLE()
#define LEDx_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOI_CLK_DISABLE()
/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_BUTTON LOW LEVEL BUTTON
  * @{
  */
/* Button state */
#define BUTTON_RELEASED                    0U
#define BUTTON_PRESSED                     1U
/**
  * @brief Wakeup push-button
  */
#define BUTTON_WAKEUP_PIN                   GPIO_PIN_13
#define BUTTON_WAKEUP_GPIO_PORT             GPIOC
#define BUTTON_WAKEUP_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define BUTTON_WAKEUP_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()
#define BUTTON_WAKEUP_EXTI_IRQn             EXTI15_10_IRQn
#define BUTTON_WAKEUP_EXTI_LINE             EXTI_LINE_13

/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_COM LOW LEVEL COM
  * @{
  */
#if (USE_BSP_COM_FEATURE > 0)
/**
 * @brief Definition for COM port1, connected to USART1
 */
#define COM1_UART                      USART1
#define COM1_CLK_ENABLE()             __HAL_RCC_USART1_CLK_ENABLE()
#define COM1_CLK_DISABLE()            __HAL_RCC_USART1_CLK_DISABLE()

#define COM1_TX_PIN                   GPIO_PIN_9
#define COM1_TX_GPIO_PORT             GPIOA
#define COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define COM1_TX_AF                    GPIO_AF7_USART1

#define COM1_RX_PIN                    GPIO_PIN_10
#define COM1_RX_GPIO_PORT              GPIOA
#define COM1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM1_RX_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOA_CLK_DISABLE()
#define COM1_RX_AF                    GPIO_AF7_USART1
#define COM_POLL_TIMEOUT              1000

#define MX_UART_InitTypeDef COM_InitTypeDef
#endif
/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_JOY LOW LEVEL JOY
  * @{
  */
#define JOY_KEY_NUMBER                     5U

#define JOY1_SEL_PIN                       GPIO_PIN_2
#define JOY1_SEL_GPIO_PORT                 GPIOK
#define JOY1_SEL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOK_CLK_ENABLE()
#define JOY1_SEL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOK_CLK_DISABLE()
#define JOY1_SEL_EXTI_IRQn                 EXTI2_IRQn
#define JOY1_SEL_EXTI_LINE                 EXTI_LINE_2

#define JOY1_DOWN_PIN                      GPIO_PIN_3
#define JOY1_DOWN_GPIO_PORT                GPIOK
#define JOY1_DOWN_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOK_CLK_ENABLE()
#define JOY1_DOWN_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOK_CLK_DISABLE()
#define JOY1_DOWN_EXTI_IRQn                EXTI3_IRQn
#define JOY1_DOWN_EXTI_LINE                EXTI_LINE_3

#define JOY1_LEFT_PIN                      GPIO_PIN_4
#define JOY1_LEFT_GPIO_PORT                GPIOK
#define JOY1_LEFT_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOK_CLK_ENABLE()
#define JOY1_LEFT_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOK_CLK_DISABLE()
#define JOY1_LEFT_EXTI_IRQn                EXTI4_IRQn
#define JOY1_LEFT_EXTI_LINE                EXTI_LINE_4

#define JOY1_RIGHT_PIN                     GPIO_PIN_5
#define JOY1_RIGHT_GPIO_PORT               GPIOK
#define JOY1_RIGHT_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOK_CLK_ENABLE()
#define JOY1_RIGHT_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOK_CLK_DISABLE()
#define JOY1_RIGHT_EXTI_IRQn               EXTI9_5_IRQn
#define JOY1_RIGHT_EXTI_LINE               EXTI_LINE_5

#define JOY1_UP_PIN                        GPIO_PIN_6
#define JOY1_UP_GPIO_PORT                  GPIOK
#define JOY1_UP_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOK_CLK_ENABLE()
#define JOY1_UP_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOK_CLK_DISABLE()
#define JOY1_UP_EXTI_IRQn                  EXTI9_5_IRQn
#define JOY1_UP_EXTI_LINE                  EXTI_LINE_6
/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup STM32H747I_DISCO_LOW_LEVEL_Exported_Variables
  * @{
  */
extern EXTI_HandleTypeDef hpb_exti[];
extern UART_HandleTypeDef hcom_uart[];
extern USART_TypeDef* COM_USART[];

/**
  * @}
  */

/** @addtogroup STM32H747I_DISCO_LOW_LEVEL_Exported_Functions
  * @{
  */
int32_t  BSP_GetVersion(void);
const uint8_t* BSP_GetBoardName(void);
const uint8_t* BSP_GetBoardID(void);

int32_t  BSP_LED_Init(Led_TypeDef Led);
int32_t  BSP_LED_DeInit(Led_TypeDef Led);
int32_t  BSP_LED_On(Led_TypeDef Led);
int32_t  BSP_LED_Off(Led_TypeDef Led);
int32_t  BSP_LED_Toggle(Led_TypeDef Led);
int32_t  BSP_LED_GetState (Led_TypeDef Led);
int32_t  BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t  BSP_PB_DeInit(Button_TypeDef Button);
int32_t  BSP_PB_GetState(Button_TypeDef Button);
void     BSP_PB_Callback(Button_TypeDef Button);
#if (USE_BSP_COM_FEATURE > 0)
int32_t  BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init);
int32_t  BSP_COM_DeInit(COM_TypeDef COM);
#if( USE_COM_LOG > 0)
int32_t  BSP_COM_SelectLogPort (COM_TypeDef COM);
#endif

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM);
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *Callback);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
HAL_StatusTypeDef MX_USART1_Init(UART_HandleTypeDef *huart, MX_UART_InitTypeDef *COM_Init);
#endif


int32_t  BSP_JOY_Init(JOY_TypeDef JOY, JOYMode_TypeDef JoyMode,  JOYPin_TypeDef JoyPins);
int32_t  BSP_JOY_DeInit(JOY_TypeDef JOY,  JOYPin_TypeDef JoyPins);
int32_t  BSP_JOY_GetState(JOY_TypeDef JOY,uint32_t JoyPin);
void     BSP_JOY_Callback(JOY_TypeDef JOY, uint32_t JoyPin);
void     BSP_JOY_IRQHandler(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin);

void BSP_JOY_IRQHandler(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin);
void BSP_PB_IRQHandler(Button_TypeDef Button);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32H747I_DISCO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
