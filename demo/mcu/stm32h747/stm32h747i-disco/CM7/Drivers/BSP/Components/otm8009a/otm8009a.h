/**
  ******************************************************************************
  * @file    otm8009a.h
  * @author  MCD Application Team
  * @brief   This file contains all the constants parameters for the OTM8009A
  *          which is the LCD Driver for KoD KM-040TMP-02-0621 (WVGA)
  *          DSI LCD Display.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
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
#ifndef OTM8009A_H
#define OTM8009A_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "otm8009a_reg.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup otm8009a
  * @{
  */

/** @addtogroup OTM8009A_Exported_Variables
  * @{
  */

typedef int32_t (*OTM8009A_GetTick_Func) (void);
typedef int32_t (*OTM8009A_Delay_Func)   (uint32_t);
typedef int32_t (*OTM8009A_WriteReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*OTM8009A_ReadReg_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);

/** @addtogroup OTM8009A_Exported_Types
  * @{
  */

typedef struct
{ 
  uint32_t  Orientation;
  uint32_t  ColorCode;
  uint32_t  Brightness;
} OTM8009A_LCD_Ctx_t;

typedef struct
{
  uint16_t                  Address;  
  OTM8009A_WriteReg_Func    WriteReg;
  OTM8009A_ReadReg_Func     ReadReg;  
  OTM8009A_GetTick_Func     GetTick; 
} OTM8009A_IO_t;

typedef struct
{
  OTM8009A_IO_t         IO;
  otm8009a_ctx_t        Ctx; 
  uint8_t               IsInitialized;
} OTM8009A_Object_t;

typedef struct
{
  /* Control functions */
  int32_t (*Init             )(OTM8009A_Object_t*, uint32_t, uint32_t);
  int32_t (*DeInit           )(OTM8009A_Object_t*);
  int32_t (*ReadID           )(OTM8009A_Object_t*, uint32_t*);
  int32_t (*DisplayOn        )(OTM8009A_Object_t*);
  int32_t (*DisplayOff       )(OTM8009A_Object_t*);
  int32_t (*SetBrightness    )(OTM8009A_Object_t*, uint32_t); 
  int32_t (*GetBrightness    )(OTM8009A_Object_t*, uint32_t*);   
  int32_t (*SetOrientation   )(OTM8009A_Object_t*, uint32_t);
  int32_t (*GetOrientation   )(OTM8009A_Object_t*, uint32_t*);

  /* Drawing functions*/
  int32_t ( *SetCursor       ) (OTM8009A_Object_t*, uint32_t, uint32_t); 
  int32_t ( *DrawBitmap      ) (OTM8009A_Object_t*, uint32_t, uint32_t, uint8_t *);
  int32_t ( *FillRGBRect     ) (OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height);
  int32_t ( *DrawHLine       ) (OTM8009A_Object_t*, uint32_t, uint32_t, uint32_t, uint32_t);
  int32_t ( *DrawVLine       ) (OTM8009A_Object_t*, uint32_t, uint32_t, uint32_t, uint32_t);
  int32_t ( *FillRect        ) (OTM8009A_Object_t*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
  int32_t ( *GetPixel        ) (OTM8009A_Object_t*, uint32_t, uint32_t, uint32_t*);
  int32_t ( *SetPixel        ) (OTM8009A_Object_t*, uint32_t, uint32_t, uint32_t);
  int32_t ( *GetXSize        ) (OTM8009A_Object_t*, uint32_t *);
  int32_t ( *GetYSize        ) (OTM8009A_Object_t*, uint32_t *);
  
}OTM8009A_LCD_Drv_t;

/**
  * @}
  */

#define OTM8009A_OK                (0)
#define OTM8009A_ERROR             (-1)

/* OTM8009A ID */
#define OTM8009A_ID                 0x40U

/**
 *  Possible values of Display Orientation
 */
#define OTM8009A_ORIENTATION_PORTRAIT    0U /* Portrait orientation choice of LCD screen  */
#define OTM8009A_ORIENTATION_LANDSCAPE   1U /* Landscape orientation choice of LCD screen */

/**
 *  @brief  Possible values of
 *  pixel data format (ie color coding) transmitted on DSI Data lane in DSI packets
 */
#define OTM8009A_FORMAT_RGB888    ((uint32_t)0x00) /* Pixel format chosen is RGB888 : 24 bpp */
#define OTM8009A_FORMAT_RBG565    ((uint32_t)0x02) /* Pixel format chosen is RGB565 : 16 bpp */

/**
  * @brief  otm8009a_480x800 Size
  */

/* Width and Height in Portrait mode */
#define  OTM8009A_480X800_WIDTH             ((uint16_t)480)     /* LCD PIXEL WIDTH   */
#define  OTM8009A_480X800_HEIGHT            ((uint16_t)800)     /* LCD PIXEL HEIGHT  */

/* Width and Height in Landscape mode */
#define  OTM8009A_800X480_WIDTH             ((uint16_t)800)     /* LCD PIXEL WIDTH   */
#define  OTM8009A_800X480_HEIGHT            ((uint16_t)480)     /* LCD PIXEL HEIGHT  */

/**
  * @brief  OTM8009A_480X800 Timing parameters for Portrait orientation mode
  */
#define  OTM8009A_480X800_HSYNC             ((uint16_t)2)      /* Horizontal synchronization */
#define  OTM8009A_480X800_HBP               ((uint16_t)34)     /* Horizontal back porch      */
#define  OTM8009A_480X800_HFP               ((uint16_t)34)     /* Horizontal front porch     */
#define  OTM8009A_480X800_VSYNC             ((uint16_t)1)      /* Vertical synchronization   */
#define  OTM8009A_480X800_VBP               ((uint16_t)15)     /* Vertical back porch        */
#define  OTM8009A_480X800_VFP               ((uint16_t)16)     /* Vertical front porch       */

/**
  * @brief  OTM8009A_800X480 Timing parameters for Landscape orientation mode
  *         Same values as for Portrait mode in fact.
  */
#define  OTM8009A_800X480_HSYNC             OTM8009A_480X800_HSYNC  /* Horizontal synchronization */
#define  OTM8009A_800X480_HBP               OTM8009A_480X800_HBP    /* Horizontal back porch      */
#define  OTM8009A_800X480_HFP               OTM8009A_480X800_HFP    /* Horizontal front porch     */
#define  OTM8009A_800X480_VSYNC             OTM8009A_480X800_VSYNC  /* Vertical synchronization   */
#define  OTM8009A_800X480_VBP               OTM8009A_480X800_VBP    /* Vertical back porch        */
#define  OTM8009A_800X480_VFP               OTM8009A_480X800_VFP    /* Vertical front porch       */

/**
  * @brief  OTM8009A_480X800 frequency divider
  */
#define OTM8009A_480X800_FREQUENCY_DIVIDER  2   /* LCD Frequency divider      */

/**
  * @}
  */
   
/* Exported macro ------------------------------------------------------------*/
   
/** @defgroup OTM8009A_Exported_Macros OTM8009A Exported Macros
  * @{
  */ 

/**
  * @}
  */ 

/* Exported functions --------------------------------------------------------*/
  
/** @addtogroup OTM8009A_Exported_Functions
  * @{
  */
int32_t OTM8009A_RegisterBusIO (OTM8009A_Object_t *pObj, OTM8009A_IO_t *pIO);
int32_t OTM8009A_Init(OTM8009A_Object_t *pObj, uint32_t ColorCoding, uint32_t Orientation);
int32_t OTM8009A_DeInit(OTM8009A_Object_t *pObj);
int32_t OTM8009A_ReadID(OTM8009A_Object_t *pObj, uint32_t *Id);
int32_t OTM8009A_DisplayOn(OTM8009A_Object_t *pObj);
int32_t OTM8009A_DisplayOff(OTM8009A_Object_t *pObj);
int32_t OTM8009A_SetBrightness(OTM8009A_Object_t *pObj, uint32_t Brightness);
int32_t OTM8009A_GetBrightness(OTM8009A_Object_t *pObj, uint32_t *Brightness);
int32_t OTM8009A_SetOrientation(OTM8009A_Object_t *pObj, uint32_t Orientation);
int32_t OTM8009A_GetOrientation(OTM8009A_Object_t *pObj, uint32_t *Orientation);


int32_t OTM8009A_SetCursor(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos);
int32_t OTM8009A_DrawBitmap(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp);
int32_t OTM8009A_FillRGBRect(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height);
int32_t OTM8009A_DrawHLine(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t OTM8009A_DrawVLine(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t OTM8009A_FillRect(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color);
int32_t OTM8009A_SetPixel(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Color);
int32_t OTM8009A_GetPixel(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t *Color);
int32_t OTM8009A_GetXSize(OTM8009A_Object_t *pObj, uint32_t *XSize);
int32_t OTM8009A_GetYSize(OTM8009A_Object_t *pObj, uint32_t *YSize);

extern OTM8009A_LCD_Drv_t   OTM8009A_LCD_Driver;

/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* OTM8009A_H */
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
