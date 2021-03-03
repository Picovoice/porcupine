/**
  ******************************************************************************
  * @file    wm8994_reg.h
  * @author  MCD Application Team
  * @brief   Header of wm8994_reg.c 
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#ifndef WM8994_REG_H
#define WM8994_REG_H

#include <cmsis_compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Component
  * @{
  */ 
  
/** @addtogroup WM8994
  * @{
  */


/** @defgroup WM8994_Exported_Constants WM8994 Exported Constants
  * @{
  */ 
/******************************************************************************/
/****************************** REGISTER MAPPING ******************************/
/******************************************************************************/
/* SW Reset */
#define WM8994_SW_RESET               (uint16_t)0x0000

/* Power Management */
#define WM8994_PWR_MANAGEMENT_1       (uint16_t)0x0001
#define WM8994_PWR_MANAGEMENT_2       (uint16_t)0x0002
#define WM8994_PWR_MANAGEMENT_3       (uint16_t)0x0003
#define WM8994_PWR_MANAGEMENT_4       (uint16_t)0x0004
#define WM8994_PWR_MANAGEMENT_5       (uint16_t)0x0005
#define WM8994_PWR_MANAGEMENT_6       (uint16_t)0x0006

/* Input mixer */
#define WM8994_INPUT_MIXER_1          (uint16_t)0x0015
/* Input volume */
#define WM8994_LEFT_LINE_IN12_VOL     (uint16_t)0x0018
#define WM8994_LEFT_LINE_IN34_VOL     (uint16_t)0x0019
#define WM8994_RIGHT_LINE_IN12_VOL    (uint16_t)0x001A
#define WM8994_RIGHT_LINE_IN34_VOL    (uint16_t)0x001B

/* L/R Output volumes */
#define WM8994_LEFT_OUTPUT_VOL        (uint16_t)0x001C
#define WM8994_RIGHT_OUTPUT_VOL       (uint16_t)0x001D
#define WM8994_LINE_OUTPUT_VOL        (uint16_t)0x001E
#define WM8994_OUTPUT2_VOL            (uint16_t)0x001F


/* L/R OPGA volumes */
#define WM8994_LEFT_OPGA_VOL          (uint16_t)0x0020
#define WM8994_RIGHT_OPGA_VOL         (uint16_t)0x0021

/* SPKMIXL/R Attenuation */
#define WM8994_SPKMIXL_ATT            (uint16_t)0x0022
#define WM8994_SPKMIXR_ATT            (uint16_t)0x0023
#define WM8994_OUTPUT_MIXER           (uint16_t)0x0024
#define WM8994_CLASS_D                (uint16_t)0x0025
/* L/R Speakers volumes */
#define WM8994_SPK_LEFT_VOL           (uint16_t)0x0026
#define WM8994_SPK_RIGHT_VOL          (uint16_t)0x0027

/* Input mixer */
#define WM8994_INPUT_MIXER_2          (uint16_t)0x0028
#define WM8994_INPUT_MIXER_3          (uint16_t)0x0029
#define WM8994_INPUT_MIXER_4          (uint16_t)0x002A
#define WM8994_INPUT_MIXER_5          (uint16_t)0x002B
#define WM8994_INPUT_MIXER_6          (uint16_t)0x002C

/* Output mixer */
#define WM8994_OUTPUT_MIXER_1         (uint16_t)0x002D
#define WM8994_OUTPUT_MIXER_2         (uint16_t)0x002E
#define WM8994_OUTPUT_MIXER_3         (uint16_t)0x002F
#define WM8994_OUTPUT_MIXER_4         (uint16_t)0x0030
#define WM8994_OUTPUT_MIXER_5         (uint16_t)0x0031
#define WM8994_OUTPUT_MIXER_6         (uint16_t)0x0032
#define WM8994_OUTPUT2_MIXER          (uint16_t)0x0033
#define WM8994_LINE_MIXER_1           (uint16_t)0x0034
#define WM8994_LINE_MIXER_2           (uint16_t)0x0035
#define WM8994_SPEAKER_MIXER          (uint16_t)0x0036
#define WM8994_ADD_CONTROL            (uint16_t)0x0037
/* Antipop */
#define WM8994_ANTIPOP1               (uint16_t)0x0038
#define WM8994_ANTIPOP2               (uint16_t)0x0039
#define WM8994_MICBIAS                (uint16_t)0x003A
#define WM8994_LDO1                   (uint16_t)0x003B
#define WM8994_LDO2                   (uint16_t)0x003C

/* Charge pump */
#define WM8994_CHARGE_PUMP1           (uint16_t)0x004C
#define WM8994_CHARGE_PUMP2           (uint16_t)0x004D

#define WM8994_CLASS_W                (uint16_t)0x0051

#define WM8994_DC_SERVO1              (uint16_t)0x0054
#define WM8994_DC_SERVO2              (uint16_t)0x0055
#define WM8994_DC_SERVO_READBACK      (uint16_t)0x0058
#define WM8994_DC_SERVO_WRITEVAL      (uint16_t)0x0059

/* Analog HP */
#define WM8994_ANALOG_HP              (uint16_t)0x0060

#define WM8994_CHIP_REVISION          (uint16_t)0x0100
#define WM8994_CONTROL_INTERFACE      (uint16_t)0x0101
#define WM8994_WRITE_SEQ_CTRL1        (uint16_t)0x0110
#define WM8994_WRITE_SEQ_CTRL2        (uint16_t)0x0111

/* WM8994 clocking */
#define WM8994_AIF1_CLOCKING1         (uint16_t)0x0200
#define WM8994_AIF1_CLOCKING2         (uint16_t)0x0201
#define WM8994_AIF2_CLOCKING1         (uint16_t)0x0204
#define WM8994_AIF2_CLOCKING2         (uint16_t)0x0205
#define WM8994_CLOCKING1              (uint16_t)0x0208
#define WM8994_CLOCKING2              (uint16_t)0x0209
#define WM8994_AIF1_RATE              (uint16_t)0x0210
#define WM8994_AIF2_RATE              (uint16_t)0x0211
#define WM8994_RATE_STATUS            (uint16_t)0x0212

/* FLL1 Control */
#define WM8994_FLL1_CONTROL1          (uint16_t)0x0220
#define WM8994_FLL1_CONTROL2          (uint16_t)0x0221
#define WM8994_FLL1_CONTROL3          (uint16_t)0x0222
#define WM8994_FLL1_CONTROL4          (uint16_t)0x0223
#define WM8994_FLL1_CONTROL5          (uint16_t)0x0224

/* FLL2 Control */
#define WM8994_FLL2_CONTROL1          (uint16_t)0x0240
#define WM8994_FLL2_CONTROL2          (uint16_t)0x0241
#define WM8994_FLL2_CONTROL3          (uint16_t)0x0242
#define WM8994_FLL2_CONTROL4          (uint16_t)0x0243
#define WM8994_FLL2_CONTROL5          (uint16_t)0x0244


/* AIF1 control */
#define WM8994_AIF1_CONTROL1          (uint16_t)0x0300
#define WM8994_AIF1_CONTROL2          (uint16_t)0x0301
#define WM8994_AIF1_MASTER_SLAVE      (uint16_t)0x0302
#define WM8994_AIF1_BCLK              (uint16_t)0x0303
#define WM8994_AIF1_ADC_LRCLK         (uint16_t)0x0304
#define WM8994_AIF1_DAC_LRCLK         (uint16_t)0x0305
#define WM8994_AIF1_DAC_DELTA         (uint16_t)0x0306
#define WM8994_AIF1_ADC_DELTA         (uint16_t)0x0307

/* AIF2 control */
#define WM8994_AIF2_CONTROL1          (uint16_t)0x0310
#define WM8994_AIF2_CONTROL2          (uint16_t)0x0311
#define WM8994_AIF2_MASTER_SLAVE      (uint16_t)0x0312
#define WM8994_AIF2_BCLK              (uint16_t)0x0313
#define WM8994_AIF2_ADC_LRCLK         (uint16_t)0x0314
#define WM8994_AIF2_DAC_LRCLK         (uint16_t)0x0315
#define WM8994_AIF2_DAC_DELTA         (uint16_t)0x0316
#define WM8994_AIF2_ADC_DELTA         (uint16_t)0x0317

/* AIF1 ADC/DAC LR volumes */
#define WM8994_AIF1_ADC1_LEFT_VOL     (uint16_t)0x0400
#define WM8994_AIF1_ADC1_RIGHT_VOL    (uint16_t)0x0401
#define WM8994_AIF1_DAC1_LEFT_VOL     (uint16_t)0x0402
#define WM8994_AIF1_DAC1_RIGHT_VOL    (uint16_t)0x0403
#define WM8994_AIF1_ADC2_LEFT_VOL     (uint16_t)0x0404
#define WM8994_AIF1_ADC2_RIGHT_VOL    (uint16_t)0x0405
#define WM8994_AIF1_DAC2_LEFT_VOL     (uint16_t)0x0406
#define WM8994_AIF1_DAC2_RIGHT_VOL    (uint16_t)0x0407

/* AIF1 ADC/DAC filters */
#define WM8994_AIF1_ADC1_FILTERS      (uint16_t)0x0410
#define WM8994_AIF1_ADC2_FILTERS      (uint16_t)0x0411
#define WM8994_AIF1_DAC1_FILTER1      (uint16_t)0x0420
#define WM8994_AIF1_DAC1_FILTER2      (uint16_t)0x0421
#define WM8994_AIF1_DAC2_FILTER1      (uint16_t)0x0422
#define WM8994_AIF1_DAC2_FILTER2      (uint16_t)0x0423

/* AIF1 DRC1 registers */
#define WM8994_AIF1_DRC1              (uint16_t)0x0440
#define WM8994_AIF1_DRC1_1            (uint16_t)0x0441
#define WM8994_AIF1_DRC1_2            (uint16_t)0x0442
#define WM8994_AIF1_DRC1_3            (uint16_t)0x0443
#define WM8994_AIF1_DRC1_4            (uint16_t)0x0444
/* AIF1 DRC2 registers */
#define WM8994_AIF1_DRC2              (uint16_t)0x0450
#define WM8994_AIF1_DRC2_1            (uint16_t)0x0451
#define WM8994_AIF1_DRC2_2            (uint16_t)0x0452
#define WM8994_AIF1_DRC2_3            (uint16_t)0x0453
#define WM8994_AIF1_DRC2_4            (uint16_t)0x0454

/* AIF1 DAC1 EQ Gains Bands */
#define WM8994_AIF1_DAC1_EQG_1        (uint16_t)0x0480
#define WM8994_AIF1_DAC1_EQG_2        (uint16_t)0x0481
#define WM8994_AIF1_DAC1_EQG_1A       (uint16_t)0x0482
#define WM8994_AIF1_DAC1_EQG_1B       (uint16_t)0x0483
#define WM8994_AIF1_DAC1_EQG_1PG      (uint16_t)0x0484
#define WM8994_AIF1_DAC1_EQG_2A       (uint16_t)0x0485
#define WM8994_AIF1_DAC1_EQG_2B       (uint16_t)0x0486
#define WM8994_AIF1_DAC1_EQG_2C       (uint16_t)0x0487
#define WM8994_AIF1_DAC1_EQG_2PG      (uint16_t)0x0488
#define WM8994_AIF1_DAC1_EQG_3A       (uint16_t)0x0489
#define WM8994_AIF1_DAC1_EQG_3B       (uint16_t)0x048A
#define WM8994_AIF1_DAC1_EQG_3C       (uint16_t)0x048B
#define WM8994_AIF1_DAC1_EQG_3PG      (uint16_t)0x048C
#define WM8994_AIF1_DAC1_EQG_4A       (uint16_t)0x048D
#define WM8994_AIF1_DAC1_EQG_4B       (uint16_t)0x048E
#define WM8994_AIF1_DAC1_EQG_4C       (uint16_t)0x048F
#define WM8994_AIF1_DAC1_EQG_4PG      (uint16_t)0x0490
#define WM8994_AIF1_DAC1_EQG_5A       (uint16_t)0x0491
#define WM8994_AIF1_DAC1_EQG_5B       (uint16_t)0x0492
#define WM8994_AIF1_DAC1_EQG_5PG      (uint16_t)0x0493

/* AIF1 DAC2 EQ Gains/bands */
#define WM8994_AIF1_DAC2_EQG_1        (uint16_t)0x04A0
#define WM8994_AIF1_DAC2_EQG_2        (uint16_t)0x04A1
#define WM8994_AIF1_DAC2_EQG_1A       (uint16_t)0x04A2
#define WM8994_AIF1_DAC2_EQG_1B       (uint16_t)0x04A3
#define WM8994_AIF1_DAC2_EQG_1PG      (uint16_t)0x04A4
#define WM8994_AIF1_DAC2_EQG_2A       (uint16_t)0x04A5
#define WM8994_AIF1_DAC2_EQG_2B       (uint16_t)0x04A6
#define WM8994_AIF1_DAC2_EQG_2C       (uint16_t)0x04A7
#define WM8994_AIF1_DAC2_EQG_2PG      (uint16_t)0x04A8
#define WM8994_AIF1_DAC2_EQG_3A       (uint16_t)0x04A9
#define WM8994_AIF1_DAC2_EQG_3B       (uint16_t)0x04AA
#define WM8994_AIF1_DAC2_EQG_3C       (uint16_t)0x04AB
#define WM8994_AIF1_DAC2_EQG_3PG      (uint16_t)0x04AC
#define WM8994_AIF1_DAC2_EQG_4A       (uint16_t)0x04AD
#define WM8994_AIF1_DAC2_EQG_4B       (uint16_t)0x04AE
#define WM8994_AIF1_DAC2_EQG_4C       (uint16_t)0x04AF
#define WM8994_AIF1_DAC2_EQG_4PG      (uint16_t)0x04B0
#define WM8994_AIF1_DAC2_EQG_5A       (uint16_t)0x04B1
#define WM8994_AIF1_DAC2_EQG_5B       (uint16_t)0x04B2
#define WM8994_AIF1_DAC2_EQG_5PG      (uint16_t)0x04B3

/* AIF2 ADC/DAC LR volumes */
#define WM8994_AIF2_ADC_LEFT_VOL      (uint16_t)0x0500
#define WM8994_AIF2_ADC_RIGHT_VOL     (uint16_t)0x0501
#define WM8994_AIF2_DAC_LEFT_VOL      (uint16_t)0x0502
#define WM8994_AIF2_DAC_RIGHT_VOL     (uint16_t)0x0503

/* AIF2 ADC/DAC filters */
#define WM8994_AIF2_ADC_FILTERS       (uint16_t)0x0510
#define WM8994_AIF2_DAC_FILTER_1      (uint16_t)0x0520
#define WM8994_AIF2_DAC_FILTER_2      (uint16_t)0x0521

/* AIF2 DRC registers */
#define WM8994_AIF2_DRC_1             (uint16_t)0x0540
#define WM8994_AIF2_DRC_2             (uint16_t)0x0541
#define WM8994_AIF2_DRC_3             (uint16_t)0x0542
#define WM8994_AIF2_DRC_4             (uint16_t)0x0543
#define WM8994_AIF2_DRC_5             (uint16_t)0x0544

/* AIF2 EQ Gains/bands */
#define WM8994_AIF2_EQG_1             (uint16_t)0x0580
#define WM8994_AIF2_EQG_2             (uint16_t)0x0581
#define WM8994_AIF2_EQG_1A            (uint16_t)0x0582
#define WM8994_AIF2_EQG_1B            (uint16_t)0x0583
#define WM8994_AIF2_EQG_1PG           (uint16_t)0x0584
#define WM8994_AIF2_EQG_2A            (uint16_t)0x0585
#define WM8994_AIF2_EQG_2B            (uint16_t)0x0586
#define WM8994_AIF2_EQG_2C            (uint16_t)0x0587
#define WM8994_AIF2_EQG_2PG           (uint16_t)0x0588
#define WM8994_AIF2_EQG_3A            (uint16_t)0x0589
#define WM8994_AIF2_EQG_3B            (uint16_t)0x058A
#define WM8994_AIF2_EQG_3C            (uint16_t)0x058B
#define WM8994_AIF2_EQG_3PG           (uint16_t)0x058C
#define WM8994_AIF2_EQG_4A            (uint16_t)0x058D
#define WM8994_AIF2_EQG_4B            (uint16_t)0x058E
#define WM8994_AIF2_EQG_4C            (uint16_t)0x058F
#define WM8994_AIF2_EQG_4PG           (uint16_t)0x0590
#define WM8994_AIF2_EQG_5A            (uint16_t)0x0591
#define WM8994_AIF2_EQG_5B            (uint16_t)0x0592
#define WM8994_AIF2_EQG_5PG           (uint16_t)0x0593

/* AIF1 DAC1 Mixer volume */
#define WM8994_DAC1_MIXER_VOL         (uint16_t)0x0600
/* AIF1 DAC1 Left Mixer Routing */
#define WM8994_AIF1_DAC1_LMR          (uint16_t)0x0601
/* AIF1 DAC1 Righ Mixer Routing */
#define WM8994_AIF1_DAC1_RMR          (uint16_t)0x0602
/* AIF1 DAC2 Mixer volume */
#define WM8994_DAC2_MIXER_VOL         (uint16_t)0x0603
/* AIF1 DAC2 Left Mixer Routing */
#define WM8994_AIF1_DAC2_LMR          (uint16_t)0x0604
/* AIF1 DAC2 Righ Mixer Routing */
#define WM8994_AIF1_DAC2_RMR          (uint16_t)0x0605
/* AIF1 ADC1 Left Mixer Routing */
#define WM8994_AIF1_ADC1_LMR          (uint16_t)0x0606
/* AIF1 ADC1 Righ Mixer Routing */
#define WM8994_AIF1_ADC1_RMR          (uint16_t)0x0607
/* AIF1 ADC2 Left Mixer Routing */
#define WM8994_AIF1_ADC2_LMR          (uint16_t)0x0608
/* AIF1 ADC2 Righ Mixer Routing */
#define WM8994_AIF1_ADC2_RMR          (uint16_t)0x0609

/* Volume control */
#define WM8994_DAC1_LEFT_VOL          (uint16_t)0x0610
#define WM8994_DAC1_RIGHT_VOL         (uint16_t)0x0611
#define WM8994_DAC2_LEFT_VOL          (uint16_t)0x0612
#define WM8994_DAC2_RIGHT_VOL         (uint16_t)0x0613
#define WM8994_DAC_SOFTMUTE           (uint16_t)0x0614

#define WM8994_OVERSAMPLING           (uint16_t)0x0620
#define WM8994_SIDETONE               (uint16_t)0x0621

/* GPIO */
#define WM8994_GPIO1                  (uint16_t)0x0700
#define WM8994_GPIO2                  (uint16_t)0x0701
#define WM8994_GPIO3                  (uint16_t)0x0702
#define WM8994_GPIO4                  (uint16_t)0x0703
#define WM8994_GPIO5                  (uint16_t)0x0704
#define WM8994_GPIO6                  (uint16_t)0x0705
#define WM8994_GPIO7                  (uint16_t)0x0706
#define WM8994_GPIO8                  (uint16_t)0x0707
#define WM8994_GPIO9                  (uint16_t)0x0708
#define WM8994_GPIO10                 (uint16_t)0x0709
#define WM8994_GPIO11                 (uint16_t)0x070A
/* Pull Contol */
#define WM8994_PULL_CONTROL_1         (uint16_t)0x0720
#define WM8994_PULL_CONTROL_2         (uint16_t)0x0721
/* WM8994 Inturrupts */
#define WM8994_INT_STATUS_1           (uint16_t)0x0730
#define WM8994_INT_STATUS_2           (uint16_t)0x0731
#define WM8994_INT_RAW_STATUS_2       (uint16_t)0x0732
#define WM8994_INT_STATUS1_MASK       (uint16_t)0x0738
#define WM8994_INT_STATUS2_MASK       (uint16_t)0x0739
#define WM8994_INT_CONTROL            (uint16_t)0x0740
#define WM8994_IRQ_DEBOUNCE           (uint16_t)0x0748

/* Write Sequencer registers from 0 to 511 */
#define WM8994_WRITE_SEQUENCER0       (uint16_t)0x3000
#define WM8994_WRITE_SEQUENCER1       (uint16_t)0x3001
#define WM8994_WRITE_SEQUENCER2       (uint16_t)0x3002
#define WM8994_WRITE_SEQUENCER3       (uint16_t)0x3003

#define WM8994_WRITE_SEQUENCER4       (uint16_t)0x3508
#define WM8994_WRITE_SEQUENCER5       (uint16_t)0x3509
#define WM8994_WRITE_SEQUENCER6       (uint16_t)0x3510
#define WM8994_WRITE_SEQUENCER7       (uint16_t)0x3511

/**
  * @}
  */ 

/************** Generic Function  *******************/

typedef int32_t (*WM8994_Write_Func)(void *, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*WM8994_Read_Func) (void *, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  WM8994_Write_Func   WriteReg;
  WM8994_Read_Func    ReadReg;
  void                *handle;
} wm8994_ctx_t;

/*******************************************************************************
* Register      : Generic - All
* Address       : Generic - All
* Bit Group Name: None
* Permission    : W
*******************************************************************************/
int32_t wm8994_write_reg(wm8994_ctx_t *ctx, uint16_t reg, uint16_t *data, uint16_t length);
int32_t wm8994_read_reg(wm8994_ctx_t *ctx, uint16_t reg, uint16_t* data, uint16_t length);

int32_t wm8994_register_set(wm8994_ctx_t *ctx, uint16_t reg, uint16_t value);
/**************** Base Function  *******************/
/*******************************************************************************
* Register      : WM8994_SW_RESET
* Address       : 0X00
* Bit Group Name: SW_RESET[15:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_SW_RESET_MASK        (uint16_t)0xFFFF
#define   WM8994_SW_RESET_POSITION    0
int32_t  wm8994_sw_reset_w(wm8994_ctx_t *ctx, uint16_t value);
int32_t  wm8994_sw_reset_r(wm8994_ctx_t *ctx, uint16_t *value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_1
* Address       : 0X01
* Bit Group Name: BIAS_EN
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_1_BIAS_EN_MASK        (uint16_t)0x0001U
#define   WM8994_PWR_MGMT_1_BIAS_EN_POSITION    0
int32_t  wm8994_pwr_mgmt_1_bias_en(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_1
* Address       : 0X01
* Bit Group Name: VMID_SEL [1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_1_VMID_SEL_MASK        (uint16_t)0x0006U
#define   WM8994_PWR_MGMT_1_VMID_SEL_POSITION    1
int32_t  wm8994_pwr_mgmt_1_vmid_sel(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_1
* Address       : 0X01
* Bit Group Name: MICB1_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_1_MICB1_ENA_MASK        (uint16_t)0x0010U
#define   WM8994_PWR_MGMT_1_MICB1_ENA_POSITION    4
int32_t  wm8994_pwr_mgmt_1_micb1_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_1
* Address       : 0X01
* Bit Group Name: MICB2_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_1_MICB2_ENA_MASK        (uint16_t)0x0020U
#define   WM8994_PWR_MGMT_1_MICB2_ENA_POSITION    5
int32_t  wm8994_pwr_mgmt_1_micb2_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_1
* Address       : 0X01
* Bit Group Name: HPOUT1R_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_1_HPOUT1R_ENA_MASK        (uint16_t)0x0100U
#define   WM8994_PWR_MGMT_1_HPOUT1R_ENA_POSITION    8
int32_t  wm8994_pwr_mgmt_1_hpout1r_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_1
* Address       : 0X01
* Bit Group Name: HPOUT1L_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_1_HPOUT1L_ENA_MASK        (uint16_t)0x0200U
#define   WM8994_PWR_MGMT_1_HPOUT1L_ENA_POSITION    9
int32_t  wm8994_pwr_mgmt_1_hpout1l_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_1
* Address       : 0X01
* Bit Group Name: HPOUT2_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_1_HPOUT2_ENA_MASK        (uint16_t)0x0800U
#define   WM8994_PWR_MGMT_1_HPOUT2_ENA_POSITION    11
int32_t  wm8994_pwr_mgmt_1_hpout2_ena(wm8994_ctx_t *ctx, uint16_t value);
    
/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_1
* Address       : 0X01
* Bit Group Name: SPKOUTL_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_1_SPKOUTL_ENA_MASK        (uint16_t)0x1000U
#define   WM8994_PWR_MGMT_1_SPKOUTL_ENA_POSITION    12
int32_t  wm8994_pwr_mgmt_1_spkoutl_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_1
* Address       : 0X01
* Bit Group Name: SPKOUTR_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_1_SPKOUTR_ENA_MASK        (uint16_t)0x2000U
#define   WM8994_PWR_MGMT_1_SPKOUTR_ENA_POSITION    13
int32_t  wm8994_pwr_mgmt_1_spkoutr_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_2
* Address       : 0X02
* Bit Group Name: IN1R_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_2_IN1R_ENA_MASK        (uint16_t)0x0010U
#define   WM8994_PWR_MGMT_2_IN1R_ENA_POSITION    4
int32_t  wm8994_pwr_mgmt_2_in1r_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_2
* Address       : 0X02
* Bit Group Name: IN2R_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_2_IN2R_ENA_MASK        (uint16_t)0x0020U
#define   WM8994_PWR_MGMT_2_IN2R_ENA_POSITION    5
int32_t  wm8994_pwr_mgmt_2_in2r_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_2
* Address       : 0X02
* Bit Group Name: IN1L_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_2_IN1L_ENA_MASK        (uint16_t)0x0040U
#define   WM8994_PWR_MGMT_2_IN1L_ENA_POSITION    6
int32_t  wm8994_pwr_mgmt_2_in1l_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_2
* Address       : 0X02
* Bit Group Name: IN2L_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_2_IN2L_ENA_MASK        (uint16_t)0x0080U
#define   WM8994_PWR_MGMT_2_IN2L_ENA_POSITION    7
int32_t  wm8994_pwr_mgmt_2_in2l_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_2
* Address       : 0X02
* Bit Group Name: MIXINR_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_2_MIXINR_ENA_MASK        (uint16_t)0x0100U
#define   WM8994_PWR_MGMT_2_MIXINR_ENA_POSITION    8
int32_t  wm8994_pwr_mgmt_2_mixinr_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_2
* Address       : 0X02
* Bit Group Name: MIXINL_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_2_MIXINL_ENA_MASK        (uint16_t)0x0200U
#define   WM8994_PWR_MGMT_2_MIXINL_ENA_POSITION    9
int32_t  wm8994_pwr_mgmt_2_mixinl_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_2
* Address       : 0X02
* Bit Group Name: OPCLK_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_2_OPCLK_ENA_MASK        (uint16_t)0x0800U
#define   WM8994_PWR_MGMT_2_OPCLK_ENA_POSITION    11
int32_t  wm8994_pwr_mgmt_2_opclk_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_2
* Address       : 0X02
* Bit Group Name: TSHUT_OPDIS
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_2_TSHUT_OPDIS_MASK        (uint16_t)0x2000U
#define   WM8994_PWR_MGMT_2_TSHUT_OPDIS_POSITION    13
int32_t  wm8994_pwr_mgmt_2_tshut_opdis(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_2
* Address       : 0X02
* Bit Group Name: TSHUT_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_2_TSHUT_ENA_MASK        (uint16_t)0x4000U
#define   WM8994_PWR_MGMT_2_TSHUT_ENA_POSITION    14
int32_t   wm8994_pwr_mgmt_2_tshut_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_3
* Address       : 0X03
* Bit Group Name: MIXOUTR_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_3_MIXOUTR_ENA_MASK        (uint16_t)0x0010U
#define   WM8994_PWR_MGMT_3_MIXOUTR_ENA_POSITION    4
int32_t   wm8994_pwr_mgmt_3_mixoutr_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_3
* Address       : 0X03
* Bit Group Name: MIXOUTL_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_3_MIXOUTL_ENA_MASK        (uint16_t)0x0020U
#define   WM8994_PWR_MGMT_3_MIXOUTL_ENA_POSITION    5
int32_t   wm8994_pwr_mgmt_3_mixoutl_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_3
* Address       : 0X03
* Bit Group Name: MIXOUTRVOL_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_3_MIXOUTRVOL_ENA_MASK        (uint16_t)0x0040U
#define   WM8994_PWR_MGMT_3_MIXOUTRVOL_ENA_POSITION    6
int32_t   wm8994_pwr_mgmt_3_mixoutrvol_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_3
* Address       : 0X03
* Bit Group Name: MIXOUTLVOL_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_3_MIXOUTLVOL_ENA_MASK        (uint16_t)0x0080U
#define   WM8994_PWR_MGMT_3_MIXOUTLVOL_ENA_POSITION    7
int32_t   wm8994_pwr_mgmt_3_mixoutlvol_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_3
* Address       : 0X03
* Bit Group Name: SPKLVOL_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_3_SPKLVOL_ENA_MASK        (uint16_t)0x0100U
#define   WM8994_PWR_MGMT_3_SPKLVOL_ENA_POSITION    8
int32_t   wm8994_pwr_mgmt_3_spklvol_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_3
* Address       : 0X03
* Bit Group Name: SPKRVOL_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_3_SPKRVOL_ENA_MASK        (uint16_t)0x0200U
#define   WM8994_PWR_MGMT_3_SPKRVOL_ENA_POSITION    9
int32_t   wm8994_pwr_mgmt_3_spkrvol_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_3
* Address       : 0X03
* Bit Group Name: LINEOUT2P_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_3_LINEOUT2P_ENA_MASK        (uint16_t)0x0400U
#define   WM8994_PWR_MGMT_3_LINEOUT2P_ENA_POSITION    10
int32_t   wm8994_pwr_mgmt_3_lineout2p_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_3
* Address       : 0X03
* Bit Group Name: LINEOUT2N_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_3_LINEOUT2N_ENA_MASK        (uint16_t)0x0800U
#define   WM8994_PWR_MGMT_3_LINEOUT2N_ENA_POSITION    11
int32_t   wm8994_pwr_mgmt_3_lineout2n_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_3
* Address       : 0X03
* Bit Group Name: LINEOUT1P_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_3_LINEOUT1P_ENA_MASK        (uint16_t)0x1000U
#define   WM8994_PWR_MGMT_3_LINEOUT1P_ENA_POSITION    12
int32_t   wm8994_pwr_mgmt_3_lineout1p_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_3
* Address       : 0X03
* Bit Group Name: LINEOUT1N_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_3_LINEOUT1N_ENA_MASK        (uint16_t)0x2000U
#define   WM8994_PWR_MGMT_3_LINEOUT1N_ENA_POSITION    13
int32_t   wm8994_pwr_mgmt_3_lineout1n_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: ADCR_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_ADCR_ENA_MASK        (uint16_t)0x0001U
#define   WM8994_PWR_MGMT_4_ADCR_ENA_POSITION    0
int32_t   wm8994_pwr_mgmt_4_adcr_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: ADCL_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_ADCL_ENA_MASK        (uint16_t)0x0002U
#define   WM8994_PWR_MGMT_4_ADCL_ENA_POSITION    1
int32_t   wm8994_pwr_mgmt_4_adcl_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: DMIC1R_ENA
* Permission    : RWZ
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_DMIC1R_ENA_MASK        (uint16_t)0x0004U
#define   WM8994_PWR_MGMT_4_DMIC1R_ENA_POSITION    2
int32_t   wm8994_pwr_mgmt_4_dmic1r_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: DMIC1L_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_DMIC1L_ENA_MASK        (uint16_t)0x0008U
#define   WM8994_PWR_MGMT_4_DMIC1L_ENA_POSITION    3
int32_t   wm8994_pwr_mgmt_4_dmic1l_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: DMIC2R_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_DMIC2R_ENA_MASK        (uint16_t)0x0010U
#define   WM8994_PWR_MGMT_4_DMIC2R_ENA_POSITION    4
int32_t   wm8994_pwr_mgmt_4_dmic2r_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: DMIC2L_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_DMIC2L_ENA_MASK        (uint16_t)0x0020U
#define   WM8994_PWR_MGMT_4_DMIC2L_ENA_POSITION    5
int32_t   wm8994_pwr_mgmt_4_dmic2l_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: AIF1ADC1R_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_AIF1ADC1R_ENA_MASK        (uint16_t)0x0100U
#define   WM8994_PWR_MGMT_4_AIF1ADC1R_ENA_POSITION    8
int32_t   wm8994_pwr_mgmt_4_aif1adc1r_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: AIF1ADC1L_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_AIF1ADC1L_ENA_MASK        (uint16_t)0x0200U
#define   WM8994_PWR_MGMT_4_AIF1ADC1L_ENA_POSITION    9
int32_t   wm8994_pwr_mgmt_4_aif1adc1l_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: AIF1ADC2R_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_AIF1ADC2R_ENA_MASK        (uint16_t)0x0400U
#define   WM8994_PWR_MGMT_4_AIF1ADC2R_ENA_POSITION    10
int32_t   wm8994_pwr_mgmt_4_aif1adc2r_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: AIF1ADC2L_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_AIF1ADC2L_ENA_MASK        (uint16_t)0x0800U
#define   WM8994_PWR_MGMT_4_AIF1ADC2L_ENA_POSITION    11
int32_t   wm8994_pwr_mgmt_4_aif1adc2l_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: AIF2ADCR_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_AIF2ADCR_ENA_MASK        (uint16_t)0x1000U
#define   WM8994_PWR_MGMT_4_AIF2ADCR_ENA_POSITION    12
int32_t   wm8994_pwr_mgmt_4_aif2adcr_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_4
* Address       : 0X04
* Bit Group Name: AIF2ADCL_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_4_AIF2ADCL_ENA_MASK        (uint16_t)0x2000U
#define   WM8994_PWR_MGMT_4_AIF2ADCL_ENA_POSITION    13
int32_t   wm8994_pwr_mgmt_4_aif2adcl_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_5
* Address       : 0X05
* Bit Group Name: DAC1R_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_5_DAC1R_ENA_MASK        (uint16_t)0x0001U
#define   WM8994_PWR_MGMT_5_DAC1R_ENA_POSITION    0
int32_t   wm8994_pwr_mgmt_5_dac1r_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_5
* Address       : 0X05
* Bit Group Name: DAC1L_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_5_DAC1L_ENA_MASK        (uint16_t)0x0002U
#define   WM8994_PWR_MGMT_5_DAC1L_ENA_POSITION    1
int32_t   wm8994_pwr_mgmt_5_dac1l_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_5
* Address       : 0X05
* Bit Group Name: DAC2R_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_5_DAC2R_ENA_MASK        (uint16_t)0x0004U
#define   WM8994_PWR_MGMT_5_DAC2R_ENA_POSITION    2
int32_t   wm8994_pwr_mgmt_5_dac2r_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_5
* Address       : 0X05
* Bit Group Name: DAC2L_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_5_DAC2L_ENA_MASK        (uint16_t)0x0008U
#define   WM8994_PWR_MGMT_5_DAC2L_ENA_POSITION    3
int32_t   wm8994_pwr_mgmt_5_dac2l_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_5
* Address       : 0X05
* Bit Group Name: AIF1DAC1R_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_5_AIF1DAC1R_ENA_MASK        (uint16_t)0x0100U
#define   WM8994_PWR_MGMT_5_AIF1DAC1R_ENA_POSITION    8
int32_t   wm8994_pwr_mgmt_5_aif1dac1r_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_5
* Address       : 0X05
* Bit Group Name: AIF1DAC1L_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_5_AIF1DAC1L_ENA_MASK        (uint16_t)0x0200U
#define   WM8994_PWR_MGMT_5_AIF1DAC1L_ENA_POSITION    9
int32_t   wm8994_pwr_mgmt_5_aif1dac1l_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_5
* Address       : 0X05
* Bit Group Name: AIF1DAC2R_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_5_AIF1DAC2R_ENA_MASK        (uint16_t)0x0400U
#define   WM8994_PWR_MGMT_5_AIF1DAC2R_ENA_POSITION    10
int32_t   wm8994_pwr_mgmt_5_aif1dac2r_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_5
* Address       : 0X05
* Bit Group Name: AIF1DAC2L_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_5_AIF1DAC2L_ENA_MASK        (uint16_t)0x0800U
#define   WM8994_PWR_MGMT_5_AIF1DAC2L_ENA_POSITION    11
int32_t   wm8994_pwr_mgmt_5_aif1dac2l_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_5
* Address       : 0X05
* Bit Group Name: AIF2DACR_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_5_AIF2DACR_ENA_MASK        (uint16_t)0x1000U
#define   WM8994_PWR_MGMT_5_AIF2DACR_ENA_POSITION    12
int32_t   wm8994_pwr_mgmt_5_aif2dacr_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_5
* Address       : 0X05
* Bit Group Name: AIF2DACL_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_5_AIF2DACL_ENA_MASK        (uint16_t)0x2000U
#define   WM8994_PWR_MGMT_5_AIF2DACL_ENA_POSITION    13
int32_t   wm8994_pwr_mgmt_5_aif2dacl_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_6
* Address       : 0X06
* Bit Group Name: AIF1_DACDAT_SRC
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_6_AIF1_DACDAT_SRC_MASK        (uint16_t)0x0001U
#define   WM8994_PWR_MGMT_6_AIF1_DACDAT_SRC_POSITION    0
int32_t   wm8994_pwr_mgmt_6_aif1_dacdat_src(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_6
* Address       : 0X06
* Bit Group Name: AIF2_DACDAT_SRC
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_6_AIF2_DACDAT_SRC_MASK        (uint16_t)0x0002U
#define   WM8994_PWR_MGMT_6_AIF2_DACDAT_SRC_POSITION    1
int32_t   wm8994_pwr_mgmt_6_aif2_dacdat_src(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_6
* Address       : 0X06
* Bit Group Name: AIF2_ADCDAT_SRC
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_6_AIF2_ADCDAT_SRC_MASK        (uint16_t)0x0004U
#define   WM8994_PWR_MGMT_6_AIF2_ADCDAT_SRC_POSITION    2
int32_t   wm8994_pwr_mgmt_6_aif2_adcdat_src(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_6
* Address       : 0X06
* Bit Group Name: AIF3_ADCDAT_SRC
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_6_AIF3_ADCDAT_SRC_MASK        (uint16_t)0x0018U
#define   WM8994_PWR_MGMT_6_AIF3_ADCDAT_SRC_POSITION    3
int32_t   wm8994_pwr_mgmt_6_aif3_adcdat_src(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_PWR_MANAGEMENT_6
* Address       : 0X06
* Bit Group Name: AIF3_TRI
* Permission    : RW
*******************************************************************************/
#define   WM8994_PWR_MGMT_6_AIF3_TRI_MASK        (uint16_t)0x0020U
#define   WM8994_PWR_MGMT_6_AIF3_TRI_POSITION    5
int32_t   wm8994_pwr_mgmt_6_aif3_tri(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_1
* Address       : 0X15
* Bit Group Name: INPUTS_CLAMP
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER1_INPUTS_CLAMP_MASK        (uint16_t)0x0040U
#define   WM8994_INMIXER1_INPUTS_CLAMP_POSITION    6
int32_t   wm8994_inmixer1_inputs_clamp(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_1
* Address       : 0X15
* Bit Group Name: IN1LP_MIXINL_BOOST
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER1_IN1LP_MIXINL_BOOST_MASK        (uint16_t)0x0080U
#define   WM8994_INMIXER1_IN1LP_MIXINL_BOOST_POSITION    7
int32_t   wm8994_inmixer1_in1lp_mixinl_boost(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_1
* Address       : 0X15
* Bit Group Name: IN1RP_MIXINR_BOOST
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER1_IN1RP_MIXINR_BOOST_MASK        (uint16_t)0x0100U
#define   WM8994_INMIXER1_IN1RP_MIXINR_BOOST_POSITION    8
int32_t   wm8994_inmixer1_in1rp_mixinr_boost(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_LEFT_LINE_IN12_VOL
* Address       : 0X18
* Bit Group Name: IN1L_VOL [4:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_LLI_IN1L_VOL_MASK        (uint16_t)0x001F
#define   WM8994_LLI_IN1L_VOL_POSITION    0
int32_t   wm8994_lli_in1l_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_LEFT_LINE_IN12_VOL
* Address       : 0X18
* Bit Group Name: IN1L_ZC
* Permission    : RW
*******************************************************************************/
#define   WM8994_LLI_IN1L_ZC_MASK        (uint16_t)0x0040U
#define   WM8994_LLI_IN1L_ZC_POSITION    6
int32_t   wm8994_lli_in1l_zc(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_LEFT_LINE_IN12_VOL
* Address       : 0X18
* Bit Group Name: IN1L_MUTE
* Permission    : RW
*******************************************************************************/
#define   WM8994_LLI_IN1L_MUTE_MASK        (uint16_t)0x0080U
#define   WM8994_LLI_IN1L_MUTE_POSITION    7
int32_t   wm8994_lli_in1l_mute(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_LEFT_LINE_IN12_VOL
* Address       : 0X18
* Bit Group Name: IN1_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_LLI_IN1_VU_MASK        (uint16_t)0x0100U
#define   WM8994_LLI_IN1_VU_POSITION    8
int32_t   wm8994_lli_in1_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_LEFT_LINE_IN34_VOL
* Address       : 0X19
* Bit Group Name: IN2L_VOL [4:0]
* Permission    : RW
***************************************************************2****************/
#define   WM8994_LLI_IN2L_VOL_MASK        (uint16_t)0x001F
#define   WM8994_LLI_IN2L_VOL_POSITION    0
int32_t   wm8994_lli_in2l_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_LEFT_LINE_IN34_VOL
* Address       : 0X19
* Bit Group Name: IN2L_ZC
* Permission    : RW
*******************************************************************************/
#define   WM8994_LLI_IN2L_ZC_MASK        (uint16_t)0x0040U
#define   WM8994_LLI_IN2L_ZC_POSITION    6
int32_t   wm8994_lli_in2l_zc(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_LEFT_LINE_IN34_VOL
* Address       : 0X19
* Bit Group Name: IN2L_MUTE
* Permission    : RW
*******************************************************************************/
#define   WM8994_LLI_IN2L_MUTE_MASK        (uint16_t)0x0080U
#define   WM8994_LLI_IN2L_MUTE_POSITION    7
int32_t   wm8994_lli_in2l_mute(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_LEFT_LINE_IN34_VOL
* Address       : 0X19
* Bit Group Name: IN1_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_LLI_IN2_VU_MASK        (uint16_t)0x0100U
#define   WM8994_LLI_IN2_VU_POSITION    8
int32_t   wm8994_lli_in2_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_RIGHT_LINE_IN12_VOL
* Address       : 0X1A
* Bit Group Name: IN1R_VOL [4:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_RLI_IN1R_VOL_MASK        (uint16_t)0x001F
#define   WM8994_RLI_IN1R_VOL_POSITION    0
int32_t   wm8994_rli_in1r_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_RIGHT_LINE_IN12_VOL
* Address       : 0X1A
* Bit Group Name: IN1R_ZC
* Permission    : RW
*******************************************************************************/
#define   WM8994_RLI_IN1R_ZC_MASK        (uint16_t)0x0040U
#define   WM8994_RLI_IN1R_ZC_POSITION    6
int32_t   wm8994_rli_in1r_zc(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_RIGHT_LINE_IN12_VOL
* Address       : 0X1A
* Bit Group Name: IN1R_MUTE
* Permission    : RW
*******************************************************************************/
#define   WM8994_RLI_IN1R_MUTE_MASK        (uint16_t)0x0080U
#define   WM8994_RLI_IN1R_MUTE_POSITION    7
int32_t   wm8994_rli_in1r_mute(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_RIGHT_LINE_IN12_VOL
* Address       : 0X1A
* Bit Group Name: IN1_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_RLI_IN1_VU_MASK        (uint16_t)0x0100U
#define   WM8994_RLI_IN1_VU_POSITION    8
int32_t   wm8994_rli_in1_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_RIGHT_LINE_IN34_VOL
* Address       : 0X1B
* Bit Group Name: IN2R_VOL [4:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_RLI_IN2R_VOL_MASK        (uint16_t)0x001F
#define   WM8994_RLI_IN2R_VOL_POSITION    0
int32_t   wm8994_rli_in2r_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_RIGHT_LINE_IN34_VOL
* Address       : 0X1B
* Bit Group Name: IN2R_ZC
* Permission    : RW
*******************************************************************************/
#define   WM8994_RLI_IN2R_ZC_MASK        (uint16_t)0x0040U
#define   WM8994_RLI_IN2R_ZC_POSITION    6
int32_t   wm8994_rli_in2r_zc(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_RIGHT_LINE_IN34_VOL
* Address       : 0X1B
* Bit Group Name: IN2R_MUTE
* Permission    : RW
*******************************************************************************/
#define   WM8994_RLI_IN2R_MUTE_MASK        (uint16_t)0x0080U
#define   WM8994_RLI_IN2R_MUTE_POSITION    7
int32_t   wm8994_rli_in2r_mute(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_RIGHT_LINE_IN34_VOL
* Address       : 0X1B
* Bit Group Name: IN2_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_RLI_IN2_VU_MASK        (uint16_t)0x0100U
#define   WM8994_RLI_IN2_VU_POSITION    8
int32_t   wm8994_rli_in2_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_LEFT_OUTPUT_VOL
* Address       : 0X1C
* Bit Group Name: HPOUT1L_VOL [5:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_LO_HPOUT1L_VOL_MASK        (uint16_t)0x003F
#define   WM8994_LO_HPOUT1L_VOL_POSITION    0
int32_t   wm8994_lo_hpout1l_vol(wm8994_ctx_t *ctx, uint16_t value);
int32_t   wm8994_lo_hpout1l_vol_r(wm8994_ctx_t *ctx, uint16_t *value);

/*******************************************************************************
* Register      : WM8994_LEFT_OUTPUT_VOL
* Address       : 0X1C
* Bit Group Name: HPOUT1L_MUTE_N
* Permission    : RW
*******************************************************************************/
#define   WM8994_LO_HPOUT1L_MUTE_N_MASK        (uint16_t)0x0040U
#define   WM8994_LO_HPOUT1L_MUTE_N_POSITION    6
int32_t   wm8994_lo_hpout1l_mute_n(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_LEFT_OUTPUT_VOL
* Address       : 0X1C
* Bit Group Name: HPOUT1L_ZC
* Permission    : RW
*******************************************************************************/
#define   WM8994_LO_HPOUT1L_ZC_MASK        (uint16_t)0x0080U
#define   WM8994_LO_HPOUT1L_ZC_POSITION    7
int32_t   wm8994_lo_hpout1l_zc(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_LEFT_OUTPUT_VOL
* Address       : 0X1C
* Bit Group Name: HPOUT1L_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_LO_HPOUT1L_VU_MASK        (uint16_t)0x0100U
#define   WM8994_LO_HPOUT1L_VU_POSITION    8
int32_t   wm8994_lo_hpout1l_vu(wm8994_ctx_t *ctx, uint16_t value);


/*******************************************************************************
* Register      : WM8994_RIGHT_OUTPUT_VOL
* Address       : 0X1D
* Bit Group Name: HPOUT1R_VOL [5:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_RO_HPOUT1R_VOL_MASK        (uint16_t)0x003F
#define   WM8994_RO_HPOUT1R_VOL_POSITION    5
int32_t   wm8994_ro_hpout1r_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_RIGHT_OUTPUT_VOL
* Address       : 0X1D
* Bit Group Name: HPOUT1R_MUTE_N
* Permission    : RW
*******************************************************************************/
#define   WM8994_RO_HPOUT1R_MUTE_N_MASK        (uint16_t)0x0040U
#define   WM8994_RO_HPOUT1R_MUTE_N_POSITION    6
int32_t   wm8994_ro_hpout1r_mute_n(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_RIGHT_OUTPUT_VOL
* Address       : 0X1D
* Bit Group Name: HPOUT1R_ZC
* Permission    : RW
*******************************************************************************/
#define   WM8994_RO_HPOUT1R_ZC_MASK           (uint16_t)0x0080
#define   WM8994_RO_HPOUT1R_ZC_POSITION       7
int32_t   wm8994_ro_hpout1r_zc(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_RIGHT_OUTPUT_VOL
* Address       : 0X1D
* Bit Group Name: HPOUT1R_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_RO_HPOUT1R_VU_MASK          (uint16_t)0x0100
#define   WM8994_RO_HPOUT1R_VU_POSITION      8
int32_t   wm8994_ro_hpout1r_vu(wm8994_ctx_t *ctx, uint16_t value);

/* Note: Registers (uint16_t)0x1E, (uint16_t)0x1F, (uint16_t)0x20 and (uint16_t)0x21 are not implemented here */

/*******************************************************************************
* Register      : WM8994_SPKMIXL_ATT
* Address       : 0X22
* Bit Group Name: SPKMIXL_VOL[1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXL_ATT_VOL_MASK          (uint16_t)0x0003
#define   WM8994_SPKMIXL_ATT_VOL_POSITION      0
int32_t   wm8994_spkmixl_att_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXL_ATT
* Address       : 0X22
* Bit Group Name: DAC1L_SPKMIXL_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXL_ATT_DAC1_VOL_MASK          (uint16_t)0x0004
#define   WM8994_SPKMIXL_ATT_DAC1_VOL_POSITION      2
int32_t   wm8994_spkmixl_att_dac1_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXL_ATT
* Address       : 0X22
* Bit Group Name: MIXOUTL_SPKMIXL_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXL_ATT_MIXOUTL_VOL_MASK          (uint16_t)0x0008
#define   WM8994_SPKMIXL_ATT_MIXOUTL_VOL_POSITION      3
int32_t   wm8994_spkmixl_att_mixoutl_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXL_ATT
* Address       : 0X22
* Bit Group Name: IN1LP_SPKMIXL_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXL_ATT_IN1LP_VOL_MASK          (uint16_t)0x0010
#define   WM8994_SPKMIXL_ATT_IN1LP_VOL_POSITION      4
int32_t   wm8994_spkmixl_att_in1lp_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXL_ATT
* Address       : 0X22
* Bit Group Name: MIXINL_SPKMIXL_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXL_ATT_MIXINL_VOL_MASK          (uint16_t)0x0020
#define   WM8994_SPKMIXL_ATT_MIXINL_VOL_POSITION      5
int32_t   wm8994_spkmixl_att_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXL_ATT
* Address       : 0X22
* Bit Group Name: DAC2L_SPKMIXL_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXL_ATT_DAC2L_VOL_MASK          (uint16_t)0x0040
#define   WM8994_SPKMIXL_ATT_DAC2L_VOL_POSITION      6
int32_t   wm8994_spkmixl_att_dac2l_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXL_ATT
* Address       : 0X22
* Bit Group Name: SPKAB_REF_SEL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXL_ATT_SPKAB_REFSEL_MASK          (uint16_t)0x0100
#define   WM8994_SPKMIXL_ATT_SPKAB_REFSEL_POSITION      8
int32_t   wm8994_spkmixl_att_spkab_refsel(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXR_ATT
* Address       : 0X23
* Bit Group Name: SPKMIXR_VOL[1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXR_ATT_VOL_MASK          (uint16_t)0x0003
#define   WM8994_SPKMIXR_ATT_VOL_POSITION      0
int32_t   wm8994_spkmixr_att_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXR_ATT
* Address       : 0X23
* Bit Group Name: DAC1R_SPKMIXR_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXR_ATT_DAC1_VOL_MASK          (uint16_t)0x0004
#define   WM8994_SPKMIXR_ATT_DAC1_VOL_POSITION      2
int32_t   wm8994_spkmixr_att_dac1_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXR_ATT
* Address       : 0X23
* Bit Group Name: MIXOUTR_SPKMIXR_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXR_ATT_MIXOUTL_VOL_MASK          (uint16_t)0x0008
#define   WM8994_SPKMIXR_ATT_MIXOUTL_VOL_POSITION      3
int32_t   wm8994_spkmixr_att_mixoutl_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXR_ATT
* Address       : 0X23
* Bit Group Name: IN1RP_SPKMIXR_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXR_ATT_IN1RP_VOL_MASK          (uint16_t)0x0010
#define   WM8994_SPKMIXR_ATT_IN1RP_VOL_POSITION      4
int32_t   wm8994_spkmixr_att_in1rp_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXR_ATT
* Address       : 0X23
* Bit Group Name: MIXINL_SPKMIXR_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXR_ATT_MIXINL_VOL_MASK          (uint16_t)0x0020
#define   WM8994_SPKMIXR_ATT_MIXINL_VOL_POSITION      5
int32_t   wm8994_spkmixr_att_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXR_ATT
* Address       : 0X23
* Bit Group Name: DAC2R_SPKMIXR_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXR_ATT_DAC2R_VOL_MASK          (uint16_t)0x0040
#define   WM8994_SPKMIXR_ATT_DAC2R_VOL_POSITION      6
int32_t   wm8994_spkmixr_att_dac2r_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPKMIXR_ATT
* Address       : 0X23
* Bit Group Name: SPKOUT_CLASSAB
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXR_ATT_SPKOUT_CLASSAB_MASK          (uint16_t)0x0100
#define   WM8994_SPKMIXR_ATT_SPKOUT_CLASSAB_POSITION      8
int32_t   wm8994_spkmixr_att_spkout_classab(wm8994_ctx_t *ctx, uint16_t value);

/* Note: Registers (uint16_t)0x24 and (uint16_t)0x25 are not implemented here */

/*******************************************************************************
* Register      : WM8994_SPK_LEFT_VOL
* Address       : 0X26
* Bit Group Name: SPKOUTL_VOL [5:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPK_LEFT_VOL_SPKOUT_VOL_MASK          (uint16_t)0x003F
#define   WM8994_SPK_LEFT_VOL_SPKOUT_VOL_POSITION      5
int32_t   wm8994_spk_left_vol_spkout_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPK_LEFT_VOL
* Address       : 0X26
* Bit Group Name: SPKOUTL_MUTE_N
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPK_LEFT_VOL_SPKOUT_MUTE_N_MASK          (uint16_t)0x0040
#define   WM8994_SPK_LEFT_VOL_SPKOUT_MUTE_N_POSITION      6
int32_t   wm8994_spk_left_vol_spkout_mute_n(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPK_LEFT_VOL
* Address       : 0X26
* Bit Group Name: SPKOUTL_ZC
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPK_LEFT_VOL_SPKOUT_ZC_MASK          (uint16_t)0x0080
#define   WM8994_SPK_LEFT_VOL_SPKOUT_ZC_POSITION      7
int32_t   wm8994_spk_left_vol_spkout_zc(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPK_LEFT_VOL
* Address       : 0X26
* Bit Group Name: SPKOUT_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPK_LEFT_VOL_SPKOUT_VU_MASK          (uint16_t)0x0100
#define   WM8994_SPK_LEFT_VOL_SPKOUT_VU_POSITION      8
int32_t   wm8994_spk_left_vol_spkout_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPK_RIGHT_VOL
* Address       : 0X27
* Bit Group Name: SPKOUTR_VOL [5:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPK_RIGHT_VOL_SPKOUT_VOL_MASK          (uint16_t)0x003F
#define   WM8994_SPK_RIGHT_VOL_SPKOUT_VOL_POSITION      5
int32_t   wm8994_spk_right_vol_spkout_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPK_RIGHT_VOL
* Address       : 0X27
* Bit Group Name: SPKOUTR_MUTE_N
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPK_RIGHT_VOL_SPKOUT_MUTE_N_MASK          (uint16_t)0x0040
#define   WM8994_SPK_RIGHT_VOL_SPKOUT_MUTE_N_POSITION      6
int32_t   wm8994_spk_right_vol_spkout_mute_n(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPK_RIGHT_VOL
* Address       : 0X27
* Bit Group Name: SPKOUTR_ZC
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPK_RIGHT_VOL_SPKOUT_ZC_MASK          (uint16_t)0x0080
#define   WM8994_SPK_RIGHT_VOL_SPKOUT_ZC_POSITION      7
int32_t   wm8994_spk_right_vol_spkout_zc(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPK_RIGHT_VOL
* Address       : 0X27
* Bit Group Name: SPKOUT_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPK_RIGHT_VOL_SPKOUT_VU_MASK          (uint16_t)0x0100
#define   WM8994_SPK_RIGHT_VOL_SPKOUT_VU_POSITION      8
int32_t   wm8994_spk_right_vol_spkout_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_2
* Address       : 0X28
* Bit Group Name: IN1RN_TO_IN1R
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER2_IN1RN_TO_IN1R_MASK          (uint16_t)0x0001
#define   WM8994_INMIXER2_IN1RN_TO_IN1R_POSITION      0
int32_t   wm8994_inmixer2_in1rn_to_in1r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_2
* Address       : 0X28
* Bit Group Name: IN1RP_TO_IN1R
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER2_IN1RP_TO_IN1R_MASK          (uint16_t)0x0002
#define   WM8994_INMIXER2_IN1RP_TO_IN1R_POSITION      1
int32_t   wm8994_inmixer2_in1rp_to_in1r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_2
* Address       : 0X28
* Bit Group Name: IN2RN_TO_IN2R
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER2_IN2RN_TO_IN2R_MASK          (uint16_t)0x0004
#define   WM8994_INMIXER2_IN2RN_TO_IN2R_POSITION      2
int32_t   wm8994_inmixer2_in2rn_to_in2r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_2
* Address       : 0X28
* Bit Group Name: IN2RP_TO_IN2R
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER2_IN2RP_TO_IN2R_MASK          (uint16_t)0x0008
#define   WM8994_INMIXER2_IN2RP_TO_IN2R_POSITION      3
int32_t   wm8994_inmixer2_in2rp_to_in2r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_2
* Address       : 0X28
* Bit Group Name: IN1LN_TO_IN1L
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER2_IN1LN_TO_IN1L_MASK          (uint16_t)0x0010
#define   WM8994_INMIXER2_IN1LN_TO_IN1L_POSITION      4
int32_t   wm8994_inmixer2_in1ln_to_in1l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_2
* Address       : 0X28
* Bit Group Name: IN1LP_TO_IN1L
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER2_IN1LP_TO_IN1L_MASK          (uint16_t)0x0020
#define   WM8994_INMIXER2_IN1LP_TO_IN1L_POSITION      5
int32_t   wm8994_inmixer2_in1lp_to_in1l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_2
* Address       : 0X28
* Bit Group Name: IN2LN_TO_IN2L
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER2_IN2LN_TO_IN2L_MASK          (uint16_t)0x0040
#define   WM8994_INMIXER2_IN2LN_TO_IN2L_POSITION      6
int32_t   wm8994_inmixer2_in2ln_to_in2l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_2
* Address       : 0X28
* Bit Group Name: IN2LP_TO_IN2L
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER2_IN2LP_TO_IN2L_MASK          (uint16_t)0x0080
#define   WM8994_INMIXER2_IN2LP_TO_IN2L_POSITION      7
int32_t   wm8994_inmixer2_in2lp_to_in2l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_3
* Address       : 0X29
* Bit Group Name: MIXOUTL_MIXINL_VOL [2:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER3_MIXOUTL_MIXINL_VOL_MASK          (uint16_t)0x0007
#define   WM8994_INMIXER3_MIXOUTL_MIXINL_VOL_POSITION      0
int32_t   wm8994_inmixer3_mixoutl_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_3
* Address       : 0X29
* Bit Group Name: IN1L_MIXINL_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER3_IN1L_MIXINL_VOL_MASK          (uint16_t)0x0010
#define   WM8994_INMIXER3_IN1L_MIXINL_VOL_POSITION      4
int32_t   wm8994_inmixer3_in1l_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_3
* Address       : 0X29
* Bit Group Name: IN1L_TO_MIXINL
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER3_IN1L_TO_MIXINL_MASK          (uint16_t)0x0020
#define   WM8994_INMIXER3_IN1L_TO_MIXINL_POSITION      5
int32_t   wm8994_inmixer3_in1l_to_mixinl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_3
* Address       : 0X29
* Bit Group Name: IN2L_MIXINL_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER3_IN2L_MIXINL_VOL_MASK          (uint16_t)0x0080
#define   WM8994_INMIXER3_IN2L_MIXINL_VOL_POSITION      7
int32_t   wm8994_inmixer3_in2l_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_3
* Address       : 0X29
* Bit Group Name: IN2L_TO_MIXINL
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER3_IN2L_TO_MIXINL_MASK          (uint16_t)0x0100
#define   WM8994_INMIXER3_IN2L_TO_MIXINL_POSITION      8
int32_t   wm8994_inmixer3_in2l_to_mixinl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_4
* Address       : 0X2A
* Bit Group Name: MIXOUTR_MIXINR_VOL [2:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER4_MIXOUTR_MIXINR_VOL_MASK          (uint16_t)0x0007
#define   WM8994_INMIXER4_MIXOUTR_MIXINR_VOL_POSITION      0
int32_t   wm8994_inmixer4_mixoutr_mixinr_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_4
* Address       : 0X2A
* Bit Group Name: IN1R_MIXINR_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER4_IN1R_MIXINR_VOL_MASK          (uint16_t)0x0010
#define   WM8994_INMIXER4_IN1R_MIXINR_VOL_POSITION      4
int32_t   wm8994_inmixer4_in1r_mixinr_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_4
* Address       : 0X2A
* Bit Group Name: IN1R_TO_MIXINR
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER4_IN1R_TO_MIXINR_MASK          (uint16_t)0x0020
#define   WM8994_INMIXER4_IN1R_TO_MIXINR_POSITION      5
int32_t   wm8994_inmixer4_in1r_to_mixinr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_4
* Address       : 0X2A
* Bit Group Name: IN2R_MIXINR_VOL
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER4_IN2R_MIXINR_VOL_MASK          (uint16_t)0x0080
#define   WM8994_INMIXER4_IN2R_MIXINR_VOL_POSITION      7
int32_t   wm8994_inmixer4_in2r_mixinr_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_4
* Address       : 0X2A
* Bit Group Name: IN2R_TO_MIXINR
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER4_IN2R_TO_MIXINR_MASK          (uint16_t)0x0100
#define   WM8994_INMIXER4_IN2R_TO_MIXINR_POSITION      8
int32_t   wm8994_inmixer4_in2r_to_mixinr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_5
* Address       : 0X2B
* Bit Group Name: IN2LRP_MIXINL_VOL [2:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER5_IN2LRP_MIXINL_VOL_MASK          (uint16_t)0x0007
#define   WM8994_INMIXER5_IN2LRP_MIXINL_VOL_POSITION      0
int32_t   wm8994_inmixer5_in2lrp_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_5
* Address       : 0X2B
* Bit Group Name: IN1LP_MIXINL_VOL [2:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER5_IN1LP_MIXINL_VOL_MASK          (uint16_t)0x01C0
#define   WM8994_INMIXER5_IN1LP_MIXINL_VOL_POSITION      6
int32_t   wm8994_inmixer5_in1lp_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value);


/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_6
* Address       : 0X2C
* Bit Group Name: IN2LRP_MIXINR_VOL [2:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER6_IN2LRP_MIXINR_VOL_MASK          (uint16_t)0x0007
#define   WM8994_INMIXER6_IN2LRP_MIXINR_VOL_POSITION      0
int32_t   wm8994_inmixer6_in2lrp_mixinr_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_INPUT_MIXER_6
* Address       : 0X2C
* Bit Group Name: IN1RP_MIXINR_VOL [2:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_INMIXER6_IN1RP_MIXINR_VOL_MASK          (uint16_t)0x01C0
#define   WM8994_INMIXER6_IN1RP_MIXINR_VOL_POSITION      6
int32_t   wm8994_inmixer6_in1rp_mixinr_vol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_1
* Address       : 0X2D
* Bit Group Name: DAC1L_TO_MIXOUTL
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER1_DAC1L_TO_MIXOUTL_MASK          (uint16_t)0x0001
#define   WM8994_OUTMIXER1_DAC1L_TO_MIXOUTL_POSITION      0
int32_t   wm8994_outmixer1_dac1l_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_1
* Address       : 0X2D
* Bit Group Name: IN2LP_TO_MIXOUTL
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER1_IN2LP_TO_MIXOUTL_MASK          (uint16_t)0x0002
#define   WM8994_OUTMIXER1_IN2LP_TO_MIXOUTL_POSITION      1
int32_t   wm8994_outmixer1_in2lp_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_1
* Address       : 0X2D
* Bit Group Name: IN1L_TO_MIXOUTL
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER1_IN1L_TO_MIXOUTL_MASK          (uint16_t)0x0004
#define   WM8994_OUTMIXER1_IN1L_TO_MIXOUTL_POSITION      2
int32_t   wm8994_outmixer1_in1l_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_1
* Address       : 0X2D
* Bit Group Name: IN1R_TO_MIXOUTL
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER1_IN1R_TO_MIXOUTL_MASK          (uint16_t)0x0008
#define   WM8994_OUTMIXER1_IN1R_TO_MIXOUTL_POSITION      3
int32_t   wm8994_outmixer1_in1r_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_1
* Address       : 0X2D
* Bit Group Name: IN2LN_TO_MIXOUTL
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER1_IN2LN_TO_MIXOUTL_MASK          (uint16_t)0x0010
#define   WM8994_OUTMIXER1_IN2LN_TO_MIXOUTL_POSITION      4
int32_t   wm8994_outmixer1_in2ln_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_1
* Address       : 0X2D
* Bit Group Name: IN2RN_TO_MIXOUTL
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER1_IN2RN_TO_MIXOUTL_MASK          (uint16_t)0x0020
#define   WM8994_OUTMIXER1_IN2RN_TO_MIXOUTL_POSITION      5
int32_t   wm8994_outmixer1_in2rn_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_1
* Address       : 0X2D
* Bit Group Name: MIXINL_TO_MIXOUTL
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER1_MIXINL_TO_MIXOUTL_MASK          (uint16_t)0x0040
#define   WM8994_OUTMIXER1_MIXINL_TO_MIXOUTL_POSITION      6
int32_t   wm8994_outmixer1_mixinl_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_1
* Address       : 0X2D
* Bit Group Name: MIXINR_TO_MIXOUTL
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER1_MIXINR_TO_MIXOUTL_MASK          (uint16_t)0x0080
#define   WM8994_OUTMIXER1_MIXINR_TO_MIXOUTL_POSITION      7
int32_t   wm8994_outmixer1_mixinr_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_1
* Address       : 0X2D
* Bit Group Name: DAC1L_TO_HPOUT1L
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER1_DAC1L_TO_HPOUT1L_MASK          (uint16_t)0x0100
#define   WM8994_OUTMIXER1_DAC1L_TO_HPOUT1L_POSITION      8
int32_t   wm8994_outmixer1_dac1l_to_hpout1l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_2
* Address       : 0X2E
* Bit Group Name: DAC1R_TO_MIXOUTR
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER2_DAC1R_TO_MIXOUTR_MASK          (uint16_t)0x0001
#define   WM8994_OUTMIXER2_DAC1R_TO_MIXOUTR_POSITION      0
int32_t   wm8994_outmixer2_dac1r_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_2
* Address       : 0X2E
* Bit Group Name: IN2RP_TO_MIXOUTR
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER2_IN2RP_TO_MIXOUTR_MASK          (uint16_t)0x0002
#define   WM8994_OUTMIXER2_IN2RP_TO_MIXOUTR_POSITION      1
int32_t   wm8994_outmixer2_in2rp_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_2
* Address       : 0X2E
* Bit Group Name: IN1R_TO_MIXOUTR
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER2_IN1R_TO_MIXOUTR_MASK          (uint16_t)0x0004
#define   WM8994_OUTMIXER2_IN1R_TO_MIXOUTR_POSITION      2
int32_t   wm8994_outmixer2_in1r_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_2
* Address       : 0X2E
* Bit Group Name: IN1L_TO_MIXOUTR
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER2_IN1L_TO_MIXOUTR_MASK          (uint16_t)0x0008
#define   WM8994_OUTMIXER2_IN1L_TO_MIXOUTR_POSITION      3
int32_t   wm8994_outmixer2_in1l_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_2
* Address       : 0X2E
* Bit Group Name: IN2RN_TO_MIXOUTR
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER2_IN2RN_TO_MIXOUTR_MASK          (uint16_t)0x0010
#define   WM8994_OUTMIXER2_IN2RN_TO_MIXOUTR_POSITION      4
int32_t   wm8994_outmixer2_in2rn_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_2
* Address       : 0X2E
* Bit Group Name: IN2LN_TO_MIXOUTR
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER2_IN2LN_TO_MIXOUTR_MASK          (uint16_t)0x0020
#define   WM8994_OUTMIXER2_IN2LN_TO_MIXOUTR_POSITION      5
int32_t   wm8994_outmixer2_in2ln_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_2
* Address       : 0X2E
* Bit Group Name: MIXINR_TO_MIXOUTR
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER2_MIXINR_TO_MIXOUTR_MASK          (uint16_t)0x0040
#define   WM8994_OUTMIXER2_MIXINR_TO_MIXOUTR_POSITION      6
int32_t   wm8994_outmixer2_mixinr_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_2
* Address       : 0X2E
* Bit Group Name: MIXINL_TO_MIXOUTR
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER2_MIXINL_TO_MIXOUTR_MASK          (uint16_t)0x0080
#define   WM8994_OUTMIXER2_MIXINL_TO_MIXOUTR_POSITION      7
int32_t   wm8994_outmixer2_mixinl_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OUTPUT_MIXER_2
* Address       : 0X2E
* Bit Group Name: DAC1R_TO_HPOUT1R
* Permission    : RW
*******************************************************************************/
#define   WM8994_OUTMIXER2_DAC1R_TO_HPOUT1R_MASK          (uint16_t)0x0100
#define   WM8994_OUTMIXER2_DAC1R_TO_HPOUT1R_POSITION      8
int32_t   wm8994_outmixer2_dac1r_to_hpout1r(wm8994_ctx_t *ctx, uint16_t value);

/* Note: Registers (uint16_t)0x2F to (uint16_t)0x35 are not implemented here */

/*******************************************************************************
* Register      : WM8994_SPEAKER_MIXER
* Address       : 0X36
* Bit Group Name: DAC1R_TO_SPKMIXR
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXER_DAC1R_TO_SPKMIXR_MASK          (uint16_t)0x0001
#define   WM8994_SPKMIXER_DAC1R_TO_SPKMIXR_POSITION      0
int32_t   wm8994_spkmixer_dac1r_to_spkmixr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPEAKER_MIXER
* Address       : 0X36
* Bit Group Name: DAC1L_TO_SPKMIXL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXER_DAC1L_TO_SPKMIXL_MASK          (uint16_t)0x0002
#define   WM8994_SPKMIXER_DAC1L_TO_SPKMIXL_POSITION      1
int32_t   wm8994_spkmixer_dac1l_to_spkmixl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPEAKER_MIXER
* Address       : 0X36
* Bit Group Name: MIXOUTR_TO_SPKMIXR
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXER_MIXOUTR_TO_SPKMIXR_MASK          (uint16_t)0x0004
#define   WM8994_SPKMIXER_MIXOUTR_TO_SPKMIXR_POSITION      2
int32_t   wm8994_spkmixer_mixoutr_to_spkmixr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPEAKER_MIXER
* Address       : 0X36
* Bit Group Name: MIXOUTL_TO_SPKMIXL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXER_MIXOUTL_TO_SPKMIXL_MASK          (uint16_t)0x0008
#define   WM8994_SPKMIXER_MIXOUTL_TO_SPKMIXL_POSITION      3
int32_t   wm8994_spkmixer_mixoutl_to_spkmixl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPEAKER_MIXER
* Address       : 0X36
* Bit Group Name: IN1RP_TO_SPKMIXR
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXER_IN1RP_TO_SPKMIXR_MASK          (uint16_t)0x0010
#define   WM8994_SPKMIXER_IN1RP_TO_SPKMIXR_POSITION      4
int32_t   wm8994_spkmixer_in1rp_to_spkmixr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPEAKER_MIXER
* Address       : 0X36
* Bit Group Name: IN1LP_TO_SPKMIXL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXER_IN1LP_TO_SPKMIXL_MASK          (uint16_t)0x0020
#define   WM8994_SPKMIXER_IN1LP_TO_SPKMIXL_POSITION      5
int32_t   wm8994_spkmixer_in1lp_to_spkmixl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPEAKER_MIXER
* Address       : 0X36
* Bit Group Name: MIXINR_TO_SPKMIXR
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXER_MIXINR_TO_SPKMIXR_MASK          (uint16_t)0x0040
#define   WM8994_SPKMIXER_MIXINR_TO_SPKMIXR_POSITION      6
int32_t   wm8994_spkmixer_mixinr_to_spkmixr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPEAKER_MIXER
* Address       : 0X36
* Bit Group Name: MIXINL_TO_SPKMIXL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXER_MIXINL_TO_SPKMIXL_MASK          (uint16_t)0x0080
#define   WM8994_SPKMIXER_MIXINL_TO_SPKMIXL_POSITION      7
int32_t   wm8994_spkmixer_mixinl_to_spkmixl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPEAKER_MIXER
* Address       : 0X36
* Bit Group Name: DAC2R_TO_SPKMIXR
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXER_DAC2R_TO_SPKMIXR_MASK          (uint16_t)0x0100
#define   WM8994_SPKMIXER_DAC2R_TO_SPKMIXR_POSITION      8
int32_t   wm8994_spkmixer_dac2r_to_spkmixr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_SPEAKER_MIXER
* Address       : 0X36
* Bit Group Name: DAC2L_TO_SPKMIXL
* Permission    : RW
*******************************************************************************/
#define   WM8994_SPKMIXER_DAC2L_TO_SPKMIXL_MASK          (uint16_t)0x0200
#define   WM8994_SPKMIXER_DAC2L_TO_SPKMIXL_POSITION      9
int32_t   wm8994_spkmixer_dac2l_to_spkmixl(wm8994_ctx_t *ctx, uint16_t value);

/* Note: Registers (uint16_t)0x37 and (uint16_t)0x38 are not implemented here */

/*******************************************************************************
* Register      : WM8994_ANTIPOP2
* Address       : 0X39
* Bit Group Name: VMID_DISCH
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANTIPOP2_VMID_DISCH_MASK          (uint16_t)0x0001
#define   WM8994_ANTIPOP2_VMID_DISCH_POSITION      0
int32_t   wm8994_antipop2_vmid_disch(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_ANTIPOP2
* Address       : 0X39
* Bit Group Name: BIAS_SRC
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANTIPOP2_BIAS_SRC_MASK          (uint16_t)0x0002
#define   WM8994_ANTIPOP2_BIAS_SRC_POSITION      1
int32_t   wm8994_antipop2_bias_src(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_ANTIPOP2
* Address       : 0X39
* Bit Group Name: STARTUP_BIAS_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANTIPOP2_STARTUP_BIAS_ENA_MASK          (uint16_t)0x0004
#define   WM8994_ANTIPOP2_STARTUP_BIAS_ENA_POSITION      2
int32_t   wm8994_antipop2_startup_bias_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_ANTIPOP2
* Address       : 0X39
* Bit Group Name: VMID_BUF_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANTIPOP2_VMID_BUF_ENA_MASK          (uint16_t)0x0008
#define   WM8994_ANTIPOP2_VMID_BUF_ENA_POSITION      3
int32_t   wm8994_antipop2_vmid_buf_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_ANTIPOP2
* Address       : 0X39
* Bit Group Name: VMID_RAMP[1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANTIPOP2_VMID_RAMP_MASK          (uint16_t)0x0060
#define   WM8994_ANTIPOP2_VMID_RAMP_POSITION      5
int32_t   wm8994_antipop2_vmid_ramp(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_ANTIPOP2
* Address       : 0X39
* Bit Group Name: MICB1_DISCH
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANTIPOP2_MICB1_DISCH_MASK          (uint16_t)0x0080
#define   WM8994_ANTIPOP2_MICB1_DISCH_POSITION      7
int32_t   wm8994_antipop2_micb1_disch(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_ANTIPOP2
* Address       : 0X39
* Bit Group Name: MICB2_DISCH
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANTIPOP2_MICB2_DISCH_MASK          (uint16_t)0x0100
#define   WM8994_ANTIPOP2_MICB2_DISCH_POSITION      8
int32_t   wm8994_antipop2_micb2_disch(wm8994_ctx_t *ctx, uint16_t value);


/*******************************************************************************
* Register      : WM8994_CHARGE_PUMP1
* Address       : 0X4C
* Bit Group Name: CP_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_CHARGE_PUMP1_CP_ENA_MASK          (uint16_t)0x8000
#define   WM8994_CHARGE_PUMP1_CP_ENA_POSITION      15
int32_t   wm8994_charge_pump1_cp_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_CHARGE_PUMP2
* Address       : 0X4D
* Bit Group Name: CP_DISCH
* Permission    : RW
*******************************************************************************/
#define   WM8994_CHARGE_PUMP2_CP_DISCH_MASK          (uint16_t)0x8000
#define   WM8994_CHARGE_PUMP2_CP_DISCH_POSITION      15
int32_t   wm8994_charge_pump2_cp_disch(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_CLASS_W
* Address       : 0X51
* Bit Group Name: CP_DYN_PWR
* Permission    : RW
*******************************************************************************/
#define   WM8994_CLASS_W_CP_DYN_PWR_MASK          (uint16_t)0x0001
#define   WM8994_CLASS_W_CP_DYN_PWR_POSITION      0
int32_t   wm8994_class_w_cp_dyn_pwr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_CLASS_W
* Address       : 0X51
* Bit Group Name: CP_DYN_SRC_SEL [1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_CLASS_W_CP_DYN_SRC_SEL_MASK          (uint16_t)0x0300
#define   WM8994_CLASS_W_CP_DYN_SRC_SEL_POSITION      8
int32_t   wm8994_class_w_cp_dyn_src_sel(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DC_SERVO1
* Address       : 0X54
* Bit Group Name: DCS_ENA_CHAN_0
* Permission    : RW
*******************************************************************************/
#define   WM8994_DC_SERVO1_DCS_ENA_CHAN_0_MASK          (uint16_t)0x0001
#define   WM8994_DC_SERVO1_DCS_ENA_CHAN_0_POSITION      0
int32_t   wm8994_dc_servo1_dcs_ena_chan_0(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DC_SERVO1
* Address       : 0X54
* Bit Group Name: DCS_ENA_CHAN_1
* Permission    : RW
*******************************************************************************/
#define   WM8994_DC_SERVO1_DCS_ENA_CHAN_1_MASK          (uint16_t)0x0002
#define   WM8994_DC_SERVO1_DCS_ENA_CHAN_1_POSITION      1
int32_t   wm8994_dc_servo1_dcs_ena_chan_1(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DC_SERVO1
* Address       : 0X54
* Bit Group Name: DCS_TRIG_DAC_WR_0
* Permission    : RW
*******************************************************************************/
#define   WM8994_DC_SERVO1_DCS_TRIG_DAC_WR_0_MASK          (uint16_t)0x0004
#define   WM8994_DC_SERVO1_DCS_TRIG_DAC_WR_0_POSITION      2
int32_t   wm8994_dc_servo1_dcs_trig_dac_wr_0(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DC_SERVO1
* Address       : 0X54
* Bit Group Name: DCS_TRIG_DAC_WR_1
* Permission    : RW
*******************************************************************************/
#define   WM8994_DC_SERVO1_DCS_TRIG_DAC_WR_1_MASK          (uint16_t)0x0008
#define   WM8994_DC_SERVO1_DCS_TRIG_DAC_WR_1_POSITION      3
int32_t   wm8994_dc_servo1_dcs_trig_dac_wr_1(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DC_SERVO1
* Address       : 0X54
* Bit Group Name: DCS_TRIG_STARTUP_0
* Permission    : RW
*******************************************************************************/
#define   WM8994_DC_SERVO1_DCS_TRIG_STARTUP_0_MASK          (uint16_t)0x0010
#define   WM8994_DC_SERVO1_DCS_TRIG_STARTUP_0_POSITION      4
int32_t   wm8994_dc_servo1_dcs_trig_startup_0(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DC_SERVO1
* Address       : 0X54
* Bit Group Name: DCS_TRIG_STARTUP_1
* Permission    : RW
*******************************************************************************/
#define   WM8994_DC_SERVO1_DCS_TRIG_STARTUP_1_MASK          (uint16_t)0x0020
#define   WM8994_DC_SERVO1_DCS_TRIG_STARTUP_1_POSITION      5
int32_t   wm8994_dc_servo1_dcs_trig_startup_1(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DC_SERVO1
* Address       : 0X54
* Bit Group Name: DCS_TRIG_SERIES_0
* Permission    : RW
*******************************************************************************/
#define   WM8994_DC_SERVO1_DCS_TRIG_SERIES_0_MASK          (uint16_t)0x0100
#define   WM8994_DC_SERVO1_DCS_TRIG_SERIES_0_POSITION      8
int32_t   wm8994_dc_servo1_dcs_trig_series_0(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DC_SERVO1
* Address       : 0X54
* Bit Group Name: DCS_TRIG_SERIES_1
* Permission    : RW
*******************************************************************************/
#define   WM8994_DC_SERVO1_DCS_TRIG_SERIES_1_MASK          (uint16_t)0x0200
#define   WM8994_DC_SERVO1_DCS_TRIG_SERIES_1_POSITION      9
int32_t   wm8994_dc_servo1_dcs_trig_series_1(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DC_SERVO1
* Address       : 0X54
* Bit Group Name: DCS_TRIG_SINGLE_0
* Permission    : RW
*******************************************************************************/
#define   WM8994_DC_SERVO1_DCS_TRIG_SINGLE_0_MASK          (uint16_t)0x1000
#define   WM8994_DC_SERVO1_DCS_TRIG_SINGLE_0_POSITION      12
int32_t   wm8994_dc_servo1_dcs_trig_single_0(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DC_SERVO1
* Address       : 0X54
* Bit Group Name: DCS_TRIG_SINGLE_1
* Permission    : RW
*******************************************************************************/
#define   WM8994_DC_SERVO1_DCS_TRIG_SINGLE_1_MASK          (uint16_t)0x2000
#define   WM8994_DC_SERVO1_DCS_TRIG_SINGLE_1_POSITION      13
int32_t   wm8994_dc_servo1_dcs_trig_single_1(wm8994_ctx_t *ctx, uint16_t value);

/* Note: Registers (uint16_t)0x55, (uint16_t)0x58 and (uint16_t)0x59 are not implemented here */

/*******************************************************************************
* Register      : WM8994_ANALOG_HP
* Address       : 0X60
* Bit Group Name: HPOUT1R_DLY
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANALOG_HP_HPOUT1R_DLY_MASK                 (uint16_t)0x0002
#define   WM8994_ANALOG_HP_HPOUT1R_DLY_POSITION             1
int32_t   wm8994_analog_hp_hpout1r_dly(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_ANALOG_HP
* Address       : 0X60
* Bit Group Name: HPOUT1R_OUTP
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANALOG_HP_HPOUT1R_OUTP_MASK                 (uint16_t)0x0004
#define   WM8994_ANALOG_HP_HPOUT1R_OUTP_POSITION             2
int32_t   wm8994_analog_hp_hpout1r_outp(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_ANALOG_HP
* Address       : 0X60
* Bit Group Name: HPOUT1R_RMV_SHORT
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANALOG_HP_HPOUT1R_RMV_SHORT_MASK                 (uint16_t)0x0008
#define   WM8994_ANALOG_HP_HPOUT1R_RMV_SHORT_POSITION             3
int32_t   wm8994_analog_hp_hpout1r_rmv_short(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_ANALOG_HP
* Address       : 0X60
* Bit Group Name: HPOUT1L_DLY
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANALOG_HP_HPOUT1L_DLY_MASK                 (uint16_t)0x0020
#define   WM8994_ANALOG_HP_HPOUT1L_DLY_POSITION             5
int32_t   wm8994_analog_hp_hpout1l_dly(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_ANALOG_HP
* Address       : 0X60
* Bit Group Name: HPOUT1L_OUTP
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANALOG_HP_HPOUT1L_OUTP_MASK                 (uint16_t)0x0040
#define   WM8994_ANALOG_HP_HPOUT1L_OUTP_POSITION             6
int32_t   wm8994_analog_hp_hpout1l_outp(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_ANALOG_HP
* Address       : 0X60
* Bit Group Name: HPOUT1L_RMV_SHORT
* Permission    : RW
*******************************************************************************/
#define   WM8994_ANALOG_HP_HPOUT1L_RMV_SHORT_MASK                 (uint16_t)0x0080
#define   WM8994_ANALOG_HP_HPOUT1L_RMV_SHORT_POSITION             7
int32_t   wm8994_analog_hp_hpout1l_rmv_short(wm8994_ctx_t *ctx, uint16_t value);

/* Note: Registers (uint16_t)0x100, (uint16_t)0x101 and (uint16_t)0x111 are not implemented here */

/*******************************************************************************
* Register      : WM8994_WRITE_SEQ_CTRL1
* Address       : 0X110
* Bit Group Name: WSEQ_START_INDEX [6:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_WSEQ_CTRL1_START_INDEX_MASK                 (uint16_t)0x007F
#define   WM8994_WSEQ_CTRL1_START_INDEX_POSITION             0
int32_t   wm8994_wseq_ctrl1_start_index(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_WRITE_SEQ_CTRL1
* Address       : 0X110
* Bit Group Name: WSEQ_START
* Permission    : RW
*******************************************************************************/
#define   WM8994_WSEQ_CTRL1_START_MASK                 (uint16_t)0x0100
#define   WM8994_WSEQ_CTRL1_START_POSITION             8
int32_t   wm8994_wseq_ctrl1_start(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_WRITE_SEQ_CTRL1
* Address       : 0X110
* Bit Group Name: WSEQ_ABORT
* Permission    : RW
*******************************************************************************/
#define   WM8994_WSEQ_CTRL1_ABORT_MASK                 (uint16_t)0x0200
#define   WM8994_WSEQ_CTRL1_ABORT_POSITION             9
int32_t   wm8994_wseq_ctrl1_abort(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_WRITE_SEQ_CTRL1
* Address       : 0X110
* Bit Group Name: WSEQ_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_WSEQ_CTRL1_ENA_MASK                 (uint16_t)0x8000
#define   WM8994_WSEQ_CTRL1_ENA_POSITION             15
int32_t   wm8994_wseq_ctrl1_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_CLOCKING1
* Address       : 0X200
* Bit Group Name: AIF1CLK_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CLOCKING1_ENA_MASK                 (uint16_t)0x0001
#define   WM8994_AIF1_CLOCKING1_ENA_POSITION             0
int32_t   wm8994_aif1_clocking1_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_CLOCKING1
* Address       : 0X200
* Bit Group Name: AIF1CLK_DIV
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CLOCKING1_DIV_MASK                 (uint16_t)0x0002
#define   WM8994_AIF1_CLOCKING1_DIV_POSITION             1
int32_t   wm8994_aif1_clocking1_div(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_CLOCKING1
* Address       : 0X200
* Bit Group Name: AIF1CLK_INV
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CLOCKING1_INV_MASK                 (uint16_t)0x0004
#define   WM8994_AIF1_CLOCKING1_INV_POSITION             2
int32_t   wm8994_aif1_clocking1_inv(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_CLOCKING1
* Address       : 0X200
* Bit Group Name: AIF1CLK_SRC [1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CLOCKING1_SRC_MASK                 (uint16_t)0x0018U
#define   WM8994_AIF1_CLOCKING1_SRC_POSITION             3
int32_t   wm8994_aif1_clocking1_src(wm8994_ctx_t *ctx, uint16_t value);

/* Note: Registers (uint16_t)0x201, (uint16_t)0x204 and (uint16_t)0x205 are not implemented here */

/*******************************************************************************
* Register      : WM8994_CLOCKING1
* Address       : 0X208
* Bit Group Name: SYSCLK_SRC
* Permission    : RW
*******************************************************************************/
#define   WM8994_CLOCKING1_SYSCLK_SRC_MASK                 (uint16_t)0x0001
#define   WM8994_CLOCKING1_SYSCLK_SRC_POSITION             0
int32_t   wm8994_clocking1_sysclk_src(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_CLOCKING1
* Address       : 0X208
* Bit Group Name: SYSDSPCLK_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_CLOCKING1_SYSDSPCLK_ENA_MASK                 (uint16_t)0x0002
#define   WM8994_CLOCKING1_SYSDSPCLK_ENA_POSITION             1
int32_t   wm8994_clocking1_sysdspclk_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_CLOCKING1
* Address       : 0X208
* Bit Group Name: AIF2DSPCLK_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_CLOCKING1_AIF2DSPCLK_ENA_MASK                 (uint16_t)0x0004
#define   WM8994_CLOCKING1_AIF2DSPCLK_ENA_POSITION             2
int32_t   wm8994_clocking1_aif2dspclk_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_CLOCKING1
* Address       : 0X208
* Bit Group Name: AIF1DSPCLK_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_CLOCKING1_AIF1DSPCLK_ENA_MASK                 (uint16_t)0x0008
#define   WM8994_CLOCKING1_AIF1DSPCLK_ENA_POSITION             3
int32_t   wm8994_clocking1_aif1dspclk_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_CLOCKING1
* Address       : 0X208
* Bit Group Name: TOCLK_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_CLOCKING1_TOCLK_ENA_MASK                 (uint16_t)0x0010
#define   WM8994_CLOCKING1_TOCLK_ENA_POSITION             4
int32_t   wm8994_clocking1_toclk_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_CLOCKING2
* Address       : 0X209
* Bit Group Name: OPCLK_DIV[2:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_CLOCKING2_OPCLK_DIV_MASK                 (uint16_t)0x0007
#define   WM8994_CLOCKING2_OPCLK_DIV_POSITION             0
int32_t   wm8994_clocking2_opclk_div(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_CLOCKING2
* Address       : 0X209
* Bit Group Name: DBCLK_DIV[2:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_CLOCKING2_DBCLK_DIV_MASK                 (uint16_t)0x0070
#define   WM8994_CLOCKING2_DBCLK_DIV_POSITION             4
int32_t   wm8994_clocking2_dbclk_div(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_CLOCKING2
* Address       : 0X209
* Bit Group Name: TOCLK_DIV[2:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_CLOCKING2_TOCLK_DIV_MASK                 (uint16_t)0x0700
#define   WM8994_CLOCKING2_TOCLK_DIV_POSITION             8
int32_t   wm8994_clocking2_toclk_div(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_RATE
* Address       : 0X210
* Bit Group Name: AIF1CLK_RATE [3:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CLK_RATE_MASK                 (uint16_t)0x000F
#define   WM8994_AIF1_CLK_RATE_POSITION             0
int32_t   wm8994_aif1_clk_rate(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_RATE
* Address       : 0X210
* Bit Group Name: AIF1_SR [3:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_SR_MASK                 (uint16_t)0x00F0
#define   WM8994_AIF1_SR_POSITION             4
int32_t   wm8994_aif1_sr(wm8994_ctx_t *ctx, uint16_t value);
int32_t   wm8994_aif1_sr_r(wm8994_ctx_t *ctx, uint16_t *value);

/* Note: Registers (uint16_t)0x211, (uint16_t)0x212, (uint16_t)0x220, (uint16_t)0x221, (uint16_t)0x222, (uint16_t)0x223, (uint16_t)0x224,
         (uint16_t)0x240, (uint16_t)0x241, (uint16_t)0x242, (uint16_t)0x243 and (uint16_t)0x244  are not implemented here */

/*******************************************************************************
* Register      : WM8994_AIF1_CONTROL1
* Address       : 0X300
* Bit Group Name: AIF1_FMT [1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CONTROL1_FMT_MASK                 (uint16_t)0x0018U
#define   WM8994_AIF1_CONTROL1_FMT_POSITION             3
int32_t   wm8994_aif1_control1_fmt(wm8994_ctx_t *ctx, uint16_t value);
int32_t   wm8994_aif1_control1_fmt_r(wm8994_ctx_t *ctx, uint16_t *value);

/*******************************************************************************
* Register      : WM8994_AIF1_CONTROL1
* Address       : 0X300
* Bit Group Name: AIF1_WL [1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CONTROL1_WL_MASK                 (uint16_t)0x0060
#define   WM8994_AIF1_CONTROL1_WL_POSITION             5
int32_t   wm8994_aif1_control1_wl(wm8994_ctx_t *ctx, uint16_t value);
int32_t   wm8994_aif1_control1_wl_r(wm8994_ctx_t *ctx, uint16_t *value);
/*******************************************************************************
* Register      : WM8994_AIF1_CONTROL1
* Address       : 0X300
* Bit Group Name: AIF1_LRCLK_INV
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CONTROL1_LRCLK_INV_MASK                 (uint16_t)0x0080
#define   WM8994_AIF1_CONTROL1_LRCLK_INV_POSITION             7
int32_t   wm8994_aif1_control1_lrclk_inv(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_CONTROL1
* Address       : 0X300
* Bit Group Name: AIF1_BCLK_INV
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CONTROL1_BCLK_INV_MASK                 (uint16_t)0x0100
#define   WM8994_AIF1_CONTROL1_BCLK_INV_POSITION             8
int32_t   wm8994_aif1_control1_bclk_inv(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_CONTROL1
* Address       : 0X300
* Bit Group Name: AIF1ADC_TDM
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CONTROL1_ADC_TDM_MASK                 (uint16_t)0x2000
#define   WM8994_AIF1_CONTROL1_ADC_TDM_POSITION             13
int32_t   wm8994_aif1_control1_adc_tdm(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_CONTROL1
* Address       : 0X300
* Bit Group Name: AIF1ADCR_SRC
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CONTROL1_ADCR_SRC_MASK                 (uint16_t)0x4000
#define   WM8994_AIF1_CONTROL1_ADCR_SRC_POSITION             14
int32_t   wm8994_aif1_control1_adcr_src(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_CONTROL1
* Address       : 0X300
* Bit Group Name: AIF1ADCL_SRC
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_CONTROL1_ADCL_SRC_MASK                 (uint16_t)0x8000
#define   WM8994_AIF1_CONTROL1_ADCL_SRC_POSITION             15
int32_t   wm8994_aif1_control1_adcl_src(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_MASTER_SLAVE
* Address       : 0X302
* Bit Group Name: AIF1_LRCLK_FRC
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_MS_LRCLK_FRC_MASK                 (uint16_t)0x1000
#define   WM8994_AIF1_MS_LRCLK_FRC_POSITION             12
int32_t   wm8994_aif1_ms_lrclk_frc(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_MASTER_SLAVE
* Address       : 0X302
* Bit Group Name: AIF1_CLK_FRC
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_MS_CLK_FRC_MASK                 (uint16_t)0x2000
#define   WM8994_AIF1_MS_CLK_FRC_POSITION             13
int32_t   wm8994_aif1_ms_clk_frc(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_MASTER_SLAVE
* Address       : 0X302
* Bit Group Name: AIF1_MSTR
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_MS_MSTR_MASK                 (uint16_t)0x4000
#define   WM8994_AIF1_MS_MSTR_POSITION             14
int32_t   wm8994_aif1_ms_mstr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_MASTER_SLAVE
* Address       : 0X302
* Bit Group Name: AIF1_TRI
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_MS_TRI_MASK                 (uint16_t)0x8000
#define   WM8994_AIF1_MS_TRI_POSITION             15
int32_t   wm8994_aif1_ms_tri(wm8994_ctx_t *ctx, uint16_t value);

/* Note: Registers (uint16_t)0x301, (uint16_t)0x303, (uint16_t)0x304, (uint16_t)0x305, (uint16_t)0x306, (uint16_t)0x307, (uint16_t)0x310, (uint16_t)0x311,
         (uint16_t)0x312, (uint16_t)0x313, (uint16_t)0x314, (uint16_t)0x315, (uint16_t)0x316 and (uint16_t)0x317  are not implemented here */

/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_LEFT_VOL
* Address       : 0X400
* Bit Group Name: AIF1ADC1L_VOL [7:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC1_LEFT_VOL_ADC1L_MASK                 (uint16_t)0x00FF
#define   WM8994_AIF1_ADC1_LEFT_VOL_ADC1L_POSITION             0
int32_t   wm8994_aif1_adc1_left_vol_adc1l(wm8994_ctx_t *ctx, uint16_t value);
int32_t   wm8994_aif1_adc1_left_vol_adc1l_r(wm8994_ctx_t *ctx, uint16_t *value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_LEFT_VOL
* Address       : 0X400
* Bit Group Name: AIF1ADC1L_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC1_LEFT_VOL_VU_MASK                 (uint16_t)0x0100
#define   WM8994_AIF1_ADC1_LEFT_VOL_VU_POSITION             8
int32_t   wm8994_aif1_adc1_left_vol_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_RIGHT_VOL
* Address       : 0X401
* Bit Group Name: AIF1ADC1R_VOL [7:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC1_RIGHT_VOL_ADC1R_MASK                 (uint16_t)0x00FF
#define   WM8994_AIF1_ADC1_RIGHT_VOL_ADC1R_POSITION             0
int32_t   wm8994_aif1_adc1_right_vol_adc1r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_RIGHT_VOL
* Address       : 0X401
* Bit Group Name: AIF1ADC1R_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC1_RIGHT_VOL_VU_MASK                 (uint16_t)0x0100
#define   WM8994_AIF1_ADC1_RIGHT_VOL_VU_POSITION             8
int32_t   wm8994_aif1_adc1_right_vol_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_LEFT_VOL
* Address       : 0X404
* Bit Group Name: AIF1ADC2L_VOL [7:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC2_LEFT_VOL_ADC2L_MASK                 (uint16_t)0x00FF
#define   WM8994_AIF1_ADC2_LEFT_VOL_ADC2L_POSITION             0
int32_t   wm8994_aif1_adc2_left_vol_adc2l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_LEFT_VOL
* Address       : 0X404
* Bit Group Name: AIF1ADC2L_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC2_LEFT_VOL_VU_MASK                 (uint16_t)0x0100
#define   WM8994_AIF1_ADC2_LEFT_VOL_VU_POSITION             8
int32_t   wm8994_aif1_adc2_left_vol_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_RIGHT_VOL
* Address       : 0X405
* Bit Group Name: AIF1ADC2R_VOL [7:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC2_RIGHT_VOL_ADC2R_MASK                 (uint16_t)0x00FF
#define   WM8994_AIF1_ADC2_RIGHT_VOL_ADC2R_POSITION             0
int32_t   wm8994_aif1_adc2_right_vol_adc2r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_RIGHT_VOL
* Address       : 0X405
* Bit Group Name: AIF1ADC2R_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC2_RIGHT_VOL_VU_MASK                 (uint16_t)0x0100
#define   WM8994_AIF1_ADC2_RIGHT_VOL_VU_POSITION             8
int32_t   wm8994_aif1_adc2_right_vol_vu(wm8994_ctx_t *ctx, uint16_t value);

/* Note: Registers (uint16_t)0x402, (uint16_t)0x403, (uint16_t)0x406 and (uint16_t)0x407 are not implemented here */

/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_FILTERS
* Address       : 0X410
* Bit Group Name: AIF1ADC1R_HPF
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC1_FILTERS_ADC1R_HPF_MASK                 (uint16_t)0x0800
#define   WM8994_AIF1_ADC1_FILTERS_ADC1R_HPF_POSITION             11
int32_t   wm8994_aif1_adc1_filters_adc1r_hpf(wm8994_ctx_t *ctx, uint16_t value);
		 
/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_FILTERS
* Address       : 0X410
* Bit Group Name: AIF1ADC1L_HPF
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC1_FILTERS_ADC1L_HPF_MASK                 (uint16_t)0x1000
#define   WM8994_AIF1_ADC1_FILTERS_ADC1L_HPF_POSITION             12
int32_t   wm8994_aif1_adc1_filters_adc1l_hpf(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_FILTERS
* Address       : 0X410
* Bit Group Name: AIF1ADC1_HPF_CUT[1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC1_FILTERS_HPF_CUT_MASK                 (uint16_t)0x6000
#define   WM8994_AIF1_ADC1_FILTERS_HPF_CUT_POSITION             13
int32_t   wm8994_aif1_adc1_filters_hpf_cut(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_FILTERS
* Address       : 0X410
* Bit Group Name: AIF1ADC_4FS
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC1_FILTERS_4FS_MASK                 (uint16_t)0x8000
#define   WM8994_AIF1_ADC1_FILTERS_4FS_POSITION             15
int32_t   wm8994_aif1_adc1_filters_4fs(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_FILTERS
* Address       : 0X411
* Bit Group Name: AIF1ADC2R_HPF
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC2_FILTERS_ADC2R_HPF_MASK                 (uint16_t)0x0800
#define   WM8994_AIF1_ADC2_FILTERS_ADC2R_HPF_POSITION             11
int32_t   wm8994_aif1_adc2_filters_adc2r_hpf(wm8994_ctx_t *ctx, uint16_t value);
		 
/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_FILTERS
* Address       : 0X411
* Bit Group Name: AIF1ADC2L_HPF
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC2_FILTERS_ADC2L_HPF_MASK                 (uint16_t)0x1000
#define   WM8994_AIF1_ADC2_FILTERS_ADC2L_HPF_POSITION             12
int32_t   wm8994_aif1_adc2_filters_adc2l_hpf(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_FILTERS
* Address       : 0X411
* Bit Group Name: AIF1ADC2_HPF_CUT[1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC2_FILTERS_HPF_CUT_MASK                 (uint16_t)0x6000
#define   WM8994_AIF1_ADC2_FILTERS_HPF_CUT_POSITION             13
int32_t   wm8994_aif1_adc2_filters_hpf_cut(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_FILTERS
* Address       : 0X411
* Bit Group Name: AIF1ADC_4FS
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_ADC2_FILTERS_4FS_MASK                 (uint16_t)0x8000
#define   WM8994_AIF1_ADC2_FILTERS_4FS_POSITION             15
int32_t   wm8994_aif1_adc2_filters_4fs(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_FILTER1
* Address       : 0X420
* Bit Group Name: AIF1DAC1_DEEMP [1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_FILTER1_DEEMP_MASK                 (uint16_t)0x0006
#define   WM8994_AIF1_DAC1_FILTER1_DEEMP_POSITION             1
int32_t   wm8994_aif1_dac1_filter1_deemp(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_FILTER1
* Address       : 0X420
* Bit Group Name: AIF1DAC1_UNMUTE_RAMP
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_FILTER1_UNMUTE_RAMP_MASK                 (uint16_t)0x0010
#define   WM8994_AIF1_DAC1_FILTER1_UNMUTE_RAMP_POSITION             4
int32_t   wm8994_aif1_dac1_filter1_unmute_ramp(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_FILTER1
* Address       : 0X420
* Bit Group Name: AIF1DAC1_MUTERATE
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_FILTER1_MUTERATE_MASK                 (uint16_t)0x0020
#define   WM8994_AIF1_DAC1_FILTER1_MUTERATE_POSITION             5
int32_t   wm8994_aif1_dac1_filter1_muterate(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_FILTER1
* Address       : 0X420
* Bit Group Name: AIF1DAC1_MONO
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_FILTER1_MONO_MASK                 (uint16_t)0x0080
#define   WM8994_AIF1_DAC1_FILTER1_MONO_POSITION             7
int32_t   wm8994_aif1_dac1_filter1_mono(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_FILTER1
* Address       : 0X420
* Bit Group Name: AIF1DAC1_MUTE
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_FILTER1_MUTE_MASK                 (uint16_t)0x0200
#define   WM8994_AIF1_DAC1_FILTER1_MUTE_POSITION             9
int32_t   wm8994_aif1_dac1_filter1_mute(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_FILTER1
* Address       : 0X422
* Bit Group Name: AIF1DAC1_DEEMP [1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_FILTER1_DEEMP_MASK                 (uint16_t)0x0006
#define   WM8994_AIF1_DAC2_FILTER1_DEEMP_POSITION             1
int32_t   wm8994_aif1_dac2_filter1_deemp(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_FILTER1
* Address       : 0X422
* Bit Group Name: AIF1DAC1_UNMUTE_RAMP
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_FILTER1_UNMUTE_RAMP_MASK                 (uint16_t)0x0010
#define   WM8994_AIF1_DAC2_FILTER1_UNMUTE_RAMP_POSITION             4
int32_t   wm8994_aif1_dac2_filter1_unmute_ramp(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_FILTER1
* Address       : 0X422
* Bit Group Name: AIF1DAC1_MUTERATE
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_FILTER1_MUTERATE_MASK                 (uint16_t)0x0020
#define   WM8994_AIF1_DAC2_FILTER1_MUTERATE_POSITION             5
int32_t   wm8994_aif1_dac2_filter1_muterate(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_FILTER1
* Address       : 0X422
* Bit Group Name: AIF1DAC1_MONO
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_FILTER1_MONO_MASK                 (uint16_t)0x0080
#define   WM8994_AIF1_DAC2_FILTER1_MONO_POSITION             7
int32_t   wm8994_aif1_dac2_filter1_mono(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_FILTER1
* Address       : 0X422
* Bit Group Name: AIF1DAC1_MUTE
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_FILTER1_MUTE_MASK                 (uint16_t)0x0200
#define   WM8994_AIF1_DAC2_FILTER1_MUTE_POSITION             9
int32_t   wm8994_aif1_dac2_filter1_mute(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC1
* Address       : 0X440
* Bit Group Name: AIF1ADC1R_DRC_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC1_ADC1R_DRC_ENA_MASK                 (uint16_t)0x0001
#define   WM8994_AIF1DRC1_ADC1R_DRC_ENA_POSITION             0
int32_t   wm8994_aif1drc1_adc1r_drc_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC1
* Address       : 0X440
* Bit Group Name: AIF1ADC1L_DRC_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC1_ADC1L_DRC_ENA_MASK                 (uint16_t)0x0002
#define   WM8994_AIF1DRC1_ADC1L_DRC_ENA_POSITION             1
int32_t   wm8994_aif1drc1_adc1l_drc_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC1
* Address       : 0X440
* Bit Group Name: AIF1DAC1_DRC_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC1_DAC1_DRC_ENA_MASK                 (uint16_t)0x0004
#define   WM8994_AIF1DRC1_DAC1_DRC_ENA_POSITION             2
int32_t   wm8994_aif1drc1_dac1_drc_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC1
* Address       : 0X440
* Bit Group Name: AIF1DRC1_ANTICLIP
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC1_ANTICLIP_MASK                 (uint16_t)0x0008
#define   WM8994_AIF1DRC1_ANTICLIP_POSITION             3
int32_t   wm8994_aif1drc1_anticlip(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC1
* Address       : 0X440
* Bit Group Name: AIF1DRC1_QR
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC1_QR_MASK                 (uint16_t)0x0010
#define   WM8994_AIF1DRC1_QR_POSITION             4
int32_t   wm8994_aif1drc1_qr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC1
* Address       : 0X440
* Bit Group Name: AIF1DRC1_KNEE2_OP_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC1_KNEE2_OP_ENA_MASK                 (uint16_t)0x0020
#define   WM8994_AIF1DRC1_KNEE2_OP_ENA_POSITION             5
int32_t   wm8994_aif1drc1_knee2_op_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC1
* Address       : 0X440
* Bit Group Name: AIF1DRC1_SIG_DET
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC1_SIG_DET_MASK                 (uint16_t)0x0040
#define   WM8994_AIF1DRC1_SIG_DET_POSITION             6
int32_t   wm8994_aif1drc1_sig_det(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC1
* Address       : 0X440
* Bit Group Name: AIF1DRC1_SIG_DET_MODE
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC1_SIG_DET_MODE_MASK                 (uint16_t)0x0080
#define   WM8994_AIF1DRC1_SIG_DET_MODE_POSITION             7
int32_t   wm8994_aif1drc1_sig_det_mode(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC1
* Address       : 0X440
* Bit Group Name: AIF1DRC1_NG_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC1_NG_ENA_MASK                 (uint16_t)0x0100
#define   WM8994_AIF1DRC1_NG_ENA_POSITION             8
int32_t   wm8994_aif1drc1_ng_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC1
* Address       : 0X440
* Bit Group Name: AIF1DRC1_SIG_DET_PK[1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC1_SIG_DET_PK_MASK                 (uint16_t)0x0600
#define   WM8994_AIF1DRC1_SIG_DET_PK_POSITION             9
int32_t   wm8994_aif1drc1_sig_det_pk(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC1
* Address       : 0X440
* Bit Group Name: AIF1DRC1_SIG_DET_RMS[4:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC1_SIG_DET_RMS_MASK                 (uint16_t)0xF800
#define   WM8994_AIF1DRC1_SIG_DET_RMS_POSITION             11
int32_t   wm8994_aif1drc1_sig_det_rms(wm8994_ctx_t *ctx, uint16_t value);

/* Note: Registers (uint16_t)0x0421, (uint16_t)0x0423, and (uint16_t)0x0441 to (uint16_t)0x0444 are not implemented here */


/*******************************************************************************
* Register      : WM8994_AIF1_DRC2
* Address       : 0X450
* Bit Group Name: AIF1ADC2R_DRC_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC2_ADC2R_DRC_ENA_MASK                 (uint16_t)0x0001
#define   WM8994_AIF1DRC2_ADC2R_DRC_ENA_POSITION             0
int32_t   wm8994_aif1drc2_adc2r_drc_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC2
* Address       : 0X450
* Bit Group Name: AIF1ADC2L_DRC_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC2_ADC2L_DRC_ENA_MASK                 (uint16_t)0x0002
#define   WM8994_AIF1DRC2_ADC2L_DRC_ENA_POSITION             1
int32_t   wm8994_aif1drc2_adc2l_drc_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC2
* Address       : 0X450
* Bit Group Name: AIF1DAC2_DRC_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC2_DAC2_DRC_ENA_MASK                 (uint16_t)0x0004
#define   WM8994_AIF1DRC2_DAC2_DRC_ENA_POSITION             2
int32_t   wm8994_aif1drc2_dac2_drc_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC2
* Address       : 0X450
* Bit Group Name: AIF1DRC2_ANTICLIP
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC2_ANTICLIP_MASK                 (uint16_t)0x0008
#define   WM8994_AIF1DRC2_ANTICLIP_POSITION             3
int32_t   wm8994_aif1drc2_anticlip(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC2
* Address       : 0X450
* Bit Group Name: AIF1DRC2_QR
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC2_QR_MASK                 (uint16_t)0x0010
#define   WM8994_AIF1DRC2_QR_POSITION             4
int32_t   wm8994_aif1drc2_qr(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC2
* Address       : 0X450
* Bit Group Name: AIF1DRC2_KNEE2_OP_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC2_KNEE2_OP_ENA_MASK                 (uint16_t)0x0020
#define   WM8994_AIF1DRC2_KNEE2_OP_ENA_POSITION             5
int32_t   wm8994_aif1drc2_knee2_op_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC2
* Address       : 0X450
* Bit Group Name: AIF1DRC2_SIG_DET
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC2_SIG_DET_MASK                 (uint16_t)0x0040
#define   WM8994_AIF1DRC2_SIG_DET_POSITION             6
int32_t   wm8994_aif1drc2_sig_det(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC2
* Address       : 0X450
* Bit Group Name: AIF1DRC2_SIG_DET_MODE
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC2_SIG_DET_MODE_MASK                 (uint16_t)0x0080
#define   WM8994_AIF1DRC2_SIG_DET_MODE_POSITION             7
int32_t   wm8994_aif1drc2_sig_det_mode(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC2
* Address       : 0X450
* Bit Group Name: AIF1DRC2_NG_ENA
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC2_NG_ENA_MASK                 (uint16_t)0x0100
#define   WM8994_AIF1DRC2_NG_ENA_POSITION             8
int32_t   wm8994_aif1drc2_ng_ena(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC2
* Address       : 0X450
* Bit Group Name: AIF1DRC2_SIG_DET_PK[1:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC2_SIG_DET_PK_MASK                 (uint16_t)0x0600
#define   WM8994_AIF1DRC2_SIG_DET_PK_POSITION             9
int32_t   wm8994_aif1drc2_sig_det_pk(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DRC2
* Address       : 0X450
* Bit Group Name: AIF1DRC2_SIG_DET_RMS[4:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1DRC2_SIG_DET_RMS_MASK                 (uint16_t)0xF800
#define   WM8994_AIF1DRC2_SIG_DET_RMS_POSITION             11
int32_t   wm8994_aif1drc2_sig_det_rms(wm8994_ctx_t *ctx, uint16_t value);

/* Note: Registers (uint16_t)0x0451 to (uint16_t)0x0454
         Registers (uint16_t)0x0480 to (uint16_t)0x0493
         Registers (uint16_t)0x04A0 to (uint16_t)0x04B3
         Registers 0X0500, 0X0501, 0X0502, 0X0503, 0X0510, 0X0520, 0X0521
         Registers (uint16_t)0x0540 to (uint16_t)0x0544
         Registers (uint16_t)0x0580 to (uint16_t)0x0593
         are not implemented here */

/*******************************************************************************
* Register      : WM8994_DAC1_MIXER_VOL
* Address       : 0X600
* Bit Group Name: ADCL_DAC1_VOL [3:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC1_MIXER_VOL_ADCL_MASK                 (uint16_t)0x000F
#define   WM8994_DAC1_MIXER_VOL_ADCL_POSITION             0
int32_t   wm8994_dac1_mixer_vol_adcl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC1_MIXER_VOL
* Address       : 0X600
* Bit Group Name: ADCR_DAC1_VOL [3:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC1_MIXER_VOL_ADCR_MASK                 (uint16_t)0x001E
#define   WM8994_DAC1_MIXER_VOL_ADCR_POSITION             5
int32_t   wm8994_dac1_mixer_vol_adcr(wm8994_ctx_t *ctx, uint16_t value);
         
/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_LMR
* Address       : 0X601
* Bit Group Name: AIF1DAC1L_TO_DAC1L
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_LMRDAC1L_TO_DAC1L_MASK                 (uint16_t)0x0001
#define   WM8994_AIF1_DAC1_LMRDAC1L_TO_DAC1L_POSITION             0
int32_t   wm8994_aif1_dac1_lmrdac1l_to_dac1l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_LMR
* Address       : 0X601
* Bit Group Name: AIF1DAC2L_TO_DAC1L
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_LMRDAC2L_TO_DAC1L_MASK                 (uint16_t)0x0002
#define   WM8994_AIF1_DAC1_LMRDAC2L_TO_DAC1L_POSITION             1
int32_t   wm8994_aif1_dac1_lmrdac2l_to_dac1l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_LMR
* Address       : 0X601
* Bit Group Name: AIF1DACL_TO_DAC1L
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_LMRDACL_TO_DAC1L_MASK                 (uint16_t)0x0004
#define   WM8994_AIF1_DAC1_LMRDACL_TO_DAC1L_POSITION             2
int32_t   wm8994_aif1_dac1_lmrdacl_to_dac1l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_LMR
* Address       : 0X601
* Bit Group Name: AIF1ADCL_TO_DAC1L
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_LMRADCL_TO_DAC1L_MASK                 (uint16_t)0x0010
#define   WM8994_AIF1_DAC1_LMRADCL_TO_DAC1L_POSITION             4
int32_t   wm8994_aif1_dac1_lmradcl_to_dac1l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_LMR
* Address       : 0X601
* Bit Group Name: AIF1ADCR_TO_DAC1L
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_LMRADCR_TO_DAC1L_MASK                 (uint16_t)0x0020
#define   WM8994_AIF1_DAC1_LMRADCR_TO_DAC1L_POSITION             5
int32_t   wm8994_aif1_dac1_lmradcr_to_dac1l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_RMR
* Address       : 0X602
* Bit Group Name: AIF1DAC1R_TO_DAC1R
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_RMRDAC1R_TO_DAC1R_MASK                 (uint16_t)0x0001
#define   WM8994_AIF1_DAC1_RMRDAC1R_TO_DAC1R_POSITION             0
int32_t   wm8994_aif1_dac1_rmrdac1r_to_dac1r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_RMR
* Address       : 0X602
* Bit Group Name: AIF1DAC2R_TO_DAC1R
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_RMRDAC2R_TO_DAC1R_MASK                 (uint16_t)0x0002
#define   WM8994_AIF1_DAC1_RMRDAC2R_TO_DAC1R_POSITION             1
int32_t   wm8994_aif1_dac1_rmrdac2r_to_dac1r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_RMR
* Address       : 0X602
* Bit Group Name: AIF1DACR_TO_DAC1R
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_RMRDACR_TO_DAC1R_MASK                 (uint16_t)0x0004
#define   WM8994_AIF1_DAC1_RMRDACR_TO_DAC1R_POSITION             2
int32_t   wm8994_aif1_dac1_rmrdacr_to_dac1r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_RMR
* Address       : 0X602
* Bit Group Name: AIF1ADCL_TO_DAC1R
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_RMRADCL_TO_DAC1R_MASK                 (uint16_t)0x0010
#define   WM8994_AIF1_DAC1_RMRADCL_TO_DAC1R_POSITION             4
int32_t   wm8994_aif1_dac1_rmradcl_to_dac1r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC1_RMR
* Address       : 0X602
* Bit Group Name: AIF1ADCR_TO_DAC1R
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC1_RMRADCR_TO_DAC1R_MASK                 (uint16_t)0x0020
#define   WM8994_AIF1_DAC1_RMRADCR_TO_DAC1R_POSITION             5
int32_t   wm8994_aif1_dac1_rmradcr_to_dac1r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC2_LMR
* Address       : 0X604
* Bit Group Name: AIF1DAC1L_TO_DAC2L
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_LMRDAC1L_TO_DAC2L_MASK                 (uint16_t)0x0001
#define   WM8994_AIF1_DAC2_LMRDAC1L_TO_DAC2L_POSITION             0
int32_t   wm8994_aif1_dac2_lmrdac1l_to_dac2l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC2_LMR
* Address       : 0X604
* Bit Group Name: AIF1DAC2L_TO_DAC2L
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_LMRDAC2L_TO_DAC2L_MASK                 (uint16_t)0x0002
#define   WM8994_AIF1_DAC2_LMRDAC2L_TO_DAC2L_POSITION             1
int32_t   wm8994_aif1_dac2_lmrdac2l_to_dac2l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC2_LMR
* Address       : 0X604
* Bit Group Name: AIF1DACL_TO_DAC2L
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_LMRDACL_TO_DAC2L_MASK                 (uint16_t)0x0004
#define   WM8994_AIF1_DAC2_LMRDACL_TO_DAC2L_POSITION             2
int32_t   wm8994_aif1_dac2_lmrdacl_to_dac2l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC2_LMR
* Address       : 0X604
* Bit Group Name: AIF1ADCL_TO_DAC2L
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_LMRADCL_TO_DAC2L_MASK                 (uint16_t)0x0010
#define   WM8994_AIF1_DAC2_LMRADCL_TO_DAC2L_POSITION             4
int32_t   wm8994_aif1_dac2_lmradcl_to_dac2l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC2_LMR
* Address       : 0X604
* Bit Group Name: AIF1ADCR_TO_DAC2L
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_LMRADCR_TO_DAC2L_MASK                 (uint16_t)0x0020
#define   WM8994_AIF1_DAC2_LMRADCR_TO_DAC2L_POSITION             5
int32_t   wm8994_aif1_dac2_lmradcr_to_dac2l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC2_RMR
* Address       : 0X605
* Bit Group Name: AIF1DAC1R_TO_DAC2R
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_RMRDAC1R_TO_DAC2R_MASK                 (uint16_t)0x0001
#define   WM8994_AIF1_DAC2_RMRDAC1R_TO_DAC2R_POSITION             0
int32_t   wm8994_aif1_dac2_rmrdac1r_to_dac2r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC2_RMR
* Address       : 0X605
* Bit Group Name: AIF1DAC2R_TO_DAC2R
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_RMRDAC2R_TO_DAC2R_MASK                 (uint16_t)0x0002
#define   WM8994_AIF1_DAC2_RMRDAC2R_TO_DAC2R_POSITION             1
int32_t   wm8994_aif1_dac2_rmrdac2r_to_dac2r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC2_RMR
* Address       : 0X605
* Bit Group Name: AIF1DACR_TO_DAC2R
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_RMRDACR_TO_DAC2R_MASK                 (uint16_t)0x0004
#define   WM8994_AIF1_DAC2_RMRDACR_TO_DAC2R_POSITION             2
int32_t   wm8994_aif1_dac2_rmrdacr_to_dac2r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC2_RMR
* Address       : 0X605
* Bit Group Name: AIF1ADCL_TO_DAC2R
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_RMRADCL_TO_DAC2R_MASK                 (uint16_t)0x0010
#define   WM8994_AIF1_DAC2_RMRADCL_TO_DAC2R_POSITION             4
int32_t   wm8994_aif1_dac2_rmradcl_to_dac2r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_DAC2_RMR
* Address       : 0X605
* Bit Group Name: AIF1ADCR_TO_DAC2R
* Permission    : RW
*******************************************************************************/
#define   WM8994_AIF1_DAC2_RMRADCR_TO_DAC2R_MASK                 (uint16_t)0x0020
#define   WM8994_AIF1_DAC2_RMRADCR_TO_DAC2R_POSITION             5
int32_t   wm8994_aif1_dac2_rmradcr_to_dac2r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_LMR
* Address       : 0X606
* Bit Group Name: AIF2DACL_TO_AIF1ADC1L
* Permission    : RW
*******************************************************************************/
#define   WM8994_ADC1LMR_AIF2DACL_TO_AIF1ADC1L_MASK                 (uint16_t)0x0001
#define   WM8994_ADC1LMR_AIF2DACL_TO_AIF1ADC1L_POSITION             0
int32_t   wm8994_adc1lmr_aif2dacl_to_aif1adc1l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_LMR
* Address       : 0X606
* Bit Group Name: ADC1L_TO_AIF1ADC1L
* Permission    : RW
*******************************************************************************/
#define   WM8994_ADC1LMR_ADC1L_TO_AIF1ADC1L_MASK                 (uint16_t)0x0002
#define   WM8994_ADC1LMR_ADC1L_TO_AIF1ADC1L_POSITION             1
int32_t   wm8994_adc1lmr_adc1l_to_aif1adc1l(wm8994_ctx_t *ctx, uint16_t value);


/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_RMR
* Address       : 0X607
* Bit Group Name: AIF2DACL_TO_AIF1ADC1R
* Permission    : RW
*******************************************************************************/
#define   WM8994_ADC1RMR_AIF2DACL_TO_AIF1ADC1R_MASK                 (uint16_t)0x0001
#define   WM8994_ADC1RMR_AIF2DACL_TO_AIF1ADC1R_POSITION             0
int32_t   wm8994_adc1rmr_aif2dacl_to_aif1adc1r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC1_RMR
* Address       : 0X607
* Bit Group Name: ADC1R_TO_AIF1ADC1R
* Permission    : RW
*******************************************************************************/
#define   WM8994_ADC1RMR_ADC1R_TO_AIF1ADC1R_MASK                 (uint16_t)0x0002
#define   WM8994_ADC1RMR_ADC1R_TO_AIF1ADC1R_POSITION             1
int32_t   wm8994_adc1rmr_adc1r_to_aif1adc1r(wm8994_ctx_t *ctx, uint16_t value);
         
/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_LMR
* Address       : 0X608
* Bit Group Name: AIF2DACL_TO_AIF1ADC2L
* Permission    : RW
*******************************************************************************/
#define   WM8994_ADC2LMR_AIF2DACL_TO_AIF1ADC2L_MASK                 (uint16_t)0x0001
#define   WM8994_ADC2LMR_AIF2DACL_TO_AIF1ADC2L_POSITION             0
int32_t   wm8994_adc2lmr_aif2dacl_to_aif1adc2l(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_LMR
* Address       : 0X608
* Bit Group Name: ADC2L_TO_AIF1ADC2L
* Permission    : RW
*******************************************************************************/
#define   WM8994_ADC2LMR_ADC2L_TO_AIF1ADC2L_MASK                 (uint16_t)0x0002
#define   WM8994_ADC2LMR_ADC2L_TO_AIF1ADC2L_POSITION             1
int32_t   wm8994_adc2lmr_adc2l_to_aif1adc2l(wm8994_ctx_t *ctx, uint16_t value);


/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_RMR
* Address       : 0X609
* Bit Group Name: AIF2DACL_TO_AIF1ADC2R
* Permission    : RW
*******************************************************************************/
#define   WM8994_ADC2RMR_AIF2DACL_TO_AIF1ADC2R_MASK                 (uint16_t)0x0001
#define   WM8994_ADC2RMR_AIF2DACL_TO_AIF1ADC2R_POSITION             0
int32_t   wm8994_adc2rmr_aif2dacl_to_aif1adc2r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_AIF1_ADC2_RMR
* Address       : 0X609
* Bit Group Name: ADC2R_TO_AIF1ADC2R
* Permission    : RW
*******************************************************************************/
#define   WM8994_ADC2RMR_ADC2R_TO_AIF1ADC2R_MASK                 (uint16_t)0x0002
#define   WM8994_ADC2RMR_ADC2R_TO_AIF1ADC2R_POSITION             1
int32_t   wm8994_adc2rmr_adc2r_to_aif1adc2r(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC1_LEFT_VOL
* Address       : 0X610
* Bit Group Name: DAC1L_VOL[7:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC1_LEFT_VOL_VSET_MASK                 (uint16_t)0x00FF
#define   WM8994_DAC1_LEFT_VOL_VSET_POSITION             0
int32_t   wm8994_dac1_left_vol_vset(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC1_LEFT_VOL
* Address       : 0X610
* Bit Group Name: DAC1L_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC1_LEFT_VOL_VU_MASK                 (uint16_t)0x0100
#define   WM8994_DAC1_LEFT_VOL_VU_POSITION             8
int32_t   wm8994_dac1_left_vol_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC1_LEFT_VOL
* Address       : 0X610
* Bit Group Name: DAC1L_MUTE
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC1_LEFT_VOL_MUTE_MASK                 (uint16_t)0x0200
#define   WM8994_DAC1_LEFT_VOL_MUTE_POSITION             9
int32_t   wm8994_dac1_left_vol_mute(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC1_RIGHT_VOL
* Address       : 0X611
* Bit Group Name: DAC1R_VOL[7:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC1_RIGHT_VOL_VSET_MASK                 (uint16_t)0x00FF
#define   WM8994_DAC1_RIGHT_VOL_VSET_POSITION             0
int32_t   wm8994_dac1_right_vol_vset(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC1_RIGHT_VOL
* Address       : 0X611
* Bit Group Name: DAC1R_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC1_RIGHT_VOL_VU_MASK                 (uint16_t)0x0100
#define   WM8994_DAC1_RIGHT_VOL_VU_POSITION             8
int32_t   wm8994_dac1_right_vol_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC1_RIGHT_VOL
* Address       : 0X611
* Bit Group Name: DAC1R_MUTE
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC1_RIGHT_VOL_MUTE_MASK                 (uint16_t)0x0200
#define   WM8994_DAC1_RIGHT_VOL_MUTE_POSITION             9
int32_t   wm8994_dac1_right_vol_mute(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC2_LEFT_VOL
* Address       : 0X612
* Bit Group Name: DAC2L_VOL[7:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC2_LEFT_VOL_VSET_MASK                 (uint16_t)0x00FF
#define   WM8994_DAC2_LEFT_VOL_VSET_POSITION             0
int32_t   wm8994_dac2_left_vol_vset(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC2_LEFT_VOL
* Address       : 0X612
* Bit Group Name: DAC2L_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC2_LEFT_VOL_VU_MASK                 (uint16_t)0x0100
#define   WM8994_DAC2_LEFT_VOL_VU_POSITION             8
int32_t   wm8994_dac2_left_vol_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC2_LEFT_VOL
* Address       : 0X612
* Bit Group Name: DAC2L_MUTE
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC2_LEFT_VOL_MUTE_MASK                 (uint16_t)0x0200
#define   WM8994_DAC2_LEFT_VOL_MUTE_POSITION             9
int32_t   wm8994_dac2_left_vol_mute(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC2_RIGHT_VOL
* Address       : 0X613
* Bit Group Name: DAC2R_VOL[7:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC2_RIGHT_VOL_VSET_MASK                 (uint16_t)0x00FF
#define   WM8994_DAC2_RIGHT_VOL_VSET_POSITION             0
int32_t   wm8994_dac2_right_vol_vset(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC2_RIGHT_VOL
* Address       : 0X613
* Bit Group Name: DAC2R_VU
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC2_RIGHT_VOL_VU_MASK                 (uint16_t)0x0100
#define   WM8994_DAC2_RIGHT_VOL_VU_POSITION             8
int32_t   wm8994_dac2_right_vol_vu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_DAC2_RIGHT_VOL
* Address       : 0X613
* Bit Group Name: DAC2R_MUTE
* Permission    : RW
*******************************************************************************/
#define   WM8994_DAC2_RIGHT_VOL_MUTE_MASK                 (uint16_t)0x0200
#define   WM8994_DAC2_RIGHT_VOL_MUTE_POSITION             9
int32_t   wm8994_dac2_right_vol_mute(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OVERSAMPLING
* Address       : 0X620
* Bit Group Name: DAC_OSR128
* Permission    : RW
*******************************************************************************/
#define   WM8994_OVERSAMPLING_DAC_OSR128_MASK                 (uint16_t)0x0001
#define   WM8994_OVERSAMPLING_DAC_OSR128_POSITION             0
int32_t   wm8994_oversampling_dac_osr128(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_OVERSAMPLING
* Address       : 0X620
* Bit Group Name: ADC_OSR128
* Permission    : RW
*******************************************************************************/
#define   WM8994_OVERSAMPLING_ADC_OSR128_MASK                 (uint16_t)0x0002
#define   WM8994_OVERSAMPLING_ADC_OSR128_POSITION             1
int32_t   wm8994_oversampling_adc_osr128(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_GPIO1
* Address       : 0X700
* Bit Group Name: GP1_FN [4:0]
* Permission    : RW
*******************************************************************************/
#define   WM8994_GPIO1_GP1_FN_MASK                 (uint16_t)0x001F
#define   WM8994_GPIO1_GP1_FN_POSITION             0
int32_t   wm8994_gpio1_gp1_fn(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_GPIO1
* Address       : 0X700
* Bit Group Name: GP1_LVL
* Permission    : RW
*******************************************************************************/
#define   WM8994_GPIO1_GP1_LVL_MASK                 (uint16_t)0x0040
#define   WM8994_GPIO1_GP1_LVL_POSITION             6
int32_t   wm8994_gpio1_gp1_lvl(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_GPIO1
* Address       : 0X700
* Bit Group Name: GP1_DB
* Permission    : RW
*******************************************************************************/
#define   WM8994_GPIO1_GP1_DB_MASK                 (uint16_t)0x0100
#define   WM8994_GPIO1_GP1_DB_POSITION             8
int32_t   wm8994_gpio1_gp1_db(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_GPIO1
* Address       : 0X700
* Bit Group Name: GP1_OP_CFG
* Permission    : RW
*******************************************************************************/
#define   WM8994_GPIO1_GP1_OP_CFG_MASK                 (uint16_t)0x0200
#define   WM8994_GPIO1_GP1_OP_CFG_POSITION             9
int32_t   wm8994_gpio1_gp1_op_cfg(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_GPIO1
* Address       : 0X700
* Bit Group Name: GP1_POL
* Permission    : RW
*******************************************************************************/
#define   WM8994_GPIO1_GP1_POL_MASK                 (uint16_t)0x0400
#define   WM8994_GPIO1_GP1_POL_POSITION             10
int32_t   wm8994_gpio1_gp1_pol(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_GPIO1
* Address       : 0X700
* Bit Group Name: GP1_PD
* Permission    : RW
*******************************************************************************/
#define   WM8994_GPIO1_GP1_PD_MASK                 (uint16_t)0x2000
#define   WM8994_GPIO1_GP1_PD_POSITION             13
int32_t   wm8994_gpio1_gp1_pd(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_GPIO1
* Address       : 0X700
* Bit Group Name: GP1_PU
* Permission    : RW
*******************************************************************************/
#define   WM8994_GPIO1_GP1_PU_MASK                 (uint16_t)0x4000
#define   WM8994_GPIO1_GP1_PU_POSITION             14
int32_t   wm8994_gpio1_gp1_pu(wm8994_ctx_t *ctx, uint16_t value);

/*******************************************************************************
* Register      : WM8994_GPIO1
* Address       : 0X700
* Bit Group Name: GP1_DIR
* Permission    : RW
*******************************************************************************/
#define   WM8994_GPIO1_GP1_DIR_MASK                 (uint16_t)0x8000
#define   WM8994_GPIO1_GP1_DIR_POSITION             15
int32_t   wm8994_gpio1_gp1_dir(wm8994_ctx_t *ctx, uint16_t value);

#ifdef __cplusplus
}
#endif

#endif /* WM8994_REG_H */

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
