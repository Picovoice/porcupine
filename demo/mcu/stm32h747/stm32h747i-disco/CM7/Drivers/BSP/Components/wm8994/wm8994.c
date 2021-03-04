/**
  ******************************************************************************
  * @file    wm8994.c
  * @author  MCD Application Team
  * @brief   This file provides the WM8994 Audio Codec driver.
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

/* Includes ------------------------------------------------------------------*/
#include "wm8994.h"

/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup Components
  * @{
  */ 

/** @defgroup WM8994 WM8994
  * @brief     This file provides a set of functions needed to drive the 
  *            WM8994 audio codec.
  * @{
  */

/** @defgroup WM8994_Private_Types Private Types
  * @{
  */
/* Audio codec driver structure initialization */  
WM8994_Drv_t WM8994_Driver = 
{
  WM8994_Init,
  WM8994_DeInit,
  WM8994_ReadID,
  WM8994_Play,
  WM8994_Pause,
  WM8994_Resume,
  WM8994_Stop,
  WM8994_SetFrequency,
  WM8994_GetFrequency,  
  WM8994_SetVolume,
  WM8994_GetVolume,  
  WM8994_SetMute,
  WM8994_SetOutputMode,
  WM8994_SetResolution,
  WM8994_GetResolution,  
  WM8994_SetProtocol,
  WM8994_GetProtocol,  
  WM8994_Reset
};

/**
  * @}
  */ 

/** @defgroup WM8994_Function_Prototypes Function Prototypes
  * @{
  */
static int32_t WM8994_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* Data, uint16_t Length);
static int32_t WM8994_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* Data, uint16_t Length);
static int32_t WM8994_Delay(WM8994_Object_t *pObj, uint32_t Delay);

/**
  * @}
  */ 

/** @defgroup WM8994_Exported_Functions Exported Functions
  * @{
  */ 

/**
  * @brief Initializes the audio codec and the control interface.
  * @param pObj pointer to component object
  * @param pInit pointer de component init structure
  * @retval 0 if correct communication, else wrong communication
  */
int32_t WM8994_Init(WM8994_Object_t *pObj, WM8994_Init_t *pInit)
{
  int32_t ret;
  static uint8_t ColdStartup = 1;
  uint16_t tmp;
  
  /* wm8994 Errata Work-Arounds */
  tmp = 0x0003;
  ret = wm8994_write_reg(&pObj->Ctx, 0x102, &tmp, 2);
  tmp = 0x0000;
  ret += wm8994_write_reg(&pObj->Ctx, 0x817, &tmp, 2);
  ret += wm8994_write_reg(&pObj->Ctx, 0x102, &tmp, 2);
  
  /* Enable VMID soft start (fast), Start-up Bias Current Enabled: 0x006C at reg 0x39 */
  /* Bias Enable */
  tmp = 0x006C;
  ret += wm8994_write_reg(&pObj->Ctx, WM8994_ANTIPOP2, &tmp, 2);
  
  /* Enable bias generator, Enable VMID */
  if (pInit->InputDevice != WM8994_IN_NONE)
  {
    tmp = 0x0013;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);  
  }
  else
  { 
    tmp = 0x0003;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
  }
  
  /* Add Delay */
  (void)WM8994_Delay(pObj, 50);
  
  /* Path Configurations for output */
  switch (pInit->OutputDevice)
  {
  case WM8994_OUT_SPEAKER: 
    /* Enable DAC1 (Left), Enable DAC1 (Right),
    Disable DAC2 (Left), Disable DAC2 (Right)*/
    tmp = 0x0C0C;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
    
    /* Disable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
    tmp = 0x0000;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
    
    /* Disable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
    
    /* Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
    tmp = 0x0002;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
    
    /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
    break;
    
  case WM8994_OUT_HEADPHONE:    
    /* Disable DAC1 (Left), Disable DAC1 (Right),
    Enable DAC2 (Left), Enable DAC2 (Right)*/
    tmp = 0x0303;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
    
    /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
    tmp = 0x0001;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
    
    /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
    
    /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
    tmp = 0x0000;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
    
    /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
    break;
    
  case WM8994_OUT_BOTH:
    if (pInit->InputDevice == WM8994_IN_MIC1_MIC2)
    {
      /* Enable DAC1 (Left), Enable DAC1 (Right),
      also Enable DAC2 (Left), Enable DAC2 (Right)*/
      tmp = 0x0F0F;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path
      Enable the AIF1 Timeslot 1 (Left) to DAC 1 (Left) mixer path */
      tmp = 0x0003;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path
      Enable the AIF1 Timeslot 1 (Right) to DAC 1 (Right) mixer path */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 0 (Left) to DAC 2 (Left) mixer path
      Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path  */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 0 (Right) to DAC 2 (Right) mixer path
      Enable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */       
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
    }
    else
    {
      /* Enable DAC1 (Left), Enable DAC1 (Right),
      also Enable DAC2 (Left), Enable DAC2 (Right)*/
      tmp = 0x0F0F;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
      tmp = 0x0001;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
      tmp = 0x0002;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
    }
    break;
    
  case WM8994_OUT_NONE:
    break;
  case WM8994_OUT_AUTO :
  default:
    /* Disable DAC1 (Left), Disable DAC1 (Right),
    Enable DAC2 (Left), Enable DAC2 (Right)*/
    tmp = 0x0303;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
    
    /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
    tmp = 0x0001;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
    
    /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
    
    /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
    tmp = 0x0000;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
    
    /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
    break;
  }
  
  /* Path Configurations for input */
  switch (pInit->InputDevice)
  {
  case WM8994_IN_MIC2 :
    /* Enable AIF1ADC2 (Left), Enable AIF1ADC2 (Right)
    * Enable DMICDAT2 (Left), Enable DMICDAT2 (Right)
    * Enable Left ADC, Enable Right ADC */
    tmp = 0x0C30;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
    
    /* Enable AIF1 DRC2 Signal Detect & DRC in AIF1ADC2 Left/Right Timeslot 1 */
    tmp = 0x00DB;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DRC2, &tmp, 2);
    
    /* Disable IN1L, IN1R, IN2L, IN2R, Enable Thermal sensor & shutdown */
    tmp = 0x6000;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
    
    /* Enable the DMIC2(Left) to AIF1 Timeslot 1 (Left) mixer path */
    tmp = 0x0002;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC2_LMR, &tmp, 2);
    
    /* Enable the DMIC2(Right) to AIF1 Timeslot 1 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC2_RMR, &tmp, 2);
    
    /* GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC2 signal detect */
    tmp = 0x000E;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_GPIO1, &tmp, 2);
    break;
    
  case WM8994_IN_LINE1 :
    /* IN1LN_TO_IN1L, IN1RN_TO_IN1R */
    tmp = 0x0011;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_INPUT_MIXER_2, &tmp, 2);
    
    /* Disable mute on IN1L_TO_MIXINL and +30dB on IN1L PGA output */
    tmp = 0x0035;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_INPUT_MIXER_3, &tmp, 2);
    
    /* Disable mute on IN1R_TO_MIXINL, Gain = +30dB */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_INPUT_MIXER_4, &tmp, 2);
    
    /* Enable AIF1ADC1 (Left), Enable AIF1ADC1 (Right)
    * Enable Left ADC, Enable Right ADC */
    tmp = 0x0303;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
    
    /* Enable AIF1 DRC1 Signal Detect & DRC in AIF1ADC1 Left/Right Timeslot 0 */
    tmp = 0x00DB;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DRC1, &tmp, 2);
    
    /* Enable IN1L and IN1R, Disable IN2L and IN2R, Enable Thermal sensor & shutdown */
    tmp = 0x6350;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
    
    /* Enable the ADCL(Left) to AIF1 Timeslot 0 (Left) mixer path */
    tmp = 0x0002;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC1_LMR, &tmp, 2);
    
    /* Enable the ADCR(Right) to AIF1 Timeslot 0 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC1_RMR, &tmp, 2);
    
    /* GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC1 signal detect */
    tmp = 0x800D;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_GPIO1, &tmp, 2);
    break;
    
  case WM8994_IN_MIC1 :
    /* Enable AIF1ADC1 (Left), Enable AIF1ADC1 (Right)
    * Enable DMICDAT1 (Left), Enable DMICDAT1 (Right)
    * Enable Left ADC, Enable Right ADC */
    tmp = 0x030C;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
    
    /* Enable AIF1 DRC1 Signal Detect & DRC in AIF1ADC1 Left/Right Timeslot 0 */
    tmp = 0x00DB;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DRC1, &tmp, 2);
    
    /* Enable IN1L and IN1R, Disable IN2L and IN2R, Enable Thermal sensor & shutdown */
    tmp = 0x6350;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
    
    /* Enable the ADCL(Left) to AIF1 Timeslot 0 (Left) mixer path */
    tmp = 0x0002;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC1_LMR, &tmp, 2);
    
    /* Enable the ADCR(Right) to AIF1 Timeslot 0 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC1_RMR, &tmp, 2);
    
    /* GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC1 signal detect */
    tmp = 0x000D;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_GPIO1, &tmp, 2);
    break;
    
  case WM8994_IN_MIC1_MIC2 :
    /* Enable AIF1ADC1 (Left), Enable AIF1ADC1 (Right)
    * Enable DMICDAT1 (Left), Enable DMICDAT1 (Right)
    * Enable Left ADC, Enable Right ADC */
    tmp = 0x0F3C;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
    
    /* Enable AIF1 DRC2 Signal Detect & DRC in AIF1ADC2 Left/Right Timeslot 1 */
    tmp = 0x00DB;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DRC2, &tmp, 2);
    
    /* Enable AIF1 DRC2 Signal Detect & DRC in AIF1ADC1 Left/Right Timeslot 0 */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DRC1, &tmp, 2);
    
    /* Disable IN1L, IN1R, Enable IN2L, IN2R, Thermal sensor & shutdown */
    tmp = 0x63A0;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
    
    /* Enable the ADCL(Left) to AIF1 Timeslot 0 (Left) mixer path */
    tmp = 0x0002;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC1_LMR, &tmp, 2);
    
    /* Enable the ADCR(Right) to AIF1 Timeslot 0 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC1_RMR, &tmp, 2);
    
    /* Enable the DMIC2(Left) to AIF1 Timeslot 1 (Left) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC2_LMR, &tmp, 2);
    
    /* Enable the DMIC2(Right) to AIF1 Timeslot 1 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC2_RMR, &tmp, 2);
    
    /* GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC1 signal detect */
    tmp = 0x000D;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_GPIO1, &tmp, 2);
    
    break;    
  case WM8994_IN_LINE2 :
  case WM8994_IN_NONE:      
  default:
    /* Actually, no other input devices supported */
    break;
  }
  
  /*  Clock Configurations */
  ret += WM8994_SetFrequency(pObj, pInit->Frequency);
  
  if(pInit->InputDevice == WM8994_IN_MIC1_MIC2)
  {
    /* AIF1 Word Length = 16-bits, AIF1 Format = DSP mode */
    ret += WM8994_SetResolution(pObj, WM8994_RESOLUTION_16b);
    ret += WM8994_SetProtocol(pObj, WM8994_PROTOCOL_DSP);
    ret += wm8994_aif1_control1_adcr_src(&pObj->Ctx, 1);
  }
  else
  {
    /* AIF1 Word Length = 16-bits, AIF1 Format = I2S (Default Register Value) */
    ret += WM8994_SetResolution(pObj, pInit->Resolution);
    ret += WM8994_SetProtocol(pObj, WM8994_PROTOCOL_I2S);
    ret += wm8994_aif1_control1_adcr_src(&pObj->Ctx, 1);
  }
  
  /* slave mode */
  tmp = 0x0000;
  ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_MASTER_SLAVE, &tmp, 2);
  
  /* Enable the DSP processing clock for AIF1, Enable the core clock */
  tmp = 0x000A;
  ret += wm8994_write_reg(&pObj->Ctx, WM8994_CLOCKING1, &tmp, 2);
  
  /* Enable AIF1 Clock, AIF1 Clock Source = MCLK1 pin */
  tmp = 0x0001;
  ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_CLOCKING1, &tmp, 2);
  
  if (pInit->OutputDevice != WM8994_OUT_NONE)  /* Audio output selected */  
  {  
    if ((pInit->OutputDevice == WM8994_OUT_HEADPHONE) && (pInit->InputDevice == WM8994_IN_NONE))
    {    
      tmp = 0x0100;
      /* Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
      
      /* Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
      
      /* Startup sequence for Headphone */
      if(ColdStartup == 1U)
      {
        /* Enable/Start the write sequencer */
        tmp = 0x8100;
        ret += wm8994_write_reg(&pObj->Ctx, WM8994_WRITE_SEQ_CTRL1, &tmp, 2);
        
        ColdStartup=0;
        /* Add Delay */
        (void)WM8994_Delay(pObj, 325);
      }
      else 
      { 
        /* Headphone Warm Start-Up */
        tmp = 0x8108;
        ret += wm8994_write_reg(&pObj->Ctx, WM8994_WRITE_SEQ_CTRL1, &tmp, 2);
        
        /* Add Delay */
        (void)WM8994_Delay(pObj, 50);
      }
      
      /* Soft un-Mute the AIF1 Timeslot 0 DAC1 path L&R */
      tmp = 0x0000;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);
    }
    else
    {
      /* Analog Output Configuration */
      
      /* Enable SPKRVOL PGA, Enable SPKMIXR, Enable SPKLVOL PGA, Enable SPKMIXL */
      tmp = 0x0300;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
      
      /* Left Speaker Mixer Volume = 0dB */
      tmp = 0x0000;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_SPKMIXL_ATT, &tmp, 2);
      
      /* Speaker output mode = Class D, Right Speaker Mixer Volume = 0dB ((0x23, 0x0100) = class AB)*/
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_SPKMIXR_ATT, &tmp, 2);
      
      /* Unmute DAC2 (Left) to Left Speaker Mixer (SPKMIXL) path,
      Unmute DAC2 (Right) to Right Speaker Mixer (SPKMIXR) path */
      tmp = 0x0300;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_SPEAKER_MIXER, &tmp, 2);
      
      /* Enable bias generator, Enable VMID, Enable SPKOUTL, Enable SPKOUTR */
      tmp = 0x3003;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
      /* Headphone/Speaker Enable */
      
      if (pInit->InputDevice == WM8994_IN_MIC1_MIC2)
      {
        /* Enable Class W, Class W Envelope Tracking = AIF1 Timeslots 0 and 1 */
        tmp = 0x0205;
        ret += wm8994_write_reg(&pObj->Ctx, WM8994_CLASS_W, &tmp, 2); 
      }
      else
      {
        /* Enable Class W, Class W Envelope Tracking = AIF1 Timeslot 0 */
        tmp = 0x0005;
        ret += wm8994_write_reg(&pObj->Ctx, WM8994_CLASS_W, &tmp, 2);
      }
      
      /* Enable bias generator, Enable VMID, Enable HPOUT1 (Left) and Enable HPOUT1 (Right) input stages */
      /* idem for Speaker */
      tmp = 0x3303;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
      
      /* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate stages */
      tmp = 0x0022;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_ANALOG_HP, &tmp, 2);
      
      /* Enable Charge Pump */
      tmp = 0x9F25;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_CHARGE_PUMP1, &tmp, 2);
      
      /* Add Delay */
      (void)WM8994_Delay(pObj, 15);
      
      tmp = 0x0001;
      /* Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
      
      /* Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
      
      /* Enable Left Output Mixer (MIXOUTL), Enable Right Output Mixer (MIXOUTR) */
      /* idem for SPKOUTL and SPKOUTR */
      tmp = 0x0330;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
      
      /* Enable DC Servo and trigger start-up mode on left and right channels */
      tmp = 0x0033;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_DC_SERVO1, &tmp, 2);
      
      /* Add Delay */
      (void)WM8994_Delay(pObj, 257);
      
      /* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate and output stages. Remove clamps */
      tmp = 0x00EE;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_ANALOG_HP, &tmp, 2);
    }
    
    /* Unmutes */
    
    /* Unmute DAC 1 (Left) */
    tmp = 0x00C0;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_DAC1_LEFT_VOL, &tmp, 2);
    
    /* Unmute DAC 1 (Right) */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_DAC1_RIGHT_VOL, &tmp, 2);
    
    /* Unmute the AIF1 Timeslot 0 DAC path */
    tmp = 0x0010;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);
    
    /* Unmute DAC 2 (Left) */
    tmp = 0x00C0;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_DAC2_LEFT_VOL, &tmp, 2);
    
    /* Unmute DAC 2 (Right) */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_DAC2_RIGHT_VOL, &tmp, 2);
    
    /* Unmute the AIF1 Timeslot 1 DAC2 path */
    tmp = 0x0010;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);
    
    /* Volume Control */
    ret += WM8994_SetVolume(pObj, VOLUME_OUTPUT, (uint8_t)pInit->Volume); 
  }
  
  if (pInit->InputDevice != WM8994_IN_NONE) /* Audio input selected */
  {
    if ((pInit->InputDevice == WM8994_IN_MIC1) || (pInit->InputDevice == WM8994_IN_MIC2))
    {
      /* Enable Microphone bias 1 generator, Enable VMID */
      tmp = 0x0013;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
      
      /* ADC oversample enable */
      tmp = 0x0002;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_OVERSAMPLING, &tmp, 2);
      
      /* AIF ADC2 HPF enable, HPF cut = voice mode 1 fc=127Hz at fs=8kHz */
      tmp = 0x3800;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC2_FILTERS, &tmp, 2);
    }
    else if(pInit->InputDevice == WM8994_IN_MIC1_MIC2)
    {
      /* Enable Microphone bias 1 generator, Enable VMID */
      tmp = 0x0013;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
      
      /* ADC oversample enable */
      tmp = 0x0002;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_OVERSAMPLING, &tmp, 2);
      
      /* AIF ADC1 HPF enable, HPF cut = voice mode 1 fc=127Hz at fs=8kHz */
      tmp = 0x1800;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC1_FILTERS, &tmp, 2);
      
      /* AIF ADC2 HPF enable, HPF cut = voice mode 1 fc=127Hz at fs=8kHz */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC2_FILTERS, &tmp, 2);
    }    
    else /* ((pInit->InputDevice == WM8994_IN_LINE1) || (pInit->InputDevice == WM8994_IN_LINE2)) */
    {      
      /* Disable mute on IN1L, IN1L Volume = +0dB */
      tmp = 0x000B;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_LEFT_LINE_IN12_VOL, &tmp, 2);
      
      /* Disable mute on IN1R, IN1R Volume = +0dB */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_RIGHT_LINE_IN12_VOL, &tmp, 2);
      
      /* AIF ADC1 HPF enable, HPF cut = voice mode 1 fc=127Hz at fs=8kHz */
      tmp = 0x1800;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC1_FILTERS, &tmp, 2);
    }
    /* Volume Control */
    ret += WM8994_SetVolume(pObj, VOLUME_INPUT, (uint8_t)pInit->Volume); 
  }
  
  if(ret != WM8994_OK)
  {
    ret = WM8994_ERROR;
  }
  
  return ret;
}
                         
/**
  * @brief  Deinitializes the audio codec.
  * @param  pObj pointer to component object
  * @retval Component status
  */
int32_t WM8994_DeInit(WM8994_Object_t *pObj)
{
  /* De-Initialize Audio Codec interface */
  return WM8994_Stop(pObj, WM8994_PDWN_HW);
}

/**
  * @brief  Get the WM8994 ID.
  * @param  pObj pointer to component object
  * @param  Id component ID
  * @retval Component status 
  */
int32_t WM8994_ReadID(WM8994_Object_t *pObj, uint32_t *Id)
{
  int32_t ret;
  uint16_t wm8994_id;
  
  /* Initialize the Control interface of the Audio Codec */
  pObj->IO.Init();
  /* Get ID from component */
  ret = wm8994_sw_reset_r(&pObj->Ctx, &wm8994_id);
  
  *Id = wm8994_id;
  
  return ret;
}

/**
  * @brief Start the audio Codec play feature.
  * @note For this codec no Play options are required.
  * @param  pObj pointer to component object  
  * @retval Component status
  */
int32_t WM8994_Play(WM8994_Object_t *pObj)
{ 
  /* Resumes the audio file playing */  
  /* Unmute the output first */
  return WM8994_SetMute(pObj, WM8994_MUTE_OFF);
}

/**
  * @brief Pauses playing on the audio codec.
  * @param  pObj pointer to component object
  * @retval Component status
  */
int32_t WM8994_Pause(WM8994_Object_t *pObj)
{
  int32_t ret;
  uint16_t tmp = 0x0001;
  
  /* Pause the audio file playing */
  /* Mute the output first */
  if(WM8994_SetMute(pObj, WM8994_MUTE_ON) != WM8994_OK)
  {
    ret  = WM8994_ERROR;
  }/* Put the Codec in Power save mode */
  else if(wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2) != WM8994_OK)
  {
    ret  = WM8994_ERROR;
  }
  else
  {
    ret = WM8994_OK;
  }
  
  return ret;
}

/**
  * @brief Resumes playing on the audio codec.
  * @param  pObj pointer to component object 
  * @retval Component status
  */
int32_t WM8994_Resume(WM8994_Object_t *pObj)
{  
  /* Resumes the audio file playing */  
  /* Unmute the output first */
  return WM8994_SetMute(pObj, WM8994_MUTE_OFF);
}

/**
  * @brief Stops audio Codec playing. It powers down the codec.
  * @param  pObj pointer to component object 
  * @param CodecPdwnMode  selects the  power down mode.
  *          - WM8994_PDWN_SW: only mutes the audio codec. When resuming from this 
  *                           mode the codec keeps the previous initialization
  *                           (no need to re-Initialize the codec registers).
  *          - WM8994_PDWN_HW: Physically power down the codec. When resuming from this
  *                           mode, the codec is set to default configuration 
  *                           (user should re-Initialize the codec in order to 
  *                            play again the audio stream).
  * @retval 0 if correct communication, else wrong communication
  */
int32_t WM8994_Stop(WM8994_Object_t *pObj, uint32_t CodecPdwnMode)
{
  int32_t ret;
  uint16_t tmp;
  
  /* Mute the output first */
  ret = WM8994_SetMute(pObj, WM8994_MUTE_ON);
  
  if (CodecPdwnMode == WM8994_PDWN_SW)
  {
    /* Only output mute required*/
  }
  else /* WM8994_PDWN_HW */
  {
    tmp = 0x0200;
    /* Mute the AIF1 Timeslot 0 DAC1 path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);
    
    /* Mute the AIF1 Timeslot 1 DAC2 path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);
    
    tmp = 0x0000;
    /* Disable DAC1L_TO_HPOUT1L */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
    
    /* Disable DAC1R_TO_HPOUT1R */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
    
    /* Disable DAC1 and DAC2 */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
    
    /* Reset Codec by writing in 0x0000 address register */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_SW_RESET, &tmp, 2);
  }
  
  if(ret != WM8994_OK)
  {
    ret = WM8994_ERROR;
  }
  
  return ret;
}

/**
  * @brief Set higher or lower the codec volume level.
  * @param  pObj pointer to component object
  * @param  InputOutput Input or Output volume
  * @param  Volume  a byte value from 0 to 63 for output and from 0 to 240 for input
  *         (refer to codec registers description for more details).
  * @retval Component status
  */
int32_t WM8994_SetVolume(WM8994_Object_t *pObj, uint32_t InputOutput, uint8_t Volume)
{
  int32_t ret;  
  uint16_t tmp;
  
  /* Output volume */
  if (InputOutput == VOLUME_OUTPUT)
  {    
    if(Volume > 0x3EU)
    {
      /* Unmute audio codec */
      ret = WM8994_SetMute(pObj, WM8994_MUTE_OFF);
      tmp = 0x3FU | 0x140U;
      
      /* Left Headphone Volume */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_LEFT_OUTPUT_VOL, &tmp, 2);
      
      /* Right Headphone Volume */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_RIGHT_OUTPUT_VOL, &tmp, 2);
      
      /* Left Speaker Volume */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_SPK_LEFT_VOL, &tmp, 2);
      
      /* Right Speaker Volume */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_SPK_RIGHT_VOL, &tmp, 2);
    }
    else if (Volume == 0U)
    {
      /* Mute audio codec */
      ret = WM8994_SetMute(pObj, WM8994_MUTE_ON);
    }
    else
    {
      /* Unmute audio codec */
      ret = WM8994_SetMute(pObj, WM8994_MUTE_OFF);
      
      tmp = Volume | 0x140U;
      
      /* Left Headphone Volume */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_LEFT_OUTPUT_VOL, &tmp, 2);
      
      /* Right Headphone Volume */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_RIGHT_OUTPUT_VOL, &tmp, 2);
      
      /* Left Speaker Volume */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_SPK_LEFT_VOL, &tmp, 2);
      
      /* Right Speaker Volume */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_SPK_RIGHT_VOL, &tmp, 2);      
    }
  }
  else /* Input volume: VOLUME_INPUT */
  {
    tmp = Volume | 0x100U;
    
    /* Left AIF1 ADC1 volume */
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC1_LEFT_VOL, &tmp, 2); 
    
    /* Right AIF1 ADC1 volume */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC1_RIGHT_VOL, &tmp, 2); 
    
    /* Left AIF1 ADC2 volume */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC2_LEFT_VOL, &tmp, 2); 
    
    /* Right AIF1 ADC2 volume */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_ADC2_RIGHT_VOL, &tmp, 2); 
  }
  
  if(ret != WM8994_OK)
  {
    ret = WM8994_ERROR;
  }
  
  return ret;
}

/**
  * @brief Get higher or lower the codec volume level.
  * @param  pObj pointer to component object
  * @param  InputOutput Input or Output volume
  * @param  Volume audio volume 
  * @retval Component status
  */
int32_t WM8994_GetVolume(WM8994_Object_t *pObj, uint32_t InputOutput, uint8_t *Volume)
{
  int32_t ret = WM8994_OK;  
  uint16_t invertedvol;
  
  /* Output volume */
  if (InputOutput == VOLUME_OUTPUT)
  {
    if(wm8994_lo_hpout1l_vol_r(&pObj->Ctx, &invertedvol) != WM8994_OK)
    {
      ret = WM8994_ERROR;
    }
    else
    {
      *Volume = VOLUME_OUT_INVERT(invertedvol);
    }
  }
  else /* Input volume: VOLUME_INPUT */
  {
    if(wm8994_aif1_adc1_left_vol_adc1l_r(&pObj->Ctx, &invertedvol) != WM8994_OK)
    {
      ret = WM8994_ERROR;
    }
    else
    {
      *Volume = VOLUME_IN_INVERT(invertedvol);
    }
  }
  
  return ret;
}

/**
  * @brief Enables or disables the mute feature on the audio codec.
  * @param  pObj pointer to component object   
  * @param Cmd  WM8994_MUTE_ON to enable the mute or WM8994_MUTE_OFF to disable the
  *             mute mode.
  * @retval 0 if correct communication, else wrong communication
  */
int32_t WM8994_SetMute(WM8994_Object_t *pObj, uint32_t Cmd)
{
  int32_t ret;
  uint16_t tmp;
  
  /* Set the Mute mode */
  if(Cmd == WM8994_MUTE_ON)
  { 
    tmp = 0x0200;
    /* Soft Mute the AIF1 Timeslot 0 DAC1 path L&R */
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);
    
    /* Soft Mute the AIF1 Timeslot 1 DAC2 path L&R */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);
  }
  else /* WM8994_MUTE_OFF Disable the Mute */
  {
    tmp = 0x0010;
    /* Unmute the AIF1 Timeslot 0 DAC1 path L&R */
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);
    
    /* Unmute the AIF1 Timeslot 1 DAC2 path L&R */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);
  }
  
  if(ret != WM8994_OK)
  {
    ret = WM8994_ERROR;
  }
  
  return ret;
}

/**
  * @brief Switch dynamically (while audio file is played) the output target 
  *         (speaker or headphone).
  * @param  pObj pointer to component object
  * @param Output  specifies the audio output target: WM8994_OUT_SPEAKER,
  *         WM8994_OUT_HEADPHONE, WM8994_OUT_BOTH or WM8994_OUT_AUTO 
  * @retval 0 if correct communication, else wrong communication
  */
int32_t WM8994_SetOutputMode(WM8994_Object_t *pObj, uint32_t Output)
{
  int32_t ret;
  uint16_t tmp;
  
  if((Output == WM8994_OUT_HEADPHONE) || (Output == WM8994_OUT_AUTO))
  {
    /* Disable bias generator, Enable VMID, Enable SPKOUTL, Enable SPKOUTR */
    tmp = 0x0000;
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
    
    /* Disable DAC1 (Left), Disable DAC1 (Right),
    Enable DAC2 (Left), Enable DAC2 (Right)*/
    tmp = 0x0303;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
    
    /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
    tmp = 0x0001;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
    
    /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
    
    /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
    tmp = 0x0000;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
    
    /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
    
    /* Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path */
    tmp = 0x0100;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
    
    /* Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
    
    /* Startup sequence for Headphone */
    /* Enable/Start the write sequencer */
    tmp = 0x8100;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_WRITE_SEQ_CTRL1, &tmp, 2);
    
    /* Add Delay */
    (void)WM8994_Delay(pObj, 300);
    
    /* Soft un-Mute the AIF1 Timeslot 0 DAC1 path L&R */
    tmp = 0x0000;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);    
  }
  else
  {    
    switch (Output) 
    {    
    case WM8994_OUT_SPEAKER: 
      /* Enable DAC1 (Left), Enable DAC1 (Right),
      Disable DAC2 (Left), Disable DAC2 (Right)*/
      tmp = 0x0C0C;
      ret = wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
      
      /* Disable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
      tmp = 0x0000;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
      
      /* Disable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
      tmp = 0x0002;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
      
      /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
      break;
      
    case WM8994_OUT_BOTH:
    default:  
      /* Enable DAC1 (Left), Enable DAC1 (Right),
      also Enable DAC2 (Left), Enable DAC2 (Right)*/
      tmp = 0x0F0F;
      ret = wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
      tmp = 0x0001;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
      
      /* Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
      tmp = 0x0002;
      ret += wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);      
      break;
    }
    
    /* Enable SPKRVOL PGA, Enable SPKMIXR, Enable SPKLVOL PGA, Enable SPKMIXL */
    tmp = 0x0300;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
    
    /* Left Speaker Mixer Volume = 0dB */
    tmp = 0x0000;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_SPKMIXL_ATT, &tmp, 2);
    
    /* Speaker output mode = Class D, Right Speaker Mixer Volume = 0dB ((0x23, 0x0100) = class AB)*/
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_SPKMIXR_ATT, &tmp, 2);
    
    /* Unmute DAC2 (Left) to Left Speaker Mixer (SPKMIXL) path,
    Unmute DAC2 (Right) to Right Speaker Mixer (SPKMIXR) path */
    tmp = 0x0300;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_SPEAKER_MIXER, &tmp, 2);
    
    /* Enable bias generator, Enable VMID, Enable SPKOUTL, Enable SPKOUTR */
    tmp = 0x3003;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
    /* Headphone/Speaker Enable */
    
    /* Enable Class W, Class W Envelope Tracking = AIF1 Timeslot 0 */
    tmp = 0x0005;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_CLASS_W, &tmp, 2);
    
    /* Enable bias generator, Enable VMID, Enable HPOUT1 (Left) and Enable HPOUT1 (Right) input stages */
    /* idem for Speaker */
    tmp = 0x3303;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
    
    /* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate stages */
    tmp = 0x0022;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_ANALOG_HP, &tmp, 2);
    
    /* Enable Charge Pump */
    tmp = 0x9F25;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_CHARGE_PUMP1, &tmp, 2);
    
    /* Add Delay */
    (void)WM8994_Delay(pObj, 15);
    
    /* Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path */
    tmp = 0x0001;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
    
    /* Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path */
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
    
    /* Enable Left Output Mixer (MIXOUTL), Enable Right Output Mixer (MIXOUTR) */
    /* idem for SPKOUTL and SPKOUTR */
    tmp = 0x0330;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
    
    /* Enable DC Servo and trigger start-up mode on left and right channels */
    tmp = 0x0033;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_DC_SERVO1, &tmp, 2);
    
    /* Add Delay */
    (void)WM8994_Delay(pObj, 257);
    
    /* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate and output stages. Remove clamps */
    tmp = 0x00EE;
    ret += wm8994_write_reg(&pObj->Ctx, WM8994_ANALOG_HP, &tmp, 2);    
  }
  
  if(ret != WM8994_OK)
  {
    ret = WM8994_ERROR;
  }
  
  return ret;
}
   
/**
  * @brief Set Audio resolution.
  * @param pObj pointer to component object
  * @param Resolution  Audio resolution. Can be:
  *                    WM8994_RESOLUTION_16b, WM8994_RESOLUTION_20b, 
  *                    WM8994_RESOLUTION_24b or WM8994_RESOLUTION_32b
  * @retval Component status
  */
int32_t WM8994_SetResolution(WM8994_Object_t *pObj, uint32_t Resolution)
{
  int32_t ret = WM8994_OK;
  
  if(wm8994_aif1_control1_wl(&pObj->Ctx, (uint16_t)Resolution) != WM8994_OK)
  {
    ret = WM8994_ERROR;
  }

  return ret;
}

/**
  * @brief Get Audio resolution.
  * @param pObj pointer to component object
  * @retval Audio resolution
  */
int32_t WM8994_GetResolution(WM8994_Object_t *pObj, uint32_t *Resolution)
{
  int32_t ret = WM8994_OK;  
  uint16_t resolution = 0;
  
  if(wm8994_aif1_control1_wl_r(&pObj->Ctx, &resolution) != WM8994_OK)
  {
    ret = WM8994_ERROR;
  }
  else
  {
    switch(resolution)
    {
    case 0:
      *Resolution = WM8994_RESOLUTION_16b;
      break;
    case 1:
      *Resolution = WM8994_RESOLUTION_20b;
      break;
    case 2:
      *Resolution = WM8994_RESOLUTION_24b;
      break;
    case 3:
      *Resolution = WM8994_RESOLUTION_32b;
      break; 
    default:
      *Resolution = WM8994_RESOLUTION_16b;
      break;
    }
  }
  
  return ret;
}

/**
  * @brief Set Audio Protocol.
  * @param pObj pointer to component object
  * @param Protocol Audio Protocol. Can be:
  *                  WM8994_PROTOCOL_R_JUSTIFIED, WM8994_PROTOCOL_L_JUSTIFIED, 
  *                  WM8994_PROTOCOL_I2S or WM8994_PROTOCOL_DSP
  * @retval Component status
  */
int32_t WM8994_SetProtocol(WM8994_Object_t *pObj, uint32_t Protocol)
{
  int32_t ret = WM8994_OK; 
  
  if(wm8994_aif1_control1_fmt(&pObj->Ctx, (uint16_t)Protocol) != WM8994_OK)
  {
    ret = WM8994_ERROR;
  }
  
  return ret;  
}

/**
  * @brief Get Audio Protocol.
  * @param pObj pointer to component object
  * @retval Component status
  */
int32_t WM8994_GetProtocol(WM8994_Object_t *pObj, uint32_t *Protocol)
{
  int32_t ret = WM8994_OK;  
  uint16_t protocol;
  
  if(wm8994_aif1_control1_fmt_r(&pObj->Ctx, &protocol) != WM8994_OK)
  {
    ret = WM8994_ERROR;
  }
  else
  {
    *Protocol = protocol;
  }
  
  return ret;
}

/**
  * @brief Sets new frequency.
  * @param pObj pointer to component object
  * @param AudioFreq Audio frequency
  * @retval Component status
  */
int32_t WM8994_SetFrequency(WM8994_Object_t *pObj, uint32_t AudioFreq)
{
  int32_t ret;
  uint16_t tmp;
  
  switch (AudioFreq)
  {
  case  WM8994_FREQUENCY_8K:
    /* AIF1 Sample Rate = 8 (KHz), ratio=256 */
    tmp = 0x0003;
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_RATE, &tmp, 2);
    break;
    
  case  WM8994_FREQUENCY_16K:
    /* AIF1 Sample Rate = 16 (KHz), ratio=256 */ 
    tmp = 0x0033;
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_RATE, &tmp, 2);
    break;
    
  case  WM8994_FREQUENCY_32K:
    /* AIF1 Sample Rate = 32 (KHz), ratio=256 */
    tmp = 0x0063;
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_RATE, &tmp, 2);
    break;
    
  case  WM8994_FREQUENCY_96K:
    /* AIF1 Sample Rate = 96 (KHz), ratio=256 */
    tmp = 0x00A3;
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_RATE, &tmp, 2);
    break;
    
  case  WM8994_FREQUENCY_11K:
    /* AIF1 Sample Rate = 11.025 (KHz), ratio=256 */ 
    tmp = 0x0013;
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_RATE, &tmp, 2);
    break;
    
  case  WM8994_FREQUENCY_22K:
    /* AIF1 Sample Rate = 22.050 (KHz), ratio=256 */ 
    tmp = 0x0043;
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_RATE, &tmp, 2);
    break;
    
  case  WM8994_FREQUENCY_44K:
    /* AIF1 Sample Rate = 44.1 (KHz), ratio=256 */ 
    tmp = 0x0073;
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_RATE, &tmp, 2);
    break; 
    
  case  WM8994_FREQUENCY_48K:    
  default:
    /* AIF1 Sample Rate = 48 (KHz), ratio=256 */
    tmp = 0x0083;
    ret = wm8994_write_reg(&pObj->Ctx, WM8994_AIF1_RATE, &tmp, 2);
    break; 
  }
  
  return ret;
}

/**
  * @brief Get frequency.
  * @param pObj pointer to component object
  * @param AudioFreq Audio frequency
  * @retval Component status
  */
int32_t WM8994_GetFrequency(WM8994_Object_t *pObj, uint32_t *AudioFreq)
{
  int32_t ret = WM8994_OK;   
  uint16_t freq = 0;
  
  if(wm8994_aif1_sr_r(&pObj->Ctx, &freq) != WM8994_OK)
  {
    ret = WM8994_ERROR;
  }
  else
  {
    switch(freq)
    {
    case 0:
      *AudioFreq = WM8994_FREQUENCY_8K;
      break;
    case 1:
      *AudioFreq = WM8994_FREQUENCY_11K;
      break;    
    case 3:
      *AudioFreq = WM8994_FREQUENCY_16K;
      break;
    case 4:
      *AudioFreq = WM8994_FREQUENCY_22K;
      break; 
    case 6:
      *AudioFreq = WM8994_FREQUENCY_32K;
      break; 
    case 7:
      *AudioFreq = WM8994_FREQUENCY_44K;
      break;    
    case 8:
      *AudioFreq = WM8994_FREQUENCY_48K;
      break;
    case 10:
      *AudioFreq = WM8994_FREQUENCY_96K;
      break;    
    default:
      break;    
    }
  }
  
  return ret;
}

/**
  * @brief Resets wm8994 registers.
  * @param pObj pointer to component object 
  * @retval Component status if correct communication, else wrong communication
  */
int32_t WM8994_Reset(WM8994_Object_t *pObj)
{
  int32_t ret = WM8994_OK;
  
  /* Reset Codec by writing in 0x0000 address register */
  if(wm8994_sw_reset_w(&pObj->Ctx, 0x0000) != WM8994_OK)
  {
    ret = WM8994_ERROR;
  }
  
  return ret;
}

/******************** Static functions ****************************************/
/**
  * @brief  Function
  * @param  Component object pointer
  * @retval error status
  */
int32_t WM8994_RegisterBusIO (WM8994_Object_t *pObj, WM8994_IO_t *pIO)
{
  int32_t ret;
  
  if (pObj == NULL)
  {
    ret = WM8994_ERROR;
  }
  else
  {
    pObj->IO.Init      = pIO->Init;
    pObj->IO.DeInit    = pIO->DeInit;
    pObj->IO.Address   = pIO->Address;
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;
    
    pObj->Ctx.ReadReg  = WM8994_ReadRegWrap;
    pObj->Ctx.WriteReg = WM8994_WriteRegWrap;
    pObj->Ctx.handle   = pObj;
    
    if(pObj->IO.Init != NULL)
    {
      ret = pObj->IO.Init();
    }
    else
    {
      ret = WM8994_ERROR;
    }
  }
  
  return ret;
}

/**
  * @brief This function provides accurate delay (in milliseconds)
  * @param pObj pointer to component object
  * @param Delay: specifies the delay time length, in milliseconds
  * @retval Component status
  */
static int32_t WM8994_Delay(WM8994_Object_t *pObj, uint32_t Delay)
{  
  uint32_t tickstart;

  tickstart = pObj->IO.GetTick();
  while((pObj->IO.GetTick() - tickstart) < Delay)
  {
  }
  return WM8994_OK;
}

/**
  * @brief  Function
  * @param  handle  Component object handle
  * @param  Reg     The target register address to write
  * @param  pData   The target register value to be written
  * @param  Length  buffer size to be written
  * @retval error status
  */
static int32_t WM8994_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  WM8994_Object_t *pObj = (WM8994_Object_t *)handle;

  return pObj->IO.ReadReg(pObj->IO.Address, Reg, pData, Length);
}

/**
  * @brief  Function
  * @param  handle Component object handle
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval error status
  */
static int32_t WM8994_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  WM8994_Object_t *pObj = (WM8994_Object_t *)handle;

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
