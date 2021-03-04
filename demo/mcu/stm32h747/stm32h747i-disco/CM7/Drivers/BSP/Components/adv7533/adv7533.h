
/**
  ******************************************************************************
  * @file    adv7533.h
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
#ifndef ADV7533_H
#define ADV7533_H

/* Includes ------------------------------------------------------------------*/  
#include "adv7533_reg.h"
#include <stddef.h>

/** @addtogroup BSP
  * @{
  */
 
/** @addtogroup Components
  * @{
  */

/** @addtogroup ADV7533
  * @{
  */

/** @defgroup ADV7533_Exported_Types Exported Types
  * @{
  */
typedef int32_t (*ADV7533_Init_Func)    (void);
typedef int32_t (*ADV7533_DeInit_Func)  (void);
typedef int32_t (*ADV7533_GetTick_Func) (void);
typedef int32_t (*ADV7533_Delay_Func)   (uint32_t);
typedef int32_t (*ADV7533_WriteReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*ADV7533_ReadReg_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  uint32_t   Frequency;
} ADV7533_Init_t;

typedef struct
{
  ADV7533_Init_Func          Init;
  ADV7533_DeInit_Func        DeInit;
  uint16_t                   Address;  
  ADV7533_WriteReg_Func      WriteReg;
  ADV7533_ReadReg_Func       ReadReg; 
  ADV7533_GetTick_Func       GetTick; 
} ADV7533_IO_t;

typedef struct
{
  ADV7533_IO_t         IO;
  adv7533_ctx_t        Ctx;   
  uint8_t              IsInitialized;
} ADV7533_Object_t;

 typedef struct
{
  int32_t ( *Init                 ) ( ADV7533_Object_t *, ADV7533_Init_t* );
  int32_t ( *DeInit               ) ( ADV7533_Object_t * );
  int32_t ( *ReadID               ) ( ADV7533_Object_t *, uint32_t *); 
  int32_t ( *Play                 ) ( ADV7533_Object_t * );
  int32_t ( *Pause                ) ( ADV7533_Object_t * );
  int32_t ( *Resume               ) ( ADV7533_Object_t * );
  int32_t ( *Stop                 ) ( ADV7533_Object_t *, uint32_t );
  int32_t ( *SetFrequency         ) ( ADV7533_Object_t *, uint32_t );
  int32_t ( *GetFrequency         ) ( ADV7533_Object_t *, uint32_t* );  
  int32_t ( *SetVolume            ) ( ADV7533_Object_t *, uint8_t, uint8_t );
  int32_t ( *GetVolume            ) ( ADV7533_Object_t *, uint8_t );  
  int32_t ( *SetMute              ) ( ADV7533_Object_t *, uint16_t );
  int32_t ( *SetOutputMode        ) ( ADV7533_Object_t *, uint16_t ); 
  int32_t ( *SetResolution        ) ( ADV7533_Object_t *, uint32_t );
  int32_t ( *GetResolution        ) ( ADV7533_Object_t *, uint32_t *);  
  int32_t ( *SetProtocol          ) ( ADV7533_Object_t *, uint16_t );
  int32_t ( *GetProtocol          ) ( ADV7533_Object_t *);  
  int32_t ( *Reset                ) ( ADV7533_Object_t * );
} ADV7533_Drv_t;

 typedef struct
{
  /* Control functions */
  int32_t (*Init             )(ADV7533_Object_t*, uint32_t, uint32_t);
  int32_t (*DeInit           )(ADV7533_Object_t*);
  int32_t (*ReadID           )(ADV7533_Object_t*, uint32_t*);
  int32_t (*DisplayOn        )(ADV7533_Object_t*);
  int32_t (*DisplayOff       )(ADV7533_Object_t*);
  int32_t (*SetBrightness    )(ADV7533_Object_t*, uint32_t); 
  int32_t (*GetBrightness    )(ADV7533_Object_t*, uint32_t*);   
  int32_t (*SetOrientation   )(ADV7533_Object_t*, uint32_t);
  int32_t (*GetOrientation   )(ADV7533_Object_t*, uint32_t*);

  /* Drawing functions*/
  int32_t ( *SetCursor       ) (ADV7533_Object_t*, uint32_t, uint32_t); 
  int32_t ( *DrawBitmap      ) (ADV7533_Object_t*, uint32_t, uint32_t, uint8_t *);
  int32_t (*FillRGBRect      ) (ADV7533_Object_t*, uint32_t, uint32_t, uint8_t *, uint32_t, uint32_t);  
  int32_t ( *DrawHLine       ) (ADV7533_Object_t*, uint32_t, uint32_t, uint32_t, uint32_t);
  int32_t ( *DrawVLine       ) (ADV7533_Object_t*, uint32_t, uint32_t, uint32_t, uint32_t);
  int32_t ( *FillRect        ) (ADV7533_Object_t*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
  int32_t ( *GetPixel        ) (ADV7533_Object_t*, uint32_t, uint32_t, uint32_t*);
  int32_t ( *SetPixel        ) (ADV7533_Object_t*, uint32_t, uint32_t, uint32_t);
  int32_t ( *GetXSize        ) (ADV7533_Object_t*, uint32_t *);
  int32_t ( *GetYSize        ) (ADV7533_Object_t*, uint32_t *);  
} ADV7533_LcdDrv_t;

/**
  * @}
  */

/** @defgroup ADV7533_Exported_Constants Exported Constants
  * @{
  */
#define ADV7533_OK                (0)
#define ADV7533_ERROR             (-1)

/** 
  * @brief AUDIO FREQUENCY
  */
#define ADV7533_FREQUENCY_192K    192000
#define ADV7533_FREQUENCY_176K    176400
#define ADV7533_FREQUENCY_96K     96000
#define ADV7533_FREQUENCY_88K     88200  
#define ADV7533_FREQUENCY_48K     48000
#define ADV7533_FREQUENCY_44K     44100
#define ADV7533_FREQUENCY_32K     32000
#define ADV7533_FREQUENCY_22K     22050
#define ADV7533_FREQUENCY_16K     16000
#define ADV7533_FREQUENCY_11K     11025
#define ADV7533_FREQUENCY_8K      8000

/** 
  * @brief HDMI audio output DEVICE
  */ 
#define OUTPUT_DEVICE_ADV7533_HDMI       0x1000U

/** 
  * @brief  ADV7533 I2C Addresses 0x7A / 0x78
  */  
#define  ADV7533_MAIN_I2C_ADDR           0x7AU
#define  ADV7533_CEC_DSI_I2C_ADDR        0x78U

/** 
  * @brief  ADV7533 Aspect ratio
  */  
#define ADV7533_ASPECT_RATIO_16_9        00U
#define ADV7533_ASPECT_RATIO_4_3         01U

/** 
  * @brief  ADV7533 Aspect ratio
  */  
#define ADV7533_MODE_HDMI                00U
#define ADV7533_MODE_DVI                 01U
   
/**
 * @brief   ADV7533 Main Features Parameters
 */

/** @Brief adv7533 ID
  */
#define ADV7533_ID                      0x7533U

/* MUTE commands */
#define ADV7533_MUTE_ON                 1U
#define ADV7533_MUTE_OFF                0U
    

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

/* Exported functions --------------------------------------------------------*/
  
/** @addtogroup ADV7533_Exported_Functions
  * @{
  */
int32_t ADV7533_RegisterBusIO (ADV7533_Object_t *pObj, ADV7533_IO_t *pIO);
/*------------------------------------------------------------------------------
                           HDMI video functions 
------------------------------------------------------------------------------*/  
int32_t ADV7533_Init(ADV7533_Object_t *pObj, uint32_t ColorCoding, uint32_t Orientation);
int32_t ADV7533_DisplayOn(ADV7533_Object_t *pObj);
int32_t ADV7533_DisplayOff(ADV7533_Object_t *pObj);
int32_t ADV7533_Configure(ADV7533_Object_t *pObj, uint8_t LaneNumber);
int32_t ADV7533_PatternEnable(ADV7533_Object_t *pObj);
int32_t ADV7533_PatternDisable(ADV7533_Object_t *pObj);

int32_t ADV7533_SetBrightness(ADV7533_Object_t *pObj, uint32_t Brightness);
int32_t ADV7533_GetBrightness(ADV7533_Object_t *pObj, uint32_t *Brightness);
int32_t ADV7533_SetOrientation(ADV7533_Object_t *pObj, uint32_t Orientation);
int32_t ADV7533_GetOrientation(ADV7533_Object_t *pObj, uint32_t *Orientation);


int32_t ADV7533_SetCursor(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos);
int32_t ADV7533_DrawBitmap(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp);
int32_t ADV7533_FillRGBRect(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height);
int32_t ADV7533_DrawHLine(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t ADV7533_DrawVLine(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t ADV7533_FillRect(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color);
int32_t ADV7533_SetPixel(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Color);
int32_t ADV7533_GetPixel(ADV7533_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t *Color);
int32_t ADV7533_GetXSize(ADV7533_Object_t *pObj, uint32_t *XSize);
int32_t ADV7533_GetYSize(ADV7533_Object_t *pObj, uint32_t *YSize);
/*------------------------------------------------------------------------------
                           HDMI Audio functions 
------------------------------------------------------------------------------*/
int32_t ADV7533_AudioInit(ADV7533_Object_t *pObj, ADV7533_Init_t *pInit);
int32_t ADV7533_DeInit(ADV7533_Object_t *pObj);
int32_t ADV7533_ReadID(ADV7533_Object_t *pObj, uint32_t *Id);
int32_t ADV7533_Play(ADV7533_Object_t *pObj);
int32_t ADV7533_Pause(ADV7533_Object_t *pObj);
int32_t ADV7533_Resume(ADV7533_Object_t *pObj);
int32_t ADV7533_Stop(ADV7533_Object_t *pObj, uint32_t CodecPdwnMode);
int32_t ADV7533_SetVolume(ADV7533_Object_t *pObj, uint8_t Volume, uint8_t InputOutput);
int32_t ADV7533_GetVolume(ADV7533_Object_t *pObj, uint8_t InputOutput);
int32_t ADV7533_SetMute(ADV7533_Object_t *pObj, uint16_t Cmd);
int32_t ADV7533_SetOutputMode(ADV7533_Object_t *pObj, uint16_t Output);
int32_t ADV7533_SetResolution(ADV7533_Object_t *pObj, uint32_t Resolution);
int32_t ADV7533_GetResolution(ADV7533_Object_t *pObj, uint32_t *Resolution);
int32_t ADV7533_SetProtocol(ADV7533_Object_t *pObj, uint16_t Protocol);
int32_t ADV7533_GetProtocol(ADV7533_Object_t *pObj);
int32_t ADV7533_SetFrequency(ADV7533_Object_t *pObj, uint32_t AudioFreq);
int32_t ADV7533_GetFrequency(ADV7533_Object_t *pObj, uint32_t *Frequency);
int32_t ADV7533_Reset(ADV7533_Object_t *pObj);

/**
  * @}
  */ 

/* Audio driver structure */
extern ADV7533_Drv_t ADV7533_Driver;
/* LCD driver structure */
extern ADV7533_LcdDrv_t ADV7533_LCD_Driver;
#endif /* ADV7533_H */
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
