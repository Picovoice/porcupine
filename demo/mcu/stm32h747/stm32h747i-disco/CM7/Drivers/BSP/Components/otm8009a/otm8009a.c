/**
  ******************************************************************************
  * @file    otm8009a.c
  * @author  MCD Application Team
  * @brief   This file provides the LCD Driver for KoD KM-040TMP-02-0621 (WVGA)
  *          DSI LCD Display OTM8009A.   
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

/* Includes ------------------------------------------------------------------*/
#include "otm8009a.h"
#include <stddef.h>

/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup Components
  * @{
  */ 

/** @defgroup OTM8009A OTM8009A
  * @brief     This file provides a set of functions needed to drive the 
  *            otm8009a IC display driver.
  * @{
  */

/** @defgroup OTM8009A_Exported_Types OTM8009A Exported Types
  * @{
  */
OTM8009A_LCD_Drv_t   OTM8009A_LCD_Driver = 
{
  OTM8009A_Init,
  OTM8009A_DeInit,  
  OTM8009A_ReadID,
  OTM8009A_DisplayOn,
  OTM8009A_DisplayOff,
  OTM8009A_SetBrightness,
  OTM8009A_GetBrightness,  
  OTM8009A_SetOrientation,
  OTM8009A_GetOrientation,
  OTM8009A_SetCursor,
  OTM8009A_DrawBitmap,
  OTM8009A_FillRGBRect,
  OTM8009A_DrawHLine,
  OTM8009A_DrawVLine,
  OTM8009A_FillRect,
  OTM8009A_GetPixel,
  OTM8009A_SetPixel,
  OTM8009A_GetXSize,
  OTM8009A_GetYSize,
};
  
static OTM8009A_LCD_Ctx_t OTM8009ACtx;
/**
  * @}
  */

/** @defgroup OTM8009A_Private_Constants OTM8009A Private Constants
  * @{
  */

/*
 * @brief Constant tables of register settings used to transmit DSI
 * command packets as power up initialization sequence of the KoD LCD (OTM8009A LCD Driver)
 */
   
/*
  * CASET value (Column Address Set) : X direction LCD GRAM boundaries
  * depending on LCD orientation mode and PASET value (Page Address Set) : Y direction
  * LCD GRAM boundaries depending on LCD orientation mode
  * XS[15:0] = 0x000 = 0, XE[15:0] = 0x31F = 799 for landscape mode : apply to CASET
  * YS[15:0] = 0x000 = 0, YE[15:0] = 0x31F = 799 for portrait mode : apply to PASET
  */
static const uint8_t LcdRegData27[] = {0x00, 0x00, 0x03, 0x1F};
/*
  * XS[15:0] = 0x000 = 0, XE[15:0] = 0x1DF = 479 for portrait mode : apply to CASET
  * YS[15:0] = 0x000 = 0, YE[15:0] = 0x1DF = 479 for landscape mode : apply to PASET
 */
static const uint8_t LcdRegData28[] = {0x00, 0x00, 0x01, 0xDF};

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup OTM8009A_Private_FunctionsPrototyes OTM8009A Private_Functions Prototyes
  * @{
  */
static int32_t OTM8009A_ReadRegWrap(void *Handle, uint16_t Reg, uint8_t* Data, uint16_t Length);
static int32_t OTM8009A_WriteRegWrap(void *Handle, uint16_t Reg, uint8_t *pData, uint16_t Length);
static int32_t OTM8009A_IO_Delay(OTM8009A_Object_t *pObj, uint32_t Delay);
/**
  * @}
  */

/** @defgroup OTM8009A_Exported_Variables
  * @{
  */

/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/
/** @defgroup OTM8009A_Exported_Functions OTM8009A Exported Functions
  * @{
  */

/**
  * @brief  Register component IO bus
  * @param  Component object pointer
  * @retval Component status
  */
int32_t OTM8009A_RegisterBusIO (OTM8009A_Object_t *pObj, OTM8009A_IO_t *pIO)
{
  int32_t ret = OTM8009A_OK;

  if(pObj == NULL)
  {
    ret = OTM8009A_ERROR;
  }
  else
  {
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;
    
    pObj->Ctx.ReadReg  = OTM8009A_ReadRegWrap;
    pObj->Ctx.WriteReg = OTM8009A_WriteRegWrap;
    pObj->Ctx.handle   = pObj;
  }
  
  return ret;
}

/**
  * @brief  Initializes the LCD KoD display part by communication in DSI mode in Video Mode
  *         with IC Display Driver OTM8009A (see IC Driver specification for more information).
  * @param  pObj Component object
  * @param  ColorCoding   Color Code
  * @param  Orientation   Display orientation
  * @retval Component status
  */
int32_t OTM8009A_Init(OTM8009A_Object_t *pObj, uint32_t ColorCoding, uint32_t Orientation)
{
  int32_t ret;
  static const uint8_t lcd_reg_data1[]  = {0x80,0x09,0x01};
  static const uint8_t lcd_reg_data2[]  = {0x80,0x09};
  static const uint8_t lcd_reg_data3[]  = {0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01};
  static const uint8_t lcd_reg_data4[]  = {0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01};
  static const uint8_t lcd_reg_data5[]  = {0x79,0x79};
  static const uint8_t lcd_reg_data6[]  = {0x00,0x01};
  static const uint8_t lcd_reg_data7[]  = {0x85,0x01,0x00,0x84,0x01,0x00};
  static const uint8_t lcd_reg_data8[]  = {0x18,0x04,0x03,0x39,0x00,0x00,0x00,0x18,0x03,0x03,0x3A,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data9[]  = {0x18,0x02,0x03,0x3B,0x00,0x00,0x00,0x18,0x01,0x03,0x3C,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data10[] = {0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00};
  static const uint8_t lcd_reg_data11[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data12[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data13[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data14[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data15[] = {0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data16[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data17[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data18[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  static const uint8_t lcd_reg_data19[] = {0x00,0x26,0x09,0x0B,0x01,0x25,0x00,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data20[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0A,0x0C,0x02};
  static const uint8_t lcd_reg_data21[] = {0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data22[] = {0x00,0x25,0x0C,0x0A,0x02,0x26,0x00,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data23[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x0B,0x09,0x01};
  static const uint8_t lcd_reg_data24[] = {0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const uint8_t lcd_reg_data25[] = {0xFF,0xFF,0xFF};
  
  static const uint8_t short_reg_data[] = {
    0x00,0x00,0x80,0x30,0x8A,0x40,0xB1,0xA9,0x91,0x34,0xB4,0x50,0x4E,0x81,0x66,0xA1,0x08,
    0x92,0x01,0x95,0x94,0x33,0xA3,0x1B,0x82,0x83,0x83,0x0E,0xA6,0xA0,0xB0,0xC0,0xD0,0x90,
    0xE0,0xF0,0x00,OTM8009A_COLMOD_RGB565,OTM8009A_COLMOD_RGB888,0x7F,0x2C,0x02,0xFF,0x00,
    0x00,0x00,0x66,0xB6,0x06,0xB1,0x06};
  
  /* Enable CMD2 to access vendor specific commands                               */
  /* Enter in command 2 mode and set EXTC to enable address shift function (0x00) */ 
  ret = otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[1], 0);
  
  ret += otm8009a_write_reg(&pObj->Ctx, 0xFF, lcd_reg_data1, 3);
  
  /* Enter ORISE Command 2 */
  /* Shift address to 0x80 */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[2], 0);
  
  ret += otm8009a_write_reg(&pObj->Ctx, 0xFF, lcd_reg_data2, 2);
  
  /////////////////////////////////////////////////////////////////////
  /* SD_PCH_CTRL - 0xC480h - 129th parameter - Default 0x00          */
  /* Set SD_PT                                                       */
  /* -> Source output level during porch and non-display area to GND */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[2], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC4, &short_reg_data[3], 0);
  
  (void)OTM8009A_IO_Delay(pObj, 10);
  /* Not documented */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[4], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC4, &short_reg_data[5], 0);
  (void)OTM8009A_IO_Delay(pObj, 10);
  /////////////////////////////////////////////////////////////////////
  
  /* PWR_CTRL4 - 0xC4B0h - 178th parameter - Default 0xA8 */
  /* Set gvdd_en_test                                     */
  /* -> enable GVDD test mode !!!                         */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[6], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC5, &short_reg_data[7], 0);
  
  /* PWR_CTRL2 - 0xC590h - 146th parameter - Default 0x79      */
  /* Set pump 4 vgh voltage                                    */
  /* -> from 15.0v down to 13.0v                               */
  /* Set pump 5 vgh voltage                                    */
  /* -> from -12.0v downto -9.0v                               */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[8], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC5, &short_reg_data[9], 0);
  
  /* P_DRV_M - 0xC0B4h - 181th parameter - Default 0x00 */
  /* -> Column inversion                                */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[10], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC0, &short_reg_data[11], 0);
  
  /* VCOMDC - 0xD900h - 1st parameter - Default 0x39h */
  /* VCOM Voltage settings                            */
  /* -> from -1.0000v downto -1.2625v                 */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[1], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xD9, &short_reg_data[12], 0);
  
  /* Oscillator adjustment for Idle/Normal mode (LPDT only) set to 65Hz (default is 60Hz) */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[13], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC1, &short_reg_data[14], 0);
  
  /* Video mode internal */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[15], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC1, &short_reg_data[16], 0);
  
  /* PWR_CTRL2 - 0xC590h - 147h parameter - Default 0x00 */
  /* Set pump 4&5 x6                                     */
  /* -> ONLY VALID when PUMP4_EN_ASDM_HV = "0"           */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[17], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC5, &short_reg_data[18], 0);
  
  /* PWR_CTRL2 - 0xC590h - 150th parameter - Default 0x33h */
  /* Change pump4 clock ratio                              */
  /* -> from 1 line to 1/2 line                            */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[19], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC5, &short_reg_data[9], 0);
  
  /* GVDD/NGVDD settings */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[1], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xD8, lcd_reg_data5, 2);
  
  /* PWR_CTRL2 - 0xC590h - 149th parameter - Default 0x33h */
  /* Rewrite the default value !                           */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[20], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC5, &short_reg_data[21], 0);
  
  /* Panel display timing Setting 3 */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[22], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC0, &short_reg_data[23], 0);
  
  /* Power control 1 */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[24], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC5, &short_reg_data[25], 0);
  
  /* Source driver precharge */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[13], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC4, &short_reg_data[26], 0);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[15], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC1, &short_reg_data[27], 0);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[28], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xB3, lcd_reg_data6, 2);
  
  /* GOAVST */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[2], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCE, lcd_reg_data7, 6);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[29], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCE, lcd_reg_data8, 14);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[30], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCE, lcd_reg_data9, 14);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[31], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCF, lcd_reg_data10, 10);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[32], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCF, &short_reg_data[45], 0);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[2], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCB, lcd_reg_data11, 10);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[33], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCB, lcd_reg_data12, 15);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[29], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCB, lcd_reg_data13, 15);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[30], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCB, lcd_reg_data14, 10);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[31], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCB, lcd_reg_data15, 15);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[32], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCB, lcd_reg_data16, 15);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[34], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCB, lcd_reg_data17, 10);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[35], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCB, lcd_reg_data18, 10);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[2], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCC, lcd_reg_data19, 10);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[33], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCC, lcd_reg_data20, 15);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[29], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCC, lcd_reg_data21, 15);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[30], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCC, lcd_reg_data22, 10);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[31], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCC, lcd_reg_data23, 15);
  
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[32], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xCC, lcd_reg_data24, 15);
  
  /////////////////////////////////////////////////////////////////////////////
  /* PWR_CTRL1 - 0xc580h - 130th parameter - default 0x00 */
  /* Pump 1 min and max DM                                */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[13], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC5, &short_reg_data[46], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[47], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xF5, &short_reg_data[48], 0);
  /////////////////////////////////////////////////////////////////////////////
  
  /* CABC LEDPWM frequency adjusted to 19,5kHz */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[49], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xC6, &short_reg_data[50], 0);
  
  /* Exit CMD2 mode */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[1], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xFF, lcd_reg_data25, 3);
  
  /*************************************************************************** */
  /* Standard DCS Initialization TO KEEP CAN BE DONE IN HSDT                   */
  /*************************************************************************** */
  
  /* NOP - goes back to DCS std command ? */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[1], 0);
  
  /* Gamma correction 2.2+ table (HSDT possible) */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[1], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xE1, lcd_reg_data3, 16);
  
  /* Gamma correction 2.2- table (HSDT possible) */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[1], 0);
  ret += otm8009a_write_reg(&pObj->Ctx, 0xE2, lcd_reg_data4, 16);
  
  /* Send Sleep Out command to display : no parameter */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_SLPOUT, &short_reg_data[36], 0);
  
  /* Wait for sleep out exit */
  (void)OTM8009A_IO_Delay(pObj, 120);
  
  switch(ColorCoding)
  {
  case OTM8009A_FORMAT_RBG565 :
    /* Set Pixel color format to RGB565 */
    ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_COLMOD, &short_reg_data[37], 0);
    OTM8009ACtx.ColorCode = OTM8009A_FORMAT_RBG565;
    break;
  case OTM8009A_FORMAT_RGB888 :
    /* Set Pixel color format to RGB888 */
    ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_COLMOD, &short_reg_data[38], 0);
    OTM8009ACtx.ColorCode = OTM8009A_FORMAT_RGB888;
    break;
  default :
    break;
  }
  
  /* Send command to configure display in landscape orientation mode. By default
  the orientation mode is portrait  */
  if(Orientation == OTM8009A_ORIENTATION_LANDSCAPE)
  {
	uint8_t tmp = OTM8009A_MADCTR_MODE_LANDSCAPE;
    ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_MADCTR, &tmp, 0);
    ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_CASET, LcdRegData27, 4);
    ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_PASET, LcdRegData28, 4);
    
    OTM8009ACtx.Orientation = OTM8009A_ORIENTATION_LANDSCAPE;
  }
  else
  {
    OTM8009ACtx.Orientation = OTM8009A_ORIENTATION_PORTRAIT;
  }
  
  /** CABC : Content Adaptive Backlight Control section start >> */
  /* Note : defaut is 0 (lowest Brightness], 0xFF is highest Brightness, try 0x7F : intermediate value */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_WRDISBV, &short_reg_data[39], 0);
  /* Store default brightness */
  OTM8009ACtx.Brightness = ((uint32_t)short_reg_data[39] / 0xFFU) * 100U;
  
  /* defaut is 0, try 0x2C - Brightness Control Block, Display Dimming & BackLight on */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_WRCTRLD, &short_reg_data[40], 0);
  
  /* defaut is 0, try 0x02 - image Content based Adaptive Brightness [Still Picture] */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_WRCABC, &short_reg_data[41], 0);
  
  /* defaut is 0 (lowest Brightness], 0xFF is highest Brightness */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_WRCABCMB, &short_reg_data[42], 0);
  
  /** CABC : Content Adaptive Backlight Control section end << */
  
  /* Send Command Display On */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_DISPON, &short_reg_data[43], 0);
  
  /* NOP command */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_NOP, &short_reg_data[1], 0);
  
  /* Send Command GRAM memory write (no parameters) : this initiates frame write via other DSI commands sent by */
  /* DSI host from LTDC incoming pixels in video mode */
  ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_RAMWR, &short_reg_data[44], 0);
  
  if(ret != OTM8009A_OK)
  {
    ret = OTM8009A_ERROR;
  }
  
  return ret;
}

/**
  * @brief  De-Initializes the component
  * @param  pObj Component object
  * @retval Component status
  */
int32_t OTM8009A_DeInit(OTM8009A_Object_t *pObj)
{
  return OTM8009A_ERROR;
}

/**
  * @brief  Read the component ID.
  * @param  pObj Component object
  * @param  Id   Component ID
  * @retval Component status
  */
int32_t OTM8009A_ReadID(OTM8009A_Object_t *pObj, uint32_t *Id)
{ 
  int32_t ret;
  
  if(otm8009a_read_reg(&pObj->Ctx, OTM8009A_CMD_ID1, (uint8_t *)Id, 0)!= OTM8009A_OK)
  {
    ret = OTM8009A_ERROR;
  }
  else
  {
    ret = OTM8009A_OK;
  }
  
  return ret;  
}  

/**
  * @brief  Set the display brightness.
  * @param  pObj Component object
  * @param  Brightness   display brightness to be set
  * @retval Component status
  */
int32_t OTM8009A_SetBrightness(OTM8009A_Object_t *pObj, uint32_t Brightness)
{
  int32_t ret;
  uint8_t brightness = (uint8_t)((Brightness * 255U)/100U);
  
  /* Send Display on DCS command to display */
  if(otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_WRDISBV, &brightness, 0) != OTM8009A_OK)
  {
    ret = OTM8009A_ERROR;
  }
  else
  {
    OTM8009ACtx.Brightness = Brightness;
    ret = OTM8009A_OK;
  }
  
  return ret;  
}

/**
  * @brief  Get the display brightness.
  * @param  pObj Component object
  * @param  Brightness   display brightness to be returned
  * @retval Component status
  */
int32_t OTM8009A_GetBrightness(OTM8009A_Object_t *pObj, uint32_t *Brightness)
{
  *Brightness = OTM8009ACtx.Brightness;
  return OTM8009A_OK;  
}

/**
  * @brief  Set the display On.
  * @param  pObj Component object
  * @retval Component status
  */
int32_t OTM8009A_DisplayOn(OTM8009A_Object_t *pObj)
{
  int32_t ret;
  uint8_t display = 0;
  
  /* Send Display on DCS command to display */
  if(otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_DISPON, &display, 0) != OTM8009A_OK)
  {
    ret = OTM8009A_ERROR;
  }
  else
  {
    ret = OTM8009A_OK;
  }
  
  return ret;
}

/**
  * @brief  Set the display Off.
  * @param  pObj Component object
  * @retval Component status
  */
int32_t OTM8009A_DisplayOff(OTM8009A_Object_t *pObj)
{
  int32_t ret;
  uint8_t display = 0;
  
  /* Send Display on DCS command to display */
  if(otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_DISPOFF, &display, 0) != OTM8009A_OK)
  {
    ret = OTM8009A_ERROR;
  }
  else
  {
    ret = OTM8009A_OK;
  }
  
  return ret;
}

/**
* @brief  Set the display Orientation.
* @param  pObj Component object
* @param  Orientation   display Orientation to be set
* @retval Component status
*/
int32_t OTM8009A_SetOrientation(OTM8009A_Object_t *pObj, uint32_t Orientation)
{
  int32_t ret;
  uint8_t tmp = OTM8009A_MADCTR_MODE_LANDSCAPE;
  uint8_t tmp1 = OTM8009A_MADCTR_MODE_PORTRAIT;
  
  if((Orientation != OTM8009A_ORIENTATION_LANDSCAPE) && (Orientation != OTM8009A_ORIENTATION_PORTRAIT))
  {
    ret = OTM8009A_ERROR;
  }/* Send command to configure display orientation mode  */
  else if(Orientation == OTM8009A_ORIENTATION_LANDSCAPE)
  {
    ret = otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_MADCTR, &tmp, 0);
    ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_CASET, LcdRegData27, 4);
    ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_PASET, LcdRegData28, 4);
    
    OTM8009ACtx.Orientation = OTM8009A_ORIENTATION_LANDSCAPE;
  }
  else
  {
    ret = otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_MADCTR, &tmp1, 0);
    ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_CASET, LcdRegData27, 4);
    ret += otm8009a_write_reg(&pObj->Ctx, OTM8009A_CMD_PASET, LcdRegData28, 4);
    
    OTM8009ACtx.Orientation = OTM8009A_ORIENTATION_PORTRAIT;
  }
  
  if(ret != OTM8009A_OK)
  {
    ret = OTM8009A_ERROR;
  }
  
  return ret;
}

/**
  * @brief  Set the display Orientation.
  * @param  pObj Component object
  * @param  Orientation   display Orientation to be returned
  * @retval Component status
  */
int32_t OTM8009A_GetOrientation(OTM8009A_Object_t *pObj, uint32_t *Orientation)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  *Orientation = OTM8009ACtx.Orientation;
  
  return OTM8009A_OK;
}

/**
  * @brief  Set the display Width.
  * @param  pObj Component object
  * @param  Xsize   display Width to be set
  * @retval Component status
  */
int32_t OTM8009A_GetXSize(OTM8009A_Object_t *pObj, uint32_t *Xsize)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  switch(OTM8009ACtx.Orientation)
  {
  case OTM8009A_ORIENTATION_PORTRAIT:
    *Xsize = OTM8009A_480X800_WIDTH;
    break;
  case OTM8009A_ORIENTATION_LANDSCAPE:
    *Xsize = OTM8009A_800X480_WIDTH;
    break;
  default:
    *Xsize = OTM8009A_800X480_WIDTH;
    break;
  }
  
  return OTM8009A_OK;
}

/**
  * @brief  Set the display Height.
  * @param  pObj Component object
  * @param  Ysize   display Height to be set
  * @retval Component status
  */
int32_t OTM8009A_GetYSize(OTM8009A_Object_t *pObj, uint32_t *Ysize)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  switch(OTM8009ACtx.Orientation)
  {
  case OTM8009A_ORIENTATION_PORTRAIT:
    *Ysize = OTM8009A_480X800_HEIGHT;
    break;
  case OTM8009A_ORIENTATION_LANDSCAPE:
    *Ysize = OTM8009A_800X480_HEIGHT;
    break;
  default:
    *Ysize = OTM8009A_800X480_HEIGHT;
    break;
  }
  
  return OTM8009A_OK;
}

/**
  * @brief  Set the display cursor.
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @retval Component status
  */
int32_t OTM8009A_SetCursor(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return OTM8009A_ERROR;
}

/**
  * @brief  Draw Bitmap image
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  pBmp pointer to bmp data
  * @retval Component status
  */
int32_t OTM8009A_DrawBitmap(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return OTM8009A_ERROR;
}

int32_t OTM8009A_FillRGBRect(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return OTM8009A_ERROR;
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
int32_t OTM8009A_DrawHLine(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return OTM8009A_ERROR;
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
int32_t OTM8009A_DrawVLine(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return OTM8009A_ERROR;
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
int32_t OTM8009A_FillRect(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return OTM8009A_ERROR;
}

/**
  * @brief  Get pixel color
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Color Color to be returned
  * @retval Component status
  */
int32_t OTM8009A_GetPixel(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t *Color)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return OTM8009A_ERROR;
}

/**
  * @brief  Set pixel color
  * @param  pObj Component object
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Color Color to be returned
  * @retval Component status
  */
int32_t OTM8009A_SetPixel(OTM8009A_Object_t *pObj, uint32_t Xpos, uint32_t Ypos, uint32_t Color)
{
  /* Prevent unused argument(s) compilation warning */  
  (void)(pObj);
  
  return OTM8009A_ERROR;
}

/**
  * @}
  */

/** @addtogroup OTM8009A_Private_FunctionsPrototyes
  * @{
  */
/**
  * @brief  Wrap component ReadReg to Bus Read function
  * @param  Handle  Component object handle
  * @param  Reg  The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length  buffer size to be written
  * @retval error status
  */
static int32_t OTM8009A_ReadRegWrap(void *Handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  OTM8009A_Object_t *pObj = (OTM8009A_Object_t *)Handle;

  return pObj->IO.ReadReg(pObj->IO.Address, Reg, pData, Length);
}

/**
  * @brief  Wrap component WriteReg to Bus Write function
  * @param  handle  Component object handle
  * @param  Reg  The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length  buffer size to be written
  * @retval error status
  */
static int32_t OTM8009A_WriteRegWrap(void *Handle, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  OTM8009A_Object_t *pObj = (OTM8009A_Object_t *)Handle;

  return pObj->IO.WriteReg(pObj->IO.Address, Reg, pData, Length);
}

/**
  * @brief  OTM8009A delay
  * @param  Delay  Delay in ms
  */
static int32_t OTM8009A_IO_Delay(OTM8009A_Object_t *pObj, uint32_t Delay)
{
  uint32_t tickstart;
  tickstart = pObj->IO.GetTick();
  while((pObj->IO.GetTick() - tickstart) < Delay)
  {
  }
  return OTM8009A_OK;
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
