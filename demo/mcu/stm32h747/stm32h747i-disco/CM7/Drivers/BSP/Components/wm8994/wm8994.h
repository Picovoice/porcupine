/**
  ******************************************************************************
  * @file    wm8994.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the 
  *          wm8994.c driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2014 STMicroelectronics.
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
#ifndef WM8994_H
#define WM8994_H

/* Includes ------------------------------------------------------------------*/
#include "wm8994_reg.h"
#include <stddef.h>

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Component
  * @{
  */ 

/** @addtogroup WM8994
  * @{
  */
  
/** @defgroup WM8994_Exported_Types Exported Types
  * @{
  */   
typedef int32_t (*WM8994_Init_Func)    (void);
typedef int32_t (*WM8994_DeInit_Func)  (void);
typedef int32_t (*WM8994_GetTick_Func) (void);
typedef int32_t (*WM8994_Delay_Func)   (uint32_t);
typedef int32_t (*WM8994_WriteReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*WM8994_ReadReg_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  WM8994_Init_Func          Init;
  WM8994_DeInit_Func        DeInit;
  uint16_t                  Address;  
  WM8994_WriteReg_Func      WriteReg;
  WM8994_ReadReg_Func       ReadReg; 
  WM8994_GetTick_Func       GetTick; 
} WM8994_IO_t;

 
typedef struct
{
  WM8994_IO_t         IO;
  wm8994_ctx_t        Ctx;   
  uint8_t             IsInitialized;
} WM8994_Object_t;

typedef struct
{
  uint32_t   InputDevice;
  uint32_t   OutputDevice;
  uint32_t   Frequency;
  uint32_t   Resolution;
  uint32_t   Volume;  
} WM8994_Init_t;

 typedef struct
{
  int32_t ( *Init                 ) ( WM8994_Object_t *, WM8994_Init_t* );
  int32_t ( *DeInit               ) ( WM8994_Object_t * );
  int32_t ( *ReadID               ) ( WM8994_Object_t *, uint32_t * ); 
  int32_t ( *Play                 ) ( WM8994_Object_t * );
  int32_t ( *Pause                ) ( WM8994_Object_t * );
  int32_t ( *Resume               ) ( WM8994_Object_t * );
  int32_t ( *Stop                 ) ( WM8994_Object_t *, uint32_t );
  int32_t ( *SetFrequency         ) ( WM8994_Object_t *, uint32_t );
  int32_t ( *GetFrequency         ) ( WM8994_Object_t *, uint32_t*);  
  int32_t ( *SetVolume            ) ( WM8994_Object_t *, uint32_t, uint8_t );
  int32_t ( *GetVolume            ) ( WM8994_Object_t *, uint32_t, uint8_t*);  
  int32_t ( *SetMute              ) ( WM8994_Object_t *, uint32_t );
  int32_t ( *SetOutputMode        ) ( WM8994_Object_t *, uint32_t ); 
  int32_t ( *SetResolution        ) ( WM8994_Object_t *, uint32_t );
  int32_t ( *GetResolution        ) ( WM8994_Object_t *, uint32_t*);  
  int32_t ( *SetProtocol          ) ( WM8994_Object_t *, uint32_t );
  int32_t ( *GetProtocol          ) ( WM8994_Object_t *, uint32_t*);  
  int32_t ( *Reset                ) ( WM8994_Object_t * );
} WM8994_Drv_t;
/**
  * @}
  */ 

/** @defgroup WM8994_Exported_Constants Exported Constants
  * @{
  */ 
#define WM8994_OK                (0)
#define WM8994_ERROR             (-1)

/******************************************************************************/
/***************************  Codec User defines ******************************/
/******************************************************************************/
/* Audio Input Device */
#define WM8994_IN_NONE           0x00U 
#define WM8994_IN_MIC1           0x01U
#define WM8994_IN_MIC2           0x02U
#define WM8994_IN_LINE1          0x03U
#define WM8994_IN_LINE2          0x04U
#define WM8994_IN_MIC1_MIC2      0x05U

/* Audio Output Device */
#define WM8994_OUT_NONE          0x00U
#define WM8994_OUT_SPEAKER       0x01U
#define WM8994_OUT_HEADPHONE     0x02U
#define WM8994_OUT_BOTH          0x03U
#define WM8994_OUT_AUTO          0x04U

/* AUDIO FREQUENCY */
#define WM8994_FREQUENCY_192K    192000
#define WM8994_FREQUENCY_176K    176400
#define WM8994_FREQUENCY_96K     96000
#define WM8994_FREQUENCY_88K     88200  
#define WM8994_FREQUENCY_48K     48000
#define WM8994_FREQUENCY_44K     44100
#define WM8994_FREQUENCY_32K     32000
#define WM8994_FREQUENCY_22K     22050
#define WM8994_FREQUENCY_16K     16000
#define WM8994_FREQUENCY_11K     11025
#define WM8994_FREQUENCY_8K      8000  

/* AUDIO RESOLUTION */
#define WM8994_RESOLUTION_16b    0x00U
#define WM8994_RESOLUTION_20b    0x01U
#define WM8994_RESOLUTION_24b    0x02U
#define WM8994_RESOLUTION_32b    0x03U

/* Codec stop options */
#define WM8994_PDWN_HW           0x00U
#define WM8994_PDWN_SW           0x01U

/* Volume Input Output selection */
#define VOLUME_INPUT                  0U
#define VOLUME_OUTPUT                 1U

/* MUTE commands */
#define WM8994_MUTE_ON                1U
#define WM8994_MUTE_OFF               0U

/* AUDIO PROTOCOL */
#define WM8994_PROTOCOL_R_JUSTIFIED    ((uint16_t)0x0000)
#define WM8994_PROTOCOL_L_JUSTIFIED    ((uint16_t)0x0001)
#define WM8994_PROTOCOL_I2S            ((uint16_t)0x0002)
#define WM8994_PROTOCOL_DSP            ((uint16_t)0x0003)

#define VOLUME_OUT_INVERT(Volume)     ((uint8_t)(((Volume) * 100) / 63))
#define VOLUME_IN_INVERT(Volume)      ((uint8_t)(((Volume) * 100) / 239))

/** 
  * @brief  WM8994 ID  
  */  
#define  WM8994_ID    0x8994U

/**
  * @}
  */ 
  
/** @addtogroup WM8994_Exported_Variables
  * @{
  */ 
/* Audio driver structure */
extern WM8994_Drv_t WM8994_Driver;  
/**
  * @}
  */ 

/** @addtogroup WM8994_Exported_Functions
  * @{
  */
    
/*------------------------------------------------------------------------------
                           Audio Codec functions 
------------------------------------------------------------------------------*/
/* High Layer codec functions */
int32_t WM8994_RegisterBusIO (WM8994_Object_t *pObj, WM8994_IO_t *pIO);
int32_t WM8994_Init(WM8994_Object_t *pObj, WM8994_Init_t *pInit);
int32_t WM8994_DeInit(WM8994_Object_t *pObj);
int32_t WM8994_ReadID(WM8994_Object_t *pObj, uint32_t *Id);
int32_t WM8994_Play(WM8994_Object_t *pObj);
int32_t WM8994_Pause(WM8994_Object_t *pObj);
int32_t WM8994_Resume(WM8994_Object_t *pObj);
int32_t WM8994_Stop(WM8994_Object_t *pObj, uint32_t CodecPdwnMode);
int32_t WM8994_SetVolume(WM8994_Object_t *pObj, uint32_t InputOutput, uint8_t Volume);
int32_t WM8994_GetVolume(WM8994_Object_t *pObj, uint32_t InputOutput, uint8_t *Volume);
int32_t WM8994_SetMute(WM8994_Object_t *pObj, uint32_t Cmd);
int32_t WM8994_SetOutputMode(WM8994_Object_t *pObj, uint32_t Output);
int32_t WM8994_SetResolution(WM8994_Object_t *pObj, uint32_t Resolution);
int32_t WM8994_GetResolution(WM8994_Object_t *pObj, uint32_t *Resolution);
int32_t WM8994_SetProtocol(WM8994_Object_t *pObj, uint32_t Protocol);
int32_t WM8994_GetProtocol(WM8994_Object_t *pObj, uint32_t *Protocol);
int32_t WM8994_SetFrequency(WM8994_Object_t *pObj, uint32_t AudioFreq);
int32_t WM8994_GetFrequency(WM8994_Object_t *pObj, uint32_t *AudioFreq);
int32_t WM8994_Reset(WM8994_Object_t *pObj);
/**
  * @}
  */ 

#endif /* WM8994_H */


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
