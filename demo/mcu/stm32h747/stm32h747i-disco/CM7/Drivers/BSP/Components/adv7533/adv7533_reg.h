
/**
  ******************************************************************************
  * @file    adv7533_reg.h
  * @author  MCD Application Team
  * @brief   This file contains all the constants parameters for the ADV7533
  *          which is the HDMI bridge between DSI and HDMI
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
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
#ifndef ADV7533_REG_H
#define ADV7533_REG_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
/** @addtogroup BSP
  * @{
  */
 
/** @addtogroup Components
  * @{
  */

/** @addtogroup ADV7533
  * @{
  */

/** @addtogroup ADV7533_Exported_Constants
  * @{
  */
    
/** 
  * @brief  ADV7533 Main Registers
  */
#define  ADV7533_MAIN_SYNC_REG              0x17U
#define  ADV7533_MAIN_POWER_DOWN_REG        0x41U
#define  ADV7533_MAIN_HPG_REG               0x42U

/**
 * @brief   ADV7533 Main Features Parameters
 */

/**
  * @brief ADV7533 CEC DSI Registers
  */
#define ADV7533_CEC_DSI_INTERNAL_TIMING_REG  0x27U
#define ADV7533_CEC_DSI_TOTAL_WIDTH_H_REG    0x28U
#define ADV7533_CEC_DSI_TOTAL_WIDTH_L_REG    0x29U
#define ADV7533_CEC_DSI_HSYNC_H_REG          0x2AU
#define ADV7533_CEC_DSI_HSYNC_L_REG          0x2BU
#define ADV7533_CEC_DSI_HFP_H_REG            0x2CU
#define ADV7533_CEC_DSI_HFP_L_REG            0x2DU
#define ADV7533_CEC_DSI_HBP_H_REG            0x2EU
#define ADV7533_CEC_DSI_HBP_L_REG            0x2FU

#define ADV7533_CEC_DSI_TOTAL_HEIGHT_H_REG   0x30U
#define ADV7533_CEC_DSI_TOTAL_HEIGHT_L_REG   0x31U
#define ADV7533_CEC_DSI_VSYNC_H_REG          0x32U
#define ADV7533_CEC_DSI_VSYNC_L_REG          0x33U
#define ADV7533_CEC_DSI_VFP_H_REG            0x34U
#define ADV7533_CEC_DSI_VFP_L_REG            0x35U
#define ADV7533_CEC_DSI_VBP_H_REG            0x36U
#define ADV7533_CEC_DSI_VBP_L_REG            0x37U
   
/** @Brief device ID register 
  */
#define   ADV7533_CHIPID_ADDR0               0x00U
#define   ADV7533_CHIPID_ADDR1               0x01U    

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
   
/** @defgroup ADV7533_Exported_Macros ADV7533 Exported Macros
  * @{
  */ 

/**
  * @}
  */ 
/** @addtogroup ADV7533_Exported_Types
  * @{
  */
typedef int32_t (*ADV7533_Write_Func)(void *, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*ADV7533_Read_Func) (void *, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  ADV7533_Write_Func   WriteReg;
  ADV7533_Read_Func    ReadReg;
  void                *handle;
} adv7533_ctx_t;  
/**
  * @}
  */ 
    
/* Exported functions --------------------------------------------------------*/
  
/** @addtogroup ADV7533_Exported_Functions
  * @{
  */
int32_t adv7533_write_reg(adv7533_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length );
int32_t adv7533_read_reg(adv7533_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length );

/**
  * @}
  */ 
#ifdef __cplusplus
}
#endif

#endif /* ADV7533_REG_H */
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
