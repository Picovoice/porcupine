/**
  ******************************************************************************
  * @file    adv7533.c
  * @author  MCD Application Team
  * @brief   This file provides the ADV7533 DSI to HDMI bridge driver 
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

/* Includes ------------------------------------------------------------------*/
#include "adv7533.h"

/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup Components
  * @{
  */ 

/** @defgroup ADV7533 ADV7533
  * @brief     This file provides a set of functions needed to drive the 
  *            adv7533 DSI-HDMI bridge.
  * @{
  */
    
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup ADV7533_Private_Constants ADV7533 Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup ADV7533_Exported_Variables ADV7533 Exported Variables
  * @{
  */

/* Audio codec driver structure initialization */  
ADV7533_Drv_t ADV7533_Driver = 
{
  ADV7533_AudioInit,
  ADV7533_DeInit,
  ADV7533_ReadID,
  ADV7533_Play,
  ADV7533_Pause,
  ADV7533_Resume,
  ADV7533_Stop,
  ADV7533_SetFrequency,
  ADV7533_GetFrequency,  
  ADV7533_SetVolume,     /* Not supported, added for compatibility */
  ADV7533_GetVolume,     /* Not supported, added for compatibility */
  ADV7533_SetMute,
  ADV7533_SetOutputMode, /* Not supported, added for compatibility */
  ADV7533_SetResolution, /* Not supported, added for compatibility */
  ADV7533_GetResolution, /* Not supported, added for compatibility */ 
  ADV7533_SetProtocol,   /* Not supported, added for compatibility */
  ADV7533_GetProtocol,   /* Not supported, added for compatibility */
  ADV7533_Reset          /* Not supported, added for compatibility */
};

ADV7533_LcdDrv_t ADV7533_LCD_Driver =
{
  ADV7533_Init,
  ADV7533_DeInit,
  ADV7533_ReadID,
  ADV7533_DisplayOn,
  ADV7533_DisplayOff,
  ADV7533_SetBrightness,
  ADV7533_GetBrightness,  
  ADV7533_SetOrientation,
  ADV7533_GetOrientation,
  ADV7533_SetCursor,
  ADV7533_DrawBitmap,
  ADV7533_FillRGBRect,
  ADV7533_DrawHLine,
  ADV7533_DrawVLine,
  ADV7533_FillRect,
  ADV7533_GetPixel,
  ADV7533_SetPixel,
  ADV7533_GetXSize,
  ADV7533_GetYSize,
};

/**
  * @}
  */

/** @defgroup ADV7533_Function_Prototypes ADV7533 Function Prototypes
  * @{
  */
static int32_t ADV7533_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* Data, uint16_t Length);
static int32_t ADV7533_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* Data, uint16_t Length);
static void ADV7533_SetI2C_Address(ADV7533_Object_t *pObj, uint8_t Address);
/**
  * @}
  */ 

/* Exported functions --------------------------------------------------------*/
/** @defgroup ADV7533_Exported_Functions ADV7533 Exported Functions
  * @{
  */
/**
  * @brief  Function
  * @param  Component object pointer
  * @retval error status
  */
int32_t ADV7533_RegisterBusIO (ADV7533_Object_t *pObj, ADV7533_IO_t *pIO)
{
  int32_t ret;
  
  if (pObj == NULL)
  {
    ret = ADV7533_ERROR;
  }
  else
  {
    pObj->IO.Init      = pIO->Init;
    pObj->IO.DeInit    = pIO->DeInit;
    pObj->IO.Address   = pIO->Address;
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;
    
    pObj->Ctx.ReadReg  = ADV7533_ReadRegWrap;
    pObj->Ctx.WriteReg = ADV7533_WriteRegWrap;
    pObj->Ctx.handle   = pObj;
    
    if(pObj->IO.Init != NULL)
    {
      ret = pObj->IO.Init();
    }
    else
    {
      ret = ADV7533_ERROR;
    }
  }
  
  return ret;
}

/*------------------------------------------------------------------------------
                           HDMI video functions 
------------------------------------------------------------------------------*/ 
/**
  * @brief Initializes the audio codec and the control interface.
  * @param  pObj  pointer to component object
  * @param  ColorCoding   Color Code
  * @param  Orientation   Display orientation
  * @retval Component status
  */
int32_t ADV7533_Init(ADV7533_Object_t *pObj, uint32_t ColorCoding, uint32_t Orientation)
{
  int32_t ret = ADV7533_OK;
  uint8_t tmp;
  
  /* Prevent unused argument(s) compilation warning */
  (void)(ColorCoding);
  (void)(Orientation);
  
  if(pObj->IsInitialized == 0U)
  {
    pObj->IO.Init();
    
    ADV7533_SetI2C_Address(pObj, ADV7533_MAIN_I2C_ADDR);
    
    /* Configure the IC2 address for CEC_DSI interface */
    tmp = ADV7533_CEC_DSI_I2C_ADDR;
    if(adv7533_write_reg(&pObj->Ctx, 0xE1, &tmp, 1) != ADV7533_OK)
    {
      ret = ADV7533_ERROR;
    }
    else
    {
      pObj->IsInitialized = 1;
    }
  }
  
  return ret;
}

/**
  * @brief  Deinitializes the adv7533
  * @param  pObj pointer to component object
  * @retval  None
  */
int32_t ADV7533_DeInit(ADV7533_Object_t *pObj)
{
  if(pObj->IsInitialized == 1U)
  {  
    /* Deinitialize Audio adv7533 interface */
    pObj->IO.DeInit();
    
    /* Set IsInitialized to 0 */
    pObj->IsInitialized = 0;
  }
  
  return ADV7533_OK;
}

/**
  * @brief  Get the adv7533 ID.
  * @param  pObj  pointer to component object
  * @retval The adv7533 ID 
  */
int32_t ADV7533_ReadID(ADV7533_Object_t *pObj, uint32_t *Id)
{
  int32_t ret = ADV7533_OK;  
  uint8_t tmp1, tmp2;
  
  /* Set I2C address to ADV7533_CEC_DSI_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_CEC_DSI_I2C_ADDR);
  
  if(adv7533_read_reg(&pObj->Ctx, ADV7533_CHIPID_ADDR0, &tmp1, 1) != ADV7533_OK)
  {
    ret = ADV7533_ERROR;
  }
  else if(adv7533_read_reg(&pObj->Ctx, ADV7533_CHIPID_ADDR1, &tmp2, 1) != ADV7533_OK)
  {
    ret = ADV7533_ERROR;
  }
  else
  {
    *Id = (uint16_t)(((uint16_t)tmp1 << 8) | tmp2); 
  }
  
  return ret;
}

/**
  * @brief  Power on the ADV7533 bridge.
  * @param  pObj pointer to component object
  * @retval Component status
  */
int32_t ADV7533_DisplayOn(ADV7533_Object_t *pObj)
{
  int32_t ret = ADV7533_OK;  
  uint8_t tmp;
  
  ADV7533_SetI2C_Address(pObj, ADV7533_MAIN_I2C_ADDR);
  /* Power on */
  if(adv7533_read_reg(&pObj->Ctx, ADV7533_MAIN_POWER_DOWN_REG, &tmp, 1) != ADV7533_OK)
  {
    ret = ADV7533_ERROR;
  }
  else
  {
    tmp &= ~0x40U;
    if(adv7533_write_reg(&pObj->Ctx, ADV7533_MAIN_POWER_DOWN_REG, &tmp, 1) != ADV7533_OK)
    {
      ret = ADV7533_ERROR;
    }
  }
  
  return ret;
}

/**
  * @brief  Power off the ADV7533 bridge.
  * @param  pObj pointer to component object
  * @retval Component status
  */
int32_t ADV7533_DisplayOff(ADV7533_Object_t *pObj)
{
  int32_t ret = ADV7533_OK;  
  uint8_t tmp;
  
  ADV7533_SetI2C_Address(pObj, ADV7533_MAIN_I2C_ADDR);
  /* Power down */
  if(adv7533_read_reg(&pObj->Ctx, ADV7533_MAIN_POWER_DOWN_REG, &tmp, 1) != ADV7533_OK)
  {
    ret = ADV7533_ERROR;
  }
  else
  {
    tmp |= 0x40U;
    if(adv7533_write_reg(&pObj->Ctx, ADV7533_MAIN_POWER_DOWN_REG, &tmp, 1) != ADV7533_OK)
    {
      ret = ADV7533_ERROR;
    }
  }
  
  return ret;
}

/**
  * @brief  Configure the DSI-HDMI ADV7533 bridge for video.
  * @param  pObj pointer to component object
  * @param  LaneNumber Number of lanes to be configured
  * @retval Component status
  */
int32_t ADV7533_Configure(ADV7533_Object_t *pObj, uint8_t LaneNumber)
{
  int32_t ret;  
  uint8_t tmp, val;
  
  /* ADV7533 Power Settings */
  /* Set I2C address to ADV7533_MAIN_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_MAIN_I2C_ADDR);
  
  /* Power down */
  ret = adv7533_read_reg(&pObj->Ctx, 0x41, &tmp, 1);
  tmp &= ~0x40U;
  ret += adv7533_write_reg(&pObj->Ctx, 0x41, &tmp, 1);
  /* HPD Override */
  ret += adv7533_read_reg(&pObj->Ctx, 0xD6, &tmp, 1);
  tmp |= 0x40U;
  ret += adv7533_write_reg(&pObj->Ctx, 0xD6, &tmp, 1);
  
  /* Set I2C address to ADV7533_CEC_DSI_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_CEC_DSI_I2C_ADDR);
  
  /* Gate DSI LP Oscillator and DSI Bias Clock Powerdown */
  ret += adv7533_read_reg(&pObj->Ctx, 0x03, &tmp, 1);
  tmp &= ~0x02U;
  ret += adv7533_write_reg(&pObj->Ctx, 0x03, &tmp, 1);

  /* Set I2C address to ADV7533_MAIN_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_MAIN_I2C_ADDR);  
  /* Fixed registers that must be set on power-up */
  ret += adv7533_read_reg(&pObj->Ctx, 0x16, &tmp, 1);
  tmp &= ~0x3EU;
  tmp |= 0x20U; 
  ret += adv7533_write_reg(&pObj->Ctx, 0x16, &tmp, 1);
  val = 0xE0;
  ret += adv7533_write_reg(&pObj->Ctx, 0x9A, &val, 1);
  ret += adv7533_read_reg(&pObj->Ctx, 0xBA, &tmp, 1);
  tmp &= ~0xF8U;
  tmp |= 0x70U; 
  ret += adv7533_write_reg(&pObj->Ctx, 0xBA, &tmp, 1);
  val = 0x82;
  ret += adv7533_write_reg(&pObj->Ctx, 0xDE, &val, 1);
  
  ret += adv7533_read_reg(&pObj->Ctx, 0xE4, &tmp, 1); 
  tmp |= 0x40U;
  ret += adv7533_write_reg(&pObj->Ctx, 0xE4, &tmp, 1);
  val = 0x80;
  ret += adv7533_write_reg(&pObj->Ctx, 0xE5, &val, 1);
  
  /* Set I2C address to ADV7533_CEC_DSI_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_CEC_DSI_I2C_ADDR);
  
  ret += adv7533_read_reg(&pObj->Ctx, 0x15, &tmp, 1);
  tmp &= ~0x30U;
  tmp |= 0x10U; 
  ret += adv7533_read_reg(&pObj->Ctx, 0x17, &tmp, 1);
  tmp &= ~0xF0U;
  tmp |= 0xD0U; 
  ret += adv7533_write_reg(&pObj->Ctx, 0x17, &tmp, 1);
  ret += adv7533_read_reg(&pObj->Ctx, 0x24, &tmp, 1);
  tmp &= ~0x10U;
  ret += adv7533_write_reg(&pObj->Ctx, 0x24, &tmp, 1);
  ret += adv7533_read_reg(&pObj->Ctx, 0x57, &tmp, 1);
  tmp |= 0x01U;
  tmp |= 0x10U;
  ret += adv7533_write_reg(&pObj->Ctx, 0x57, &tmp, 1);
  
  /* Configure the number of DSI lanes */
  val = (LaneNumber << 4);
  ret += adv7533_write_reg(&pObj->Ctx, 0x1C, &val, 1);
  
  /* Setup video output mode */
  /* Set I2C address to ADV7533_MAIN_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_MAIN_I2C_ADDR);
  
  /* Select HDMI mode */
  ret += adv7533_read_reg(&pObj->Ctx, 0xAF, &tmp, 1);
  tmp |= 0x02U;
  ret += adv7533_write_reg(&pObj->Ctx, 0xAF, &tmp, 1);

  /* Set I2C address to ADV7533_CEC_DSI_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_CEC_DSI_I2C_ADDR); 

  /* HDMI Output Enable */
  ret += adv7533_read_reg(&pObj->Ctx, 0x03, &tmp, 1);
  tmp |= 0x80U;
  ret += adv7533_write_reg(&pObj->Ctx, 0x03, &tmp, 1);

  /* Set I2C address to ADV7533_MAIN_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_MAIN_I2C_ADDR);
  
  /* GC packet enable */
  ret += adv7533_read_reg(&pObj->Ctx, 0x40, &tmp, 1);
  tmp |= 0x80U;
  ret += adv7533_write_reg(&pObj->Ctx, 0x40, &tmp, 1);
  /* Input color depth 24-bit per pixel */
  ret += adv7533_read_reg(&pObj->Ctx, 0x4C, &tmp, 1);
  tmp &= ~0x0FU;
  tmp |= 0x03U;
  ret += adv7533_write_reg(&pObj->Ctx, 0x4C, &tmp, 1);
  /* Down dither output color depth */
  val = 0xFC;
  ret += adv7533_write_reg(&pObj->Ctx, 0x49, &val, 1);

  /* Set I2C address to ADV7533_CEC_DSI_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_CEC_DSI_I2C_ADDR);
  
  /* Internal timing disabled */
  ret += adv7533_read_reg(&pObj->Ctx, 0x27, &tmp, 1);
  tmp &= ~0x80U;
  ret += adv7533_write_reg(&pObj->Ctx, 0x27, &tmp, 1);
  
  if(ret != ADV7533_OK)
  {
    ret = ADV7533_ERROR;
  }
  
  return ret;  
}

/**
  * @brief  Enable video pattern generation.
  * @param  pObj pointer to component object
  * @retval Component status
  */
int32_t ADV7533_PatternEnable(ADV7533_Object_t *pObj)
{
  int32_t ret;
  uint8_t tmp;
  
  /* Set I2C address to ADV7533_CEC_DSI_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_CEC_DSI_I2C_ADDR);
  
  /* Timing generator enable */
  /* Color bar */
  tmp = 0x80;
  ret = adv7533_write_reg(&pObj->Ctx, 0x55, &tmp, 1);
  
  if(ret == ADV7533_OK)
  {
    tmp = 0xA0;
    ret += adv7533_write_reg(&pObj->Ctx, 0x55, &tmp, 1);
  }
  if(ret == ADV7533_OK)
  {
    tmp = 0x89;
    ret += adv7533_write_reg(&pObj->Ctx, 0x03, &tmp, 1);
  }
  if(ret == ADV7533_OK)
  {
    tmp = 0x16;
    ret += adv7533_write_reg(&pObj->Ctx, 0xAF, &tmp, 1);
  }
  
  if(ret != ADV7533_OK)
  {
    ret = ADV7533_ERROR;
  }
  
  return ret;  
}

/**
  * @brief  Disable video pattern generation.
  * @param  pObj pointer to component object
  * @retval Component status
  */
int32_t ADV7533_PatternDisable(ADV7533_Object_t *pObj)
{
  int32_t ret = ADV7533_OK;
  uint8_t tmp = 0x00;
  
  /* Set I2C address to ADV7533_CEC_DSI_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_CEC_DSI_I2C_ADDR);
  
  /* Timing generator enable */
  if(adv7533_write_reg(&pObj->Ctx, 0x55, &tmp, 1) != ADV7533_OK)
  {
    ret = ADV7533_ERROR;
  }
  
  return ret;
}

/**
  * @brief  Set the display brightness.
  * @param  pObj Component object
  * @param  Brightness   display brightness to be set
  * @retval Component status
  */
int32_t ADV7533_SetBrightness(ADV7533_Object_t *pObj, uint32_t Brightness)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;  
}

/**
  * @brief  Get the display brightness.
  * @param  pObj Component object
  * @param  Brightness   display brightness to be returned
  * @retval Component status
  */
int32_t ADV7533_GetBrightness(ADV7533_Object_t *pObj, uint32_t *Brightness)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;  
}


/**
* @brief  Set the display Orientation.
* @param  pObj Component object
* @param  Orientation   display Orientation to be set
* @retval Component status
*/
int32_t ADV7533_SetOrientation(ADV7533_Object_t *pObj, uint32_t Orientation)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;
}

/**
  * @brief  Set the display Orientation.
  * @param  pObj Component object
  * @param  Orientation   display Orientation to be returned
  * @retval Component status
  */
int32_t ADV7533_GetOrientation(ADV7533_Object_t *pObj, uint32_t *Orientation)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;
}

/**
  * @brief  Set the display Width.
  * @param  pObj Component object
  * @param  Xsize   display Width to be set
  * @retval Component status
  */
int32_t ADV7533_GetXSize(ADV7533_Object_t *pObj, uint32_t *Xsize)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);

  
  return ADV7533_ERROR;
}

/**
  * @brief  Set the display Height.
  * @param  pObj Component object
  * @param  Ysize   display Height to be set
  * @retval Component status
  */
int32_t ADV7533_GetYSize(ADV7533_Object_t *pObj, uint32_t *Ysize)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
    
  return ADV7533_ERROR;
}

/**
  * @brief  Set the display cursor.
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @retval Component status
  */
int32_t ADV7533_SetCursor(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;
}

/**
  * @brief  Draw Bitmap image
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  pBmp pointer to bmp data
  * @retval Component status
  */
int32_t ADV7533_DrawBitmap(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;
}

int32_t ADV7533_FillRGBRect(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;
}
/**
  * @brief  Draw Horizontal Line
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Length Line length
  * @param  Color Draw color
  * @retval Component status
  */
int32_t ADV7533_DrawHLine(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;
}

/**
  * @brief  Draw Vertical line
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Length Line length
  * @param  Color Draw color
  * @retval Component status
  */
int32_t ADV7533_DrawVLine(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;
}

/**
  * @brief  Fill rectangle
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Width Rectangle width
  * @param  Height Rectangle height
  * @param  Color Draw color
  * @retval Component status
  */
int32_t ADV7533_FillRect(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;
}

/**
  * @brief  Get pixel color
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Color Color to be returned
  * @retval Component status
  */
int32_t ADV7533_GetPixel(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t *Color)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;
}

/**
  * @brief  Set pixel color
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Color Color to be returned
  * @retval Component status
  */
int32_t ADV7533_SetPixel(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Color)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return ADV7533_ERROR;
}

/*------------------------------------------------------------------------------
                           HDMI Audio functions 
------------------------------------------------------------------------------*/
/**
  * @brief Initializes the audio codec and the control interface.
  * @param  pObj pointer to component object
  * @param  pInit  pointer de component init structure
  * @retval Component status
  */
int32_t ADV7533_AudioInit(ADV7533_Object_t *pObj, ADV7533_Init_t *pInit)
{
  int32_t ret = ADV7533_OK;
  uint32_t val = 4096U;
  uint8_t  tmp;
  
  if(pObj->IsInitialized == 0U)
  {    
    /* Set I2C address to ADV7533_CEC_DSI_I2C_ADDR */
    ADV7533_SetI2C_Address(pObj, ADV7533_CEC_DSI_I2C_ADDR);
    
    /* Audio data enable*/
    ret = adv7533_read_reg(&pObj->Ctx, 0x05, &tmp, 1);
    tmp &= ~0x20U;
    ret += adv7533_write_reg(&pObj->Ctx, 0x05, &tmp, 1);
    
    /* Set I2C address to ADV7533_MAIN_I2C_ADDR */
    ADV7533_SetI2C_Address(pObj, ADV7533_MAIN_I2C_ADDR);
    
    /* HDMI statup */
    tmp = (uint8_t)((val & 0xF0000U)>>16);
    ret += adv7533_write_reg(&pObj->Ctx, 0x01, &tmp, 1);
    
    tmp= (uint8_t)((val & 0xFF00U)>>8);
    ret += adv7533_write_reg(&pObj->Ctx, 0x02, &tmp, 1);
    
    tmp= (uint8_t)((val & 0xFFU));
    ret += adv7533_write_reg(&pObj->Ctx, 0x03, &tmp, 1);
    
    /* Enable spdif */
    ret += adv7533_read_reg(&pObj->Ctx, 0x0B, &tmp, 1);
    tmp |= 0x80U;
    ret += adv7533_write_reg(&pObj->Ctx, 0x0B, &tmp, 1);
    
    /* Enable I2S */
    ret += adv7533_read_reg(&pObj->Ctx, 0x0C, &tmp, 1);
    tmp |=0x04U;
    ret += adv7533_write_reg(&pObj->Ctx, 0x0C, &tmp, 1);
    
    /* Set audio sampling frequency */
    ret += ADV7533_SetFrequency(pObj, pInit->Frequency);
    
    /* Select SPDIF is 0x10 , I2S=0x00  */
    ret += adv7533_read_reg(&pObj->Ctx, 0x0A, &tmp, 1);
    tmp &=~ 0x10U;
    ret += adv7533_write_reg(&pObj->Ctx, 0x0A, &tmp, 1);
    
    /* Set v1P2 enable */
    ret += adv7533_read_reg(&pObj->Ctx, 0xE4, &tmp, 1);
    tmp |= 0x80U;
    ret += adv7533_write_reg(&pObj->Ctx, 0xE4, &tmp, 1);
    
    pObj->IsInitialized = 1;
  }

  return ret;
}

/**
  * @brief Pauses playing on the audio hdmi
  * @param  pObj pointer to component object 
  * @retval Component status
  */
int32_t ADV7533_Pause(ADV7533_Object_t *pObj)
{ 
  return(ADV7533_SetMute(pObj, ADV7533_MUTE_ON));
}       
            
/**
  * @brief Resumes playing on the audio hdmi.
  * @param  pObj pointer to component object 
  * @retval Component status
  */   
int32_t ADV7533_Resume(ADV7533_Object_t *pObj)
{ 
  return(ADV7533_SetMute(pObj, ADV7533_MUTE_OFF));
} 

/**
  * @brief Start the audio hdmi play feature.
  * @note  For this codec no Play options are required.
  * @param  pObj pointer to component object   
  * @retval Component status
  */
int32_t ADV7533_Play(ADV7533_Object_t *pObj)
{
  return(ADV7533_SetMute(pObj, ADV7533_MUTE_OFF));
}
            
/**
  * @brief Stop playing on the audio hdmi
  * @param  pObj pointer to component object 
  * @retval Component status
  */
int32_t ADV7533_Stop(ADV7533_Object_t *pObj, uint32_t CodecPdwnMode)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(CodecPdwnMode);
  
  return(ADV7533_SetMute(pObj, ADV7533_MUTE_ON));
}               
            
/**
  * @brief Enables or disables the mute feature on the audio hdmi.
  * @param  pObj pointer to component object   
  * @param Cmd: AUDIO_MUTE_ON to enable the mute or AUDIO_MUTE_OFF to disable the
  *             mute mode.
  * @retval Component status
  */
int32_t ADV7533_SetMute(ADV7533_Object_t *pObj, uint16_t Cmd)
{
  int32_t ret;  
  uint8_t tmp = 0;
  
  /* Set I2C address to ADV7533_MAIN_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_MAIN_I2C_ADDR);
  
  ret = adv7533_read_reg(&pObj->Ctx, 0x0D, &tmp, 1);
  if (Cmd == ADV7533_MUTE_ON)  
  {
    /* enable audio mute*/ 
    tmp |= 0x40U;
    if(adv7533_write_reg(&pObj->Ctx, 0x0D, &tmp, 1) != ADV7533_OK)
    {
      ret = ADV7533_ERROR;
    }
  }
  else
  {
    /*audio mute off disable the mute */
    tmp &= ~0x40U;
    if(adv7533_write_reg(&pObj->Ctx, 0x0D, &tmp, 1) != ADV7533_OK)
    {
      ret = ADV7533_ERROR;
    }
  }
  
  return ret;
}

/**
  * @brief Sets output mode.
  * @param  pObj  pointer to component object
  * @param  Output   hdmi output.
  * @retval Component status
  */
int32_t ADV7533_SetOutputMode(ADV7533_Object_t *pObj, uint16_t Output)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);
  (void)(Output);
  
  /* Not supported */  
  return ADV7533_ERROR;
}    
            
/**
  * @brief Sets volumee.
  * @param  pObj pointer to component object
  * @param  Volume : volume value.
  * @retval Component status
  */           
int32_t ADV7533_SetVolume(ADV7533_Object_t *pObj, uint8_t Volume, uint8_t InputOutput)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);
  (void)(Volume);
  (void)(InputOutput);
  
  /* Not supported */  
  return ADV7533_ERROR;
}

/**
  * @brief Sets volumee.
  * @param  pObj pointer to component object
  * @param  Volume : volume value.
  * @retval Component status
  */           
int32_t ADV7533_GetVolume(ADV7533_Object_t *pObj, uint8_t Volume)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);
  (void)(Volume);
  
  /* Not supported */  
  return ADV7533_ERROR;
}

/**
  * @brief Set Audio resolution.
  * @param  pObj pointer to component object
  * @param Resolution: Audio resolution.
  * @retval ADV7533_OK
  */
int32_t ADV7533_SetResolution(ADV7533_Object_t *pObj, uint32_t Resolution)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);
  (void)(Resolution);
  
  /* Not supported */  
  return ADV7533_ERROR;
}

/**
  * @brief Get Audio resolution.
  * @param  pObj pointer to component object
  * @retval Audio resolution
  */
int32_t ADV7533_GetResolution(ADV7533_Object_t *pObj, uint32_t *Resolution)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);
  (void)(Resolution);
  
  /* Not supported */  
  return ADV7533_ERROR;
}

/**
  * @brief Set Audio Protocol.
  * @param  pObj pointer to component object
  * @param Protocol: Audio Protocol
  * @retval ADV7533_OK
  */
int32_t ADV7533_SetProtocol(ADV7533_Object_t *pObj, uint16_t Protocol)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);
  (void)(Protocol);
         
  /* Not supported */  
  return ADV7533_ERROR;
}

/**
  * @brief Get Audio Protocol.
  * @param  pObj pointer to component object
  * @retval Audio Protocol
  */
int32_t ADV7533_GetProtocol(ADV7533_Object_t *pObj)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);
  
  /* Not supported */  
  return ADV7533_ERROR;
}

/**
  * @brief Sets new frequency.
  * @param  pObj pointer to component object
  * @param AudioFreq: Audio frequency used to play the audio stream.
  * @retval Component status
  */
int32_t ADV7533_SetFrequency(ADV7533_Object_t *pObj, uint32_t AudioFreq)
{
  int32_t ret;  
  uint8_t tmp;
  
  /* Set I2C address to ADV7533_MAIN_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_MAIN_I2C_ADDR);
  
  ret = adv7533_read_reg(&pObj->Ctx, 0x15, &tmp, 1);
  tmp &= (~0xF0U);
  /*  Clock Configurations */
  switch (AudioFreq)
  {
  case  ADV7533_FREQUENCY_32K:
    /* Sampling Frequency =32 KHZ*/
    tmp |= 0x30U;
    ret += adv7533_write_reg(&pObj->Ctx, 0x15, &tmp, 1);
    break;
  case  ADV7533_FREQUENCY_44K: 
    /* Sampling Frequency =44,1 KHZ*/
    tmp |= 0x00U;
    ret += adv7533_write_reg(&pObj->Ctx, 0x15, &tmp, 1);
    break;
    
  case  ADV7533_FREQUENCY_48K: 
    /* Sampling Frequency =48KHZ*/
    tmp |= 0x20U;
    ret += adv7533_write_reg(&pObj->Ctx, 0x15, &tmp, 1);
    break;
    
  case  ADV7533_FREQUENCY_96K: 
    /* Sampling Frequency =96 KHZ*/
    tmp |= 0xA0U;
    ret += adv7533_write_reg(&pObj->Ctx, 0x15, &tmp, 1);
    break;
    
  case  ADV7533_FREQUENCY_88K: 
    /* Sampling Frequency =88,2 KHZ*/
    tmp |= 0x80U;
    ret += adv7533_write_reg(&pObj->Ctx, 0x15, &tmp, 1);
    break;
    
  case  ADV7533_FREQUENCY_176K: 
    /* Sampling Frequency =176,4 KHZ*/
    tmp |= 0xC0U;
    ret += adv7533_write_reg(&pObj->Ctx, 0x15, &tmp, 1);
    break;
    
  case  ADV7533_FREQUENCY_192K: 
    /* Sampling Frequency =192KHZ*/
    tmp |= 0xE0U;
    ret += adv7533_write_reg(&pObj->Ctx, 0x15, &tmp, 1);
    break;
  default:
    break;
  }
  
  return ret;
}

/**
  * @brief Sets new frequency.
  * @param  pObj pointer to component object
  * @param AudioFreq: Audio frequency used to play the audio stream.
  * @retval Component status
  */
int32_t ADV7533_GetFrequency(ADV7533_Object_t *pObj, uint32_t *Frequency)
{
  int32_t ret;
  uint8_t  freq;

  /* Set I2C address to ADV7533_MAIN_I2C_ADDR */
  ADV7533_SetI2C_Address(pObj, ADV7533_MAIN_I2C_ADDR);
  
  ret = adv7533_read_reg(&pObj->Ctx, 0x15, &freq, 1);
  freq &= 0xF0U;
  
  /*  Clock Configurations */
  switch (freq)
  {
  case  0x30:
    /* Sampling Frequency =32 KHZ*/
    *Frequency = ADV7533_FREQUENCY_32K;
    break;
  case  0x00: 
    /* Sampling Frequency =44,1 KHZ*/
    *Frequency = ADV7533_FREQUENCY_44K;
    break;
    
  case  0x20: 
    /* Sampling Frequency =48KHZ*/
    *Frequency = ADV7533_FREQUENCY_48K;
    break;
    
  case  0xA0: 
    /* Sampling Frequency =96 KHZ*/
    *Frequency = ADV7533_FREQUENCY_96K;
    break;
    
  case  0x80: 
    /* Sampling Frequency =88,2 KHZ*/
    *Frequency = ADV7533_FREQUENCY_88K;
    break;
    
  case  0xC0: 
    /* Sampling Frequency =176,4 KHZ*/
    *Frequency = ADV7533_FREQUENCY_176K;
    break;
    
  case  0xE0: 
    /* Sampling Frequency =192KHZ*/
    *Frequency = ADV7533_FREQUENCY_192K;
    break;
    
  default:
    break;    
  }
  
  return ret;
}

/**
  * @brief Resets adv7533 registers.
  * @param  pObj pointer to component object 
  * @retval Component status
  */
int32_t ADV7533_Reset(ADV7533_Object_t *pObj)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);
  
  /* Not supported */  
  return ADV7533_ERROR;
}

/******************** Static functions ****************************************/
/**
  * @brief  Configure the DSI-HDMI ADV7533 bridge for video.
  * @param config : pointer to adv7533ConfigTypeDef that contains the
  *                 video configuration parameters
  * @retval Component status
  */
static void ADV7533_SetI2C_Address(ADV7533_Object_t *pObj, uint8_t Address)
{
  pObj->IO.Address = Address;
}

/**
  * @brief  Function
  * @param  handle: Component object handle
  * @param  Reg: The target register address to write
  * @param  pData: The target register value to be written
  * @param  Length: buffer size to be written
  * @retval error status
  */
static int32_t ADV7533_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  ADV7533_Object_t *pObj = (ADV7533_Object_t *)handle;

  return pObj->IO.ReadReg(pObj->IO.Address, Reg, pData, Length);
}

/**
  * @brief  Function
  * @param  handle: Component object handle
  * @param  Reg: The target register address to write
  * @param  pData: The target register value to be written
  * @param  Length: buffer size to be written
  * @retval error status
  */
static int32_t ADV7533_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  ADV7533_Object_t *pObj = (ADV7533_Object_t *)handle;

  return pObj->IO.WriteReg(pObj->IO.Address, Reg, pData, Length);
}

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
