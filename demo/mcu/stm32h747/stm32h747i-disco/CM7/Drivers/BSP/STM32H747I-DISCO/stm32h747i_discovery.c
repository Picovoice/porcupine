/**
  ******************************************************************************
  * @file    stm32h747i_discovery.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage
  *          LEDs
  *          push-buttons
  *          COM ports
  *          available on STM32H747I-DISCO board(MB1248) from STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32h747i_discovery.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32H747I_DISCO
  * @{
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL LOW LEVEL
  * @{
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_Private_TypesDefinitions  Private Types Definitions
  * @{
  */
typedef void (* BSP_EXTI_LineCallback) (void);
/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_Private_FunctionPrototypes  Private Function Prototypes
  * @{
  */
 static void BUTTON_WAKEUP_EXTI_Callback(void);
 #if (USE_BSP_COM_FEATURE > 0)
static void USART1_MspInit(UART_HandleTypeDef *huart);
static void USART1_MspDeInit(UART_HandleTypeDef *huart);
#endif
static void JOY1_UP_EXTI_Callback(void);
static void JOY1_DOWN_EXTI_Callback(void);
static void JOY1_LEFT_EXTI_Callback(void);
static void JOY1_RIGHT_EXTI_Callback(void);
static void JOY1_SEL_EXTI_Callback(void);
/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_Exported_Variables Exported Variables
  * @{
  */
EXTI_HandleTypeDef hpb_exti[BUTTONn];
#if (USE_BSP_COM_FEATURE > 0)
UART_HandleTypeDef hcom_uart[COMn];
USART_TypeDef* COM_USART[COMn]   = {COM1_UART};
#endif
/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_Private_Variables Private Variables
  * @{
  */

static GPIO_TypeDef* LED_PORT[LEDn] = { LED1_GPIO_PORT,
                                        LED2_GPIO_PORT,
                                        LED3_GPIO_PORT,
                                        LED4_GPIO_PORT};

static const uint32_t LED_PIN[LEDn] = { LED1_PIN,
                                        LED2_PIN,
                                        LED3_PIN,
                                        LED4_PIN};

static GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {BUTTON_WAKEUP_GPIO_PORT};
static const uint16_t BUTTON_PIN[BUTTONn] = {BUTTON_WAKEUP_PIN};
static const IRQn_Type BUTTON_IRQn[BUTTONn] = {BUTTON_WAKEUP_EXTI_IRQn};

#if (USE_COM_LOG > 0)
static COM_TypeDef COM_ActiveLogPort = COM1;
#endif
#if (USE_BSP_COM_FEATURE > 0)
 #if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  static uint32_t IsComMspCbValid[COMn] = {0};
 #endif
#endif

static uint32_t JoyPinsMask;
static GPIO_TypeDef* JOY1_PORT[JOY_KEY_NUMBER] = {JOY1_SEL_GPIO_PORT,
                                                  JOY1_DOWN_GPIO_PORT,
                                                  JOY1_LEFT_GPIO_PORT,
                                                  JOY1_RIGHT_GPIO_PORT,
                                                  JOY1_UP_GPIO_PORT
                                                 };

static const uint16_t JOY1_PIN[JOY_KEY_NUMBER] = {JOY1_SEL_PIN,
                                                  JOY1_DOWN_PIN,
                                                  JOY1_LEFT_PIN,
                                                  JOY1_RIGHT_PIN,
                                                  JOY1_UP_PIN
                                                 };

static const IRQn_Type JOY1_IRQn[JOY_KEY_NUMBER] = {JOY1_SEL_EXTI_IRQn,
                                                    JOY1_DOWN_EXTI_IRQn,
                                                    JOY1_LEFT_EXTI_IRQn,
                                                    JOY1_RIGHT_EXTI_IRQn,
                                                    JOY1_UP_EXTI_IRQn
                                                   };
static EXTI_HandleTypeDef hjoy_exti[JOY_KEY_NUMBER];
/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_Exported_Functions Exported Functions
  * @{
  */

  /**
  * @brief  This method returns the STM32H747I DISCO BSP Driver revision
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
int32_t BSP_GetVersion(void)
{
  return (int32_t)STM32H747I_DISCO_BSP_VERSION;
}

/**
  * @brief  This method returns the board name
  * @retval pointer to the board name string
  */
const uint8_t* BSP_GetBoardName(void)
{
  return (uint8_t*)STM32H747I_DISCO_BSP_BOARD_NAME;
}

/**
  * @brief  This method returns the board ID
  * @retval pointer to the board name string
  */
const uint8_t* BSP_GetBoardID(void)
{
  return (uint8_t*)STM32H747I_DISCO_BSP_BOARD_ID;
}

/**
  * @brief  Configures LED on GPIO.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t  BSP_LED_Init(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable the GPIO_LED clock */
  LEDx_GPIO_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = LED_PIN[Led];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(LED_PORT[Led], &GPIO_InitStruct);

  /* By default, turn off LED */
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
 return ret;
}

/**
  * @brief  DeInit LEDs.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t  BSP_LED_DeInit(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef  gpio_init_structure;

  /* DeInit the GPIO_LED pin */
  gpio_init_structure.Pin = LED_PIN [Led];
  /* Turn off LED */
  HAL_GPIO_WritePin (LED_PORT [Led], (uint16_t)LED_PIN[Led], GPIO_PIN_SET);
  HAL_GPIO_DeInit (LED_PORT [Led], gpio_init_structure.Pin);
  return ret;
}

/**
  * @brief  Turns selected LED On.
  * @param  Led LED to be set on
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t  BSP_LED_On(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;

  HAL_GPIO_WritePin (LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_PIN_RESET);
  return ret;
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t  BSP_LED_Off(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  HAL_GPIO_WritePin (LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_PIN_SET);
  return ret;
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led LED to be toggled
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t  BSP_LED_Toggle(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  HAL_GPIO_TogglePin(LED_PORT[Led], (uint16_t)LED_PIN[Led]);
  return ret;
}
/**
  * @brief  Get the selected LED state.
  * @param  Led LED to be get its state
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval LED status
  */
int32_t BSP_LED_GetState (Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  ret = (int32_t)HAL_GPIO_ReadPin (LED_PORT [Led], (uint16_t)LED_PIN [Led]);
  return ret;
}
/**
  * @brief  Configures button GPIO and EXTI Line.
  * @param  Button Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  * @param  ButtonMode Button mode
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
  *            @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line
  *                                    with interrupt generation capability
  * @retval BSP status
  */
int32_t  BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef gpio_init_structure;
  static BSP_EXTI_LineCallback ButtonCallback[BUTTONn] ={BUTTON_WAKEUP_EXTI_Callback};
  static uint32_t  BSP_BUTTON_PRIO [BUTTONn] ={BSP_BUTTON_WAKEUP_IT_PRIORITY};
  static const uint32_t BUTTON_EXTI_LINE[BUTTONn] ={BUTTON_WAKEUP_EXTI_LINE};
  /* Enable the BUTTON clock*/
  BUTTON_WAKEUP_GPIO_CLK_ENABLE();
  gpio_init_structure.Pin = BUTTON_PIN [Button];
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

  if(ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(BUTTON_PORT [Button], &gpio_init_structure);
  }
  else /* (ButtonMode == BUTTON_MODE_EXTI) */
  {
    /* Configure Button pin as input with External interrupt */
    gpio_init_structure.Mode = GPIO_MODE_IT_RISING;

    HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);

    (void)HAL_EXTI_GetHandle(&hpb_exti[Button], BUTTON_EXTI_LINE[Button]);
    (void)HAL_EXTI_RegisterCallback(&hpb_exti[Button],  HAL_EXTI_COMMON_CB_ID, ButtonCallback[Button]);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((BUTTON_IRQn[Button]), BSP_BUTTON_PRIO[Button], 0x00);
    HAL_NVIC_EnableIRQ((BUTTON_IRQn[Button]));
  }
  return BSP_ERROR_NONE;
}

/**
  * @brief  Push Button DeInit.
  * @param  Button Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  * @note PB DeInit does not disable the GPIO clock
  * @retval BSP status
  */
int32_t  BSP_PB_DeInit(Button_TypeDef Button)
{
    GPIO_InitTypeDef gpio_init_structure;

    gpio_init_structure.Pin = BUTTON_PIN[Button];
    HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
    HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);

    return BSP_ERROR_NONE;
}

/**
  * @brief  Returns the selected button state.
  * @param  Button Button to be checked
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  * @retval The Button GPIO pin value
  */
int32_t  BSP_PB_GetState(Button_TypeDef Button)
{
  return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/**
  * @brief  This function handles Push-Button interrupt requests.
  * @param  Button Specifies the pin connected EXTI line
  * @retval None
  */
void BSP_PB_IRQHandler(Button_TypeDef Button)
{
  HAL_EXTI_IRQHandler(&hpb_exti[Button]);
}

/**
  * @brief  KEY EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_WAKEUP_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_WAKEUP);
}
/**
  * @brief  BSP Push Button callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None.
  */
__weak void BSP_PB_Callback(Button_TypeDef Button)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Button);
  /* This function should be implemented by the user application.
     It is called into this driver when an event on Button is triggered. */
}

#if (USE_BSP_COM_FEATURE > 0)
/**
  * @brief  Configures COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be COM1
  * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
  *                configuration information for the specified USART peripheral.
  * @retval BSP status
  */
int32_t BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Init the UART Msp */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    USART1_MspInit(&hcom_uart[COM]);
#else
    if(IsComMspCbValid[COM] == 0U)
    {
      if(BSP_COM_RegisterDefaultMspCallbacks(COM) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif

    if(MX_USART1_Init(&hcom_uart[COM], COM_Init) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  DeInit COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be (COM1)
  * @retval BSP status
  */
int32_t BSP_COM_DeInit(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* USART configuration */
    hcom_uart[COM].Instance = COM_USART[COM];

#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    USART1_MspDeInit(&hcom_uart[COM]);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 0) */

    if(HAL_UART_DeInit(&hcom_uart[COM]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  Configures COM1 port.
  * @param  huart USART handle
  * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
  *                  configuration information for the specified USART peripheral.
  * @retval HAL error code
  */
__weak HAL_StatusTypeDef MX_USART1_Init(UART_HandleTypeDef *huart, MX_UART_InitTypeDef *COM_Init)
{
  /* USART configuration */
  huart->Instance          = COM_USART[COM1];
  huart->Init.BaudRate     = (uint32_t)COM_Init->BaudRate;
  huart->Init.Mode         = UART_MODE_TX_RX;
  huart->Init.Parity       = (uint32_t)COM_Init->Parity;
  huart->Init.WordLength   = (uint32_t)COM_Init->WordLength;
  huart->Init.StopBits     = (uint32_t)COM_Init->StopBits;
  huart->Init.HwFlowCtl    = (uint32_t)COM_Init->HwFlowCtl;
  huart->Init.OverSampling = UART_OVERSAMPLING_8;
  
  return HAL_UART_Init(huart);
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
/**
  * @brief Register Default COM Msp Callbacks
  * @param  COM COM port to be configured.
  * @retval BSP status
  */
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM]);

    /* Register default MspInit/MspDeInit Callback */
    if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, USART1_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, USART1_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }

  /* BSP status */
  return ret;
}

/**
  * @brief  Register COM1 Msp Callback registering
  * @param  COM COM port to be configured.
  * @param  Callbacks     pointer to COM MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM]);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, Callback->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, Callback->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }

  /* BSP status */
  return ret;
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

#if (USE_COM_LOG > 0)
/**
  * @brief  Select the active COM port.
  * @param  COM COM port to be activated.
  *          This parameter can be COM1 or/and COM2
  * @retval BSP status
  */
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM)
{
  if(COM_ActiveLogPort != COM)
  {
    COM_ActiveLogPort = COM;
  }
  return BSP_ERROR_NONE;
}

 #ifdef __GNUC__
 int __io_putchar (int ch)
 #else
 int fputc (int ch, FILE *f)
 #endif /* __GNUC__ */
{
  (void)HAL_UART_Transmit (&hcom_uart [COM_ActiveLogPort], (uint8_t *) &ch, 1, COM_POLL_TIMEOUT);
  return ch;
}
#endif /* USE_COM_LOG */
#endif /* USE_BSP_COM_FEATURE */

/**
  * @brief  Configures joystick GPIO and EXTI modes.
  * @param  JOY Joystick to be initialized
  * @param  JoyMode Button mode.
  *          This parameter can be one of the following values:
  *            @arg  JOY_MODE_GPIO: Joystick pins will be used as simple IOs
  *            @arg  JOY_MODE_EXTI: Joystick pins will be connected to EXTI line
  *                                 with interrupt generation capability
  * @param  JoyPins joystick pins to be initialized
  * @retval BSP status
  */
int32_t BSP_JOY_Init(JOY_TypeDef JOY, JOYMode_TypeDef JoyMode, JOYPin_TypeDef JoyPins)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t joykey, key_pressed;
  GPIO_InitTypeDef gpio_init_structure;

  static BSP_EXTI_LineCallback JoyCallback[JOY_KEY_NUMBER] = {JOY1_SEL_EXTI_Callback,
                                                              JOY1_DOWN_EXTI_Callback,
                                                              JOY1_LEFT_EXTI_Callback,
                                                              JOY1_RIGHT_EXTI_Callback,
                                                              JOY1_UP_EXTI_Callback
                                                             };
  static const uint32_t JOY_EXTI_LINE[JOY_KEY_NUMBER]   = {JOY1_SEL_EXTI_LINE,
                                                           JOY1_DOWN_EXTI_LINE,
                                                           JOY1_LEFT_EXTI_LINE,
                                                           JOY1_RIGHT_EXTI_LINE,
                                                           JOY1_UP_EXTI_LINE
                                                          };
  static uint32_t  BSP_JOY_PRIO [JOY_KEY_NUMBER] ={BSP_JOY1_SEL_IT_PRIORITY,
                                                   BSP_JOY1_DOWN_IT_PRIORITY,
                                                   BSP_JOY1_LEFT_IT_PRIORITY,
                                                   BSP_JOY1_RIGHT_IT_PRIORITY,
                                                   BSP_JOY1_UP_IT_PRIORITY
                                                  };

  /* Store Joystick pins initialized */
  JoyPinsMask |= (uint32_t)JoyPins;

  /* Initialized the Joystick. */
  for(joykey = 0U; joykey < JOY_KEY_NUMBER ; joykey++)
  {
    key_pressed = 1UL << joykey;
    if((key_pressed & (uint32_t)JoyPins) == key_pressed)
    {
      if(JOY == JOY1)
      {
      /* Enable the JOY clock */
        if(key_pressed == (uint32_t)JOY_SEL)
        {
          JOY1_SEL_GPIO_CLK_ENABLE();
        }
        else if(key_pressed == (uint32_t)JOY_DOWN)
        {
          JOY1_DOWN_GPIO_CLK_ENABLE();
        }
        else if(key_pressed == (uint32_t) JOY_LEFT)
        {
          JOY1_LEFT_GPIO_CLK_ENABLE();
        }
        else if(key_pressed == (uint32_t) JOY_RIGHT)
        {
          JOY1_RIGHT_GPIO_CLK_ENABLE();
        }
        else
        {
          if(key_pressed == (uint32_t) JOY_UP)
          {
            JOY1_UP_GPIO_CLK_ENABLE();
          }
        }

        gpio_init_structure.Pin = JOY1_PIN[joykey];
        gpio_init_structure.Pull = GPIO_PULLUP;
        gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

        if (JoyMode == JOY_MODE_GPIO)
        {
          /* Configure Joy pin as input */
          gpio_init_structure.Mode = GPIO_MODE_INPUT;
          HAL_GPIO_Init(JOY1_PORT[joykey], &gpio_init_structure);
        }
        else /*(JoyMode == JOY_MODE_EXTI)*/
        {
          /* Configure Joy pin as input with External interrupt */
          gpio_init_structure.Mode = GPIO_MODE_IT_RISING_FALLING;
          HAL_GPIO_Init(JOY1_PORT[joykey], &gpio_init_structure);

         (void)HAL_EXTI_GetHandle(&hjoy_exti[joykey], JOY_EXTI_LINE[joykey]);
         (void)HAL_EXTI_RegisterCallback(&hjoy_exti[joykey],  HAL_EXTI_COMMON_CB_ID, JoyCallback[joykey]);

          /* Enable and set Joy EXTI Interrupt to the lowest priority */
          HAL_NVIC_SetPriority((IRQn_Type)(JOY1_IRQn[joykey]), BSP_JOY_PRIO[joykey], 0x00);
          HAL_NVIC_EnableIRQ((IRQn_Type)(JOY1_IRQn[joykey]));
        }
      }
    }
  }

  return ret;
}
/**
  * @brief  DeInit joystick GPIOs.
  * @param  JOY instance
  * @note   JOY DeInit does not disable the MFX, just set the MFX pins in Off mode
  * @param  JoyPins joystick pins to be de-initialized
  * @retval BSP status
  */
int32_t BSP_JOY_DeInit(JOY_TypeDef JOY, JOYPin_TypeDef JoyPins)
{
  uint32_t joykey, key_pressed;

  /* Store Joystick pins initialized */
  JoyPinsMask &= (uint32_t)JoyPins;

  /* Initialized the Joystick. */
  for(joykey = 0U; joykey < JOY_KEY_NUMBER ; joykey++)
  {
    key_pressed = 1UL << joykey;
    if((key_pressed & (uint32_t)JoyPins) == key_pressed)
    {
      if(JOY == JOY1)
      {
        /* Disable the JOY clock */
        switch(key_pressed)
        {
          case JOY_SEL:
          JOY1_SEL_GPIO_CLK_DISABLE();
          break;

          case JOY_DOWN:
          JOY1_DOWN_GPIO_CLK_DISABLE();
          break;

          case JOY_LEFT:
          JOY1_LEFT_GPIO_CLK_DISABLE();
          break;

          case JOY_RIGHT:
          JOY1_RIGHT_GPIO_CLK_DISABLE();
          break;

          case JOY_UP:
          JOY1_UP_GPIO_CLK_DISABLE();
          break;
          default:
          break;
        }
        HAL_GPIO_DeInit(JOY1_PORT[joykey], JOY1_PIN[joykey]);
      }
    }
  }
  return BSP_ERROR_NONE;
}
/**
* @brief  Returns the current joystick status.
* @retval Code of the joystick key pressed
*          This code can be one of the following values:
*            @arg  JOY_NONE
*            @arg  JOY_SEL
*            @arg  JOY_DOWN
*            @arg  JOY_LEFT
*            @arg  JOY_RIGHT
*            @arg  JOY_UP
*/
int32_t BSP_JOY_GetState(JOY_TypeDef JOY,uint32_t JoyPin)
{

  uint32_t joykey, key_pressed;
  for (joykey = 0U; joykey < JOY_KEY_NUMBER ; joykey++)
  {
    key_pressed = 1UL << joykey;
    if((key_pressed & JoyPinsMask) == key_pressed)
    {
      if(JOY == JOY1)
      {
        if (HAL_GPIO_ReadPin(JOY1_PORT[joykey], JOY1_PIN[joykey]) != GPIO_PIN_RESET)
        {
          /* Return Code Joystick key pressed */
          return (int32_t)key_pressed;
        }
      }
    }
  }
  /* No Joystick key pressed */
  return (int32_t)JOY_NONE;

}


/**
  * @brief  This function handles JOY keys interrupt request.
  * @param  JOY Joystick instance
  * @param  JoyPin Specifies the pin connected EXTI handler
  * @retval None
  */
void BSP_JOY_IRQHandler(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  if(JOY == JOY1)
  {
    HAL_EXTI_IRQHandler(&hjoy_exti[POSITION_VAL(JoyPin)]);
  }
}

/**
  * @brief  BSP Joystick Callback.
  * @param  JOY Joystick instance
  * @param  JoyPin Specifies the pin connected EXTI line
  * @retval None.
  */
__weak void BSP_JOY_Callback(JOY_TypeDef JOY, uint32_t JoyPin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(JOY);
  UNUSED(JoyPin);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on JoyPin is triggered. */
}
/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_LOW_LEVEL_Private_Functions Private Functions
  * @{
  */
/**
  * @brief  JOY1 UP EXTI line detection callbacks.
  * @retval None
  */
static void JOY1_UP_EXTI_Callback(void)
{
  BSP_JOY_Callback(JOY1, JOY_UP);
}

/**
  * @brief  JOY1 DOWN EXTI line detection callbacks.
  * @retval None
  */
static void JOY1_DOWN_EXTI_Callback(void)
{
  BSP_JOY_Callback(JOY1, JOY_DOWN);
}

/**
  * @brief  JOY1 SEL EXTI line detection callbacks.
  * @retval None
  */
static void JOY1_SEL_EXTI_Callback(void)
{
  BSP_JOY_Callback(JOY1, JOY_SEL);
}

/**
  * @brief  JOY1 LEFT EXTI line detection callbacks.
  * @retval None
  */
static void JOY1_LEFT_EXTI_Callback(void)
{
  BSP_JOY_Callback(JOY1, JOY_LEFT);
}

/**
  * @brief  JOY1 RIGHT EXTI line detection callbacks.
  * @retval None
  */
static void JOY1_RIGHT_EXTI_Callback(void)
{
  BSP_JOY_Callback(JOY1, JOY_RIGHT);
}



#if (USE_BSP_COM_FEATURE > 0)
/**
  * @brief  Initializes UART MSP.
  * @param  huart UART handle
  * @retval None
  */
static void USART1_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* Enable GPIO clock */
  COM1_TX_GPIO_CLK_ENABLE();
  COM1_RX_GPIO_CLK_ENABLE();

  /* Enable USART clock */
  COM1_CLK_ENABLE();

  /* Configure USART Tx as alternate function */
  gpio_init_structure.Pin = COM1_TX_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Alternate = COM1_TX_AF;
  HAL_GPIO_Init(COM1_TX_GPIO_PORT, &gpio_init_structure);

  /* Configure USART Rx as alternate function */
  gpio_init_structure.Pin = COM1_RX_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = COM1_RX_AF;
  HAL_GPIO_Init(COM1_RX_GPIO_PORT, &gpio_init_structure);
}

/**
  * @brief  Initialize USART6 Msp part
  * @param  huart UART handle
  * @retval None
  */
static void USART1_MspDeInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef          gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* COM GPIO pin configuration */
  gpio_init_structure.Pin  = COM1_TX_PIN;
  HAL_GPIO_DeInit(COM1_TX_GPIO_PORT, gpio_init_structure.Pin);

  gpio_init_structure.Pin  = COM1_RX_PIN;
  HAL_GPIO_DeInit(COM1_RX_GPIO_PORT, gpio_init_structure.Pin);

  /* Disable USART clock */
  COM1_CLK_DISABLE();
}
#endif /* USE_BSP_COM_FEATURE */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

