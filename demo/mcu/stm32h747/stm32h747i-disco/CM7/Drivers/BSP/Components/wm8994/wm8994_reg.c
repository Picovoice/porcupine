/**
  ******************************************************************************
  * @file    wm8994_reg.c
  * @author  MCD Application Team
  * @brief   This file provides unitary register function to control the WM8994 
  *          Audio Codec driver.   
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

/* Includes ------------------------------------------------------------------*/
#include "wm8994_reg.h"

/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup Components
  * @{
  */ 

/** @addtogroup wm8994
  * @brief     This file provides a set of functions needed to drive the 
  *            WM8994 audio codec.
  * @{
  */

/************** Generic Function  *******************/
/*******************************************************************************
* Function Name : wm8994_read_reg
* Description   : Generic Reading function. It must be full-filled with either
*                 I2C or SPI reading functions
* Input         : Register Address, length of buffer
* Output        : data Read
*******************************************************************************/
int32_t wm8994_read_reg(wm8994_ctx_t *ctx, uint16_t reg, uint16_t* data, uint16_t length)
{
  int32_t ret;
  uint16_t tmp;
  
  ret = ctx->ReadReg(ctx->handle, reg, (uint8_t *)data, length);
  
  if(ret >= 0)
  {
    tmp = ((uint16_t)(*data >> 8) & 0x00FF);
    tmp |= ((uint16_t)(*data << 8) & 0xFF00);
    *data = tmp;
  }
  return ret;
}

/*******************************************************************************
* Function Name : wm8994_write_reg
* Description   : Generic Writing function. It must be full-filled with either
*                 I2C or SPI writing function
* Input         : Register Address, data to be written, length of buffer
* Output        : None
*******************************************************************************/
int32_t wm8994_write_reg(wm8994_ctx_t *ctx, uint16_t reg, uint16_t *data, uint16_t length)
{
  uint16_t tmp;
  tmp = ((uint16_t)(*data >> 8) & 0x00FF);
  tmp |= ((uint16_t)(*data << 8) & 0xFF00);
  
  return ctx->WriteReg(ctx->handle, reg, (uint8_t *)&tmp, length);
}

/**************** Base Function  *******************/
/*******************************************************************************
* Function Name  : wm8994_register_set
* Description    : Write value to a WM8994 register
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_register_set(wm8994_ctx_t *ctx, uint16_t reg, uint16_t value)
{
  return wm8994_write_reg(ctx, reg, &value, 2);
}


/*******************************************************************************
* Function Name  : wm8994_sw_reset_w
* Description    : Write Device Mode
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_sw_reset_w(wm8994_ctx_t *ctx, uint16_t value)
{
  return wm8994_write_reg(ctx, WM8994_SW_RESET, &value, 2);
}

/*******************************************************************************
* Function Name  : wm8994_sw_reset_r
* Description    : Read Device Mode
* Input          : Pointer to uint8_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_sw_reset_r(wm8994_ctx_t *ctx, uint16_t *value)
{
  return wm8994_read_reg(ctx, WM8994_SW_RESET, value, 2);
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_1_bias_en
* Description    : Bias enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_1_bias_en(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;  
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_1_BIAS_EN_MASK;
    tmp |= value << WM8994_PWR_MGMT_1_BIAS_EN_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_1_vmid_sel
* Description    : VMID Divider Enable and Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_1_vmid_sel(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_1_VMID_SEL_MASK;
    tmp |= value << WM8994_PWR_MGMT_1_VMID_SEL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_1_micb1_ena
* Description    : Microphone Bias 1 Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_1_micb1_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_1_MICB1_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_1_MICB1_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_1_micb2_ena
* Description    : Microphone Bias 2 Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_1_micb2_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_1_MICB2_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_1_MICB2_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_1_hpout1r_ena
* Description    : Enables HPOUT1R input stage
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_1_hpout1r_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_1_HPOUT1R_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_1_HPOUT1R_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_1_hpout1l_ena
* Description    : Enables HPOUT1L input stage
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_1_hpout1l_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;

  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_1_HPOUT1L_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_1_HPOUT1L_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_1_hpout2_ena
* Description    : HPOUT2 Output Stage Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_1_hpout2_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_1_HPOUT2_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_1_HPOUT2_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_1_spkoutl_ena
* Description    : SPKMIXL Mixer, SPKLVOL PGA and SPKOUTL Output Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_1_spkoutl_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_1_SPKOUTL_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_1_SPKOUTL_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_1_spkoutr_ena
* Description    : SPKMIXR Mixer, SPKRVOL PGA and SPKOUTR Output Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_1_spkoutr_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_1_SPKOUTR_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_1_SPKOUTR_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_2_in1r_ena
* Description    : IN1R Input PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_2_in1r_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_2_IN1R_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_2_IN1R_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_2_in2r_ena
* Description    : IN2R Input PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_2_in2r_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_2_IN2R_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_2_IN2R_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_2_in1l_ena
* Description    : IN1L Input PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_2_in1l_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_2_IN1L_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_2_IN1L_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_2_in2l_ena
* Description    : IN2L Input PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_2_in2l_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_2_IN2L_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_2_IN2L_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_2_mixinr_ena
* Description    : Right Input Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_2_mixinr_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_2_MIXINR_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_2_MIXINR_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_2_mixinl_ena
* Description    : Left Input Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_2_mixinl_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_2_MIXINL_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_2_MIXINL_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_2_opclk_ena
* Description    : GPIO Clock Output (OPCLK) Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_2_opclk_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_2_OPCLK_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_2_OPCLK_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_2_tshut_opdis
* Description    : Thermal shutdown control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_2_tshut_opdis(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_2_TSHUT_OPDIS_MASK;
    tmp |= value << WM8994_PWR_MGMT_2_TSHUT_OPDIS_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_2_tshut_ena
* Description    : Thermal sensor enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_2_tshut_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_2_TSHUT_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_2_TSHUT_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_3_mixoutr_ena
* Description    : MIXOUTR Right Output Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_3_mixoutr_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_3_MIXOUTR_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_3_MIXOUTR_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_3_mixoutl_ena
* Description    : MIXOUTL Left Output Mixer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_3_mixoutl_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_3_MIXOUTL_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_3_MIXOUTL_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_3_mixoutrvol_ena
* Description    : MIXOUTL Right Volume Control Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_3_mixoutrvol_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_3_MIXOUTRVOL_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_3_MIXOUTRVOL_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_3_mixoutlvol_ena
* Description    : MIXOUTL Left Volume Control Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_3_mixoutlvol_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_3_MIXOUTLVOL_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_3_MIXOUTLVOL_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_3_spklvol_ena
* Description    : SPKMIXL Mixer and SPKRVOL PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_3_spklvol_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_3_SPKLVOL_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_3_SPKLVOL_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_3_spkrvol_ena
* Description    : SPKMIXR Mixer and SPKRVOL PGA Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_3_spkrvol_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_3_SPKRVOL_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_3_SPKRVOL_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_3_lineout2p_ena
* Description    : LINEOUT2P Line Out and LINEOUT2PMIX Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_3_lineout2p_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_3_LINEOUT2P_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_3_LINEOUT2P_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_3_lineout2n_ena
* Description    : LINEOUT2N Line Out and LINEOUT2NMIX Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_3_lineout2n_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_3_LINEOUT2N_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_3_LINEOUT2N_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_3_lineout1p_ena
* Description    : LINEOUT1P Line Out and LINEOUT1PMIX Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_3_lineout1p_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_3_LINEOUT1P_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_3_LINEOUT1P_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_3_lineout1n_ena
* Description    : LINEOUT1N Line Out and LINEOUT1NMIX Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_3_lineout1n_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_3_LINEOUT1N_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_3_LINEOUT1N_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_adcr_ena
* Description    : Right ADC Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_adcr_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_ADCR_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_ADCR_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_adcl_ena
* Description    : Left ADC Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_adcl_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_ADCL_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_ADCL_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_dmic1r_ena
* Description    : Digital microphone DMICDAT1 Right channel enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_dmic1r_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_DMIC1R_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_DMIC1R_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_dmic1l_ena
* Description    : Digital microphone DMICDAT1 Left channel enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_dmic1l_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_DMIC1L_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_DMIC1L_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_dmic2r_ena
* Description    : Digital microphone DMICDAT2 Right channel enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_dmic2r_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_DMIC2R_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_DMIC2R_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_dmic2l_ena
* Description    : Digital microphone DMICDAT2 Left channel enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_dmic2l_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_DMIC2L_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_DMIC2L_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_aif1adc1r_ena
* Description    : Enable AIF1ADC1 (Right) output path (AIF1, Timeslot0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_aif1adc1r_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_AIF1ADC1R_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_AIF1ADC1R_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_aif1adc1l_ena
* Description    : Enable AIF1ADC1 (Left) output path (AIF1, Timeslot 0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_aif1adc1l_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_AIF1ADC1L_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_AIF1ADC1L_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_aif1adc2r_ena
* Description    : Enable AIF1ADC2 (Right) output path (AIF1, Timeslot0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_aif1adc2r_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_AIF1ADC2R_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_AIF1ADC2R_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_aif1adc2l_ena
* Description    : Enable AIF1ADC2 (Left) output path (AIF1, Timeslot 0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_aif1adc2l_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_AIF1ADC2L_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_AIF1ADC2L_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_aif2adcr_ena
* Description    : Enable AIF2ADC (Right) output path
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_aif2adcr_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_AIF2ADCR_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_AIF2ADCR_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_4_aif2adcl_ena
* Description    : Enable AIF2ADC (Left) output path
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_4_aif2adcl_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_4_AIF2ADCL_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_4_AIF2ADCL_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_5_dac1r_ena
* Description    : Right DAC1 Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_5_dac1r_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_5_DAC1R_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_5_DAC1R_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_5_dac1l_ena
* Description    : Left DAC1 Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_5_dac1l_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_5_DAC1L_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_5_DAC1L_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
  }
  
  return ret;
}


/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_5_dac2r_ena
* Description    : Right DAC2 enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_5_dac2r_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_5_DAC2R_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_5_DAC2R_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_5_dac2l_ena
* Description    : Left DAC2 enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_5_dac2l_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_5_DAC2L_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_5_DAC2L_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_5_aif1dac1r_ena
* Description    : Enable AIF1DAC1 (Right) input path (AIF1, Timeslot0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_5_aif1dac1r_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_5_AIF1DAC1R_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_5_AIF1DAC1R_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_5_aif1dac1l_ena
* Description    : Enable AIF1DAC1 (Left) input path (AIF1, Timeslot 0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_5_aif1dac1l_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_5_AIF1DAC1L_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_5_AIF1DAC1L_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_5_aif1dac2r_ena
* Description    : Enable AIF1DAC2 (Right) input path (AIF1, Timeslot0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_5_aif1dac2r_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_5_AIF1DAC2R_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_5_AIF1DAC2R_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_5_aif1dac2l_ena
* Description    : Enable AIF1DAC2 (Left) input path (AIF1, Timeslot 0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_5_aif1dac2l_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_5_AIF1DAC2L_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_5_AIF1DAC2L_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_5_aif2dacr_ena
* Description    : Enable AIF2DAC (Right) input path
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_5_aif2dacr_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_5_AIF2DACR_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_5_AIF2DACR_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_5_aif2dacl_ena
* Description    : Enable AIF2DAC (Left) input path
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_5_aif2dacl_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_5_AIF2DACL_ENA_MASK;
    tmp |= value << WM8994_PWR_MGMT_5_AIF2DACL_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_5, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_6_aif1_dacdat_src
* Description    : AIF1 DACDAT Source select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_6_aif1_dacdat_src(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_6, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_6_AIF1_DACDAT_SRC_MASK;
    tmp |= value << WM8994_PWR_MGMT_6_AIF1_DACDAT_SRC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_6, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_6_aif2_dacdat_src
* Description    : AIF2 DACDAT Source select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_6_aif2_dacdat_src(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_6, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_6_AIF2_DACDAT_SRC_MASK;
    tmp |= value << WM8994_PWR_MGMT_6_AIF2_DACDAT_SRC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_6, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_6_aif2_adcdat_src
* Description    : GPIO7/ADCDAT2 Source select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_6_aif2_adcdat_src(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_6, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_6_AIF2_ADCDAT_SRC_MASK;
    tmp |= value << WM8994_PWR_MGMT_6_AIF2_ADCDAT_SRC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_6, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_6_aif3_adcdat_src
* Description    : GPIO9/ADCDAT3 Source select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_6_aif3_adcdat_src(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_6, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_6_AIF3_ADCDAT_SRC_MASK;
    tmp |= value << WM8994_PWR_MGMT_6_AIF3_ADCDAT_SRC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_6, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_pwr_mgmt_6_aif3_tri
* Description    : GPIO9/ADCDAT3 Source select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_pwr_mgmt_6_aif3_tri(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_PWR_MANAGEMENT_6, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_PWR_MGMT_6_AIF3_TRI_MASK;
    tmp |= value << WM8994_PWR_MGMT_6_AIF3_TRI_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_PWR_MANAGEMENT_6, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer1_inputs_clamp
* Description    : Input pad VMID clamp
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer1_inputs_clamp(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER1_INPUTS_CLAMP_MASK;
    tmp |= value << WM8994_INMIXER1_INPUTS_CLAMP_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer1_in1lp_mixinl_boost
* Description    : IN1LP Pin (PGA Bypass) to MIXINL Gain Boost.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer1_in1lp_mixinl_boost(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER1_IN1LP_MIXINL_BOOST_MASK;
    tmp |= value << WM8994_INMIXER1_IN1LP_MIXINL_BOOST_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer1_in1rp_mixinr_boost
* Description    : IN1RP Pin (PGA Bypass) to MIXINR Gain Boost.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer1_in1rp_mixinr_boost(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER1_IN1RP_MIXINR_BOOST_MASK;
    tmp |= value << WM8994_INMIXER1_IN1RP_MIXINR_BOOST_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lli_in1l_vol
* Description    : IN1L volume (LLI: Left Line Input 1&2)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lli_in1l_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_LINE_IN12_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LLI_IN1L_VOL_MASK;
    tmp |= value << WM8994_LLI_IN1L_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_LINE_IN12_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lli_in1l_zc
* Description    : IN1L PGA Zero Cross Detector
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lli_in1l_zc(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_LINE_IN12_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LLI_IN1L_ZC_MASK;
    tmp |= value << WM8994_LLI_IN1L_ZC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_LINE_IN12_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lli_in1l_mute
* Description    : IN1L PGA Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lli_in1l_mute(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_LINE_IN12_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LLI_IN1L_MUTE_MASK;
    tmp |= value << WM8994_LLI_IN1L_MUTE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_LINE_IN12_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lli_in1_vu
* Description    : IN1L PGA Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lli_in1_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_LINE_IN12_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LLI_IN1_VU_MASK;
    tmp |= value << WM8994_LLI_IN1_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_LINE_IN12_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lli_in2l_vol
* Description    : IN2L volume (LLI: Left Line Input 3&4)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lli_in2l_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_LINE_IN34_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LLI_IN2L_VOL_MASK;
    tmp |= value << WM8994_LLI_IN2L_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_LINE_IN34_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lli_in2l_zc
* Description    : IN2L PGA Zero Cross Detector
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lli_in2l_zc(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_LINE_IN34_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LLI_IN2L_ZC_MASK;
    tmp |= value << WM8994_LLI_IN2L_ZC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_LINE_IN34_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lli_in2l_mute
* Description    : IN2L PGA Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lli_in2l_mute(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_LINE_IN34_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LLI_IN2L_MUTE_MASK;
    tmp |= value << WM8994_LLI_IN2L_MUTE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_LINE_IN34_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lli_in2_vu
* Description    : IN2L PGA Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lli_in2_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_LINE_IN34_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LLI_IN2_VU_MASK;
    tmp |= value << WM8994_LLI_IN2_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_LINE_IN34_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_rli_in1r_vol
* Description    : IN1R volume (RLI: Right Line Input 1&2)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_rli_in1r_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_LINE_IN12_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RLI_IN1R_VOL_MASK;
    tmp |= value << WM8994_RLI_IN1R_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_LINE_IN12_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_rli_in1r_zc
* Description    : IN1R PGA Zero Cross Detector
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_rli_in1r_zc(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_LINE_IN12_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RLI_IN1R_ZC_MASK;
    tmp |= value << WM8994_RLI_IN1R_ZC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_LINE_IN12_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_rli_in1r_mute
* Description    : IN1R PGA Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_rli_in1r_mute(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_LINE_IN12_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RLI_IN1R_MUTE_MASK;
    tmp |= value << WM8994_RLI_IN1R_MUTE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_LINE_IN12_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_rli_in1_vu
* Description    : IN1R PGA Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_rli_in1_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_LINE_IN12_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RLI_IN1_VU_MASK;
    tmp |= value << WM8994_RLI_IN1_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_LINE_IN12_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_rli_in2r_vol
* Description    : IN2R volume (RLI: Right Line Input 3&4)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_rli_in2r_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_LINE_IN34_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RLI_IN2R_VOL_MASK;
    tmp |= value << WM8994_RLI_IN2R_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_LINE_IN34_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_rli_in2r_zc
* Description    : IN2R PGA Zero Cross Detector
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_rli_in2r_zc(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_LINE_IN34_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RLI_IN2R_ZC_MASK;
    tmp |= value << WM8994_RLI_IN2R_ZC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_LINE_IN34_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_rli_in2r_mute
* Description    : IN2R PGA Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_rli_in2r_mute(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_LINE_IN34_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RLI_IN2R_MUTE_MASK;
    tmp |= value << WM8994_RLI_IN2R_MUTE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_LINE_IN34_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_rli_in2_vu
* Description    : Input PGA Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_rli_in2_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_LINE_IN34_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RLI_IN2_VU_MASK;
    tmp |= value << WM8994_RLI_IN2_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_LINE_IN34_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lo_hpout1l_vol (LO: Left Output)
* Description    : HPOUT1L VOL (Left Headphone Output PGA) Volume
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lo_hpout1l_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_OUTPUT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LO_HPOUT1L_VOL_MASK;
    tmp |= value << WM8994_LO_HPOUT1L_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_OUTPUT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lo_hpout1l_vol_r
* Description    : Read HPOUT1L VOL (Left Headphone Output PGA) Volume
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lo_hpout1l_vol_r(wm8994_ctx_t *ctx, uint16_t *value)
{
  int32_t ret;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_OUTPUT_VOL, value, 2);

  if(ret == 0)
  {  
  *value &= WM8994_LO_HPOUT1L_VOL_MASK;
  *value = *value >> WM8994_LO_HPOUT1L_VOL_POSITION;
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lo_hpout1l_mute_n
* Description    : HPOUT1L VOL (Left Headphone Output PGA) Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lo_hpout1l_mute_n(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_OUTPUT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LO_HPOUT1L_MUTE_N_MASK;
    tmp |= value << WM8994_LO_HPOUT1L_MUTE_N_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_OUTPUT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lo_hpout1l_zc
* Description    : HPOUT1L VOL (Left Headphone Output PGA) Zero
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lo_hpout1l_zc(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_OUTPUT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LO_HPOUT1L_ZC_MASK;
    tmp |= value << WM8994_LO_HPOUT1L_ZC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_OUTPUT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_lo_hpout1l_vu
* Description    : Headphone Output PGA Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_lo_hpout1l_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_LEFT_OUTPUT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_LO_HPOUT1L_VU_MASK;
    tmp |= value << WM8994_LO_HPOUT1L_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_LEFT_OUTPUT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_ro_hpout1r_vol (RO: Right Output)
* Description    : HPOUT1R VOL (Right Headphone Output PGA) Volume
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_ro_hpout1r_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_OUTPUT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RO_HPOUT1R_VOL_MASK;
    tmp |= value << WM8994_RO_HPOUT1R_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_OUTPUT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_ro_hpout1r_mute_n
* Description    : HPOUT1RVOL (Right Headphone Output PGA) Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_ro_hpout1r_mute_n(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_OUTPUT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RO_HPOUT1R_MUTE_N_MASK;
    tmp |= value << WM8994_RO_HPOUT1R_MUTE_N_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_OUTPUT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_ro_hpout1r_zc
* Description    : HPOUT1RVOL (Right Headphone Output PGA) Zero
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_ro_hpout1r_zc(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_OUTPUT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RO_HPOUT1R_ZC_MASK;
    tmp |= value << WM8994_RO_HPOUT1R_ZC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_OUTPUT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_ro_hpout1r_vu
* Description    : Headphone Output PGA Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_ro_hpout1r_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_RIGHT_OUTPUT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_RO_HPOUT1R_VU_MASK;
    tmp |= value << WM8994_RO_HPOUT1R_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_RIGHT_OUTPUT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixl_att_vol
* Description    : Left Speaker Mixer Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixl_att_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXL_ATT_VOL_MASK;
    tmp |= value << WM8994_SPKMIXL_ATT_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixl_att_dac1_vol
* Description    : Left DAC1 to SPKMIXL Fine Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixl_att_dac1_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXL_ATT_DAC1_VOL_MASK;
    tmp |= value << WM8994_SPKMIXL_ATT_DAC1_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixl_att_mixoutl_vol
* Description    : Left Mixer Output to SPKMIXL Fine Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixl_att_mixoutl_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXL_ATT_MIXOUTL_VOL_MASK;
    tmp |= value << WM8994_SPKMIXL_ATT_MIXOUTL_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixl_att_in1lp_vol
* Description    : IN1LP to SPKMIXL Fine Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixl_att_in1lp_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXL_ATT_IN1LP_VOL_MASK;
    tmp |= value << WM8994_SPKMIXL_ATT_IN1LP_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixl_att_mixinl_vol
* Description    : MIXINL (Left ADC bypass) to SPKMIXL Fine Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixl_att_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXL_ATT_MIXINL_VOL_MASK;
    tmp |= value << WM8994_SPKMIXL_ATT_MIXINL_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixl_att_dac2l_vol
* Description    : Left DAC2 to SPKMIXL Fine Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixl_att_dac2l_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXL_ATT_DAC2L_VOL_MASK;
    tmp |= value << WM8994_SPKMIXL_ATT_DAC2L_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixl_att_spkab_refsel
* Description    : Selects Reference for Speaker in Class AB mode
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixl_att_spkab_refsel(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXL_ATT_SPKAB_REFSEL_MASK;
    tmp |= value << WM8994_SPKMIXL_ATT_SPKAB_REFSEL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXL_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixr_att_vol
* Description    : Right Speaker Mixer Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixr_att_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXR_ATT_VOL_MASK;
    tmp |= value << WM8994_SPKMIXR_ATT_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixr_att_dac1_vol
* Description    : Right DAC1 to SPKMIXR Fine Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixr_att_dac1_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXR_ATT_DAC1_VOL_MASK;
    tmp |= value << WM8994_SPKMIXR_ATT_DAC1_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixr_att_mixoutl_vol
* Description    : Right Mixer Output to SPKMIXR Fine Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixr_att_mixoutl_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXR_ATT_MIXOUTL_VOL_MASK;
    tmp |= value << WM8994_SPKMIXR_ATT_MIXOUTL_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixr_att_in1rp_vol
* Description    : IN1RP to SPKMIXR Fine Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixr_att_in1rp_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXR_ATT_IN1RP_VOL_MASK;
    tmp |= value << WM8994_SPKMIXR_ATT_IN1RP_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixr_att_mixinl_vol
* Description    : MIXINL (Right ADC bypass) to SPKMIXR Fine Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixr_att_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXR_ATT_MIXINL_VOL_MASK;
    tmp |= value << WM8994_SPKMIXR_ATT_MIXINL_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixr_att_dac2r_vol
* Description    : Right DAC2 to SPKMIXR Fine Volume Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixr_att_dac2r_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXR_ATT_DAC2R_VOL_MASK;
    tmp |= value << WM8994_SPKMIXR_ATT_DAC2R_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixr_att_spkout_classab
* Description    : Speaker Class AB Mode Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixr_att_spkout_classab(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXR_ATT_SPKOUT_CLASSAB_MASK;
    tmp |= value << WM8994_SPKMIXR_ATT_SPKOUT_CLASSAB_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPKMIXR_ATT, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spk_left_vol_spkout_vol
* Description    : SPKLVOL (Left Speaker Output PGA) Volume
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spk_left_vol_spkout_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPK_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPK_LEFT_VOL_SPKOUT_VOL_MASK;
    tmp |= value << WM8994_SPK_LEFT_VOL_SPKOUT_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPK_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spk_left_vol_spkout_mute_n
* Description    : SPKLVOL (Left Speaker Output PGA) Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spk_left_vol_spkout_mute_n(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPK_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPK_LEFT_VOL_SPKOUT_MUTE_N_MASK;
    tmp |= value << WM8994_SPK_LEFT_VOL_SPKOUT_MUTE_N_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPK_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spk_left_vol_spkout_zc
* Description    : SPKLVOL (Left Speaker Output PGA) Zero Cross Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spk_left_vol_spkout_zc(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPK_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPK_LEFT_VOL_SPKOUT_ZC_MASK;
    tmp |= value << WM8994_SPK_LEFT_VOL_SPKOUT_ZC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPK_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spk_left_vol_spkout_vu
* Description    : Speaker Output PGA Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spk_left_vol_spkout_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPK_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPK_LEFT_VOL_SPKOUT_VU_MASK;
    tmp |= value << WM8994_SPK_LEFT_VOL_SPKOUT_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPK_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spk_right_vol_spkout_vol
* Description    : SPKLVOL (Right Speaker Output PGA) Volume
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spk_right_vol_spkout_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPK_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPK_RIGHT_VOL_SPKOUT_VOL_MASK;
    tmp |= value << WM8994_SPK_RIGHT_VOL_SPKOUT_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPK_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spk_right_vol_spkout_mute_n
* Description    : SPKLVOL (Right Speaker Output PGA) Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spk_right_vol_spkout_mute_n(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPK_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPK_RIGHT_VOL_SPKOUT_MUTE_N_MASK;
    tmp |= value << WM8994_SPK_RIGHT_VOL_SPKOUT_MUTE_N_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPK_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spk_right_vol_spkout_zc
* Description    : SPKLVOL (Right Speaker Output PGA) Zero Cross Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spk_right_vol_spkout_zc(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPK_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPK_RIGHT_VOL_SPKOUT_ZC_MASK;
    tmp |= value << WM8994_SPK_RIGHT_VOL_SPKOUT_ZC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPK_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spk_right_vol_spkout_vu
* Description    : Speaker Output PGA Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spk_right_vol_spkout_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_SPK_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPK_RIGHT_VOL_SPKOUT_VU_MASK;
    tmp |= value << WM8994_SPK_RIGHT_VOL_SPKOUT_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_SPK_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer2_in1rn_to_in1r
* Description    : IN1R PGA Inverting Input Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer2_in1rn_to_in1r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER2_IN1RN_TO_IN1R_MASK;
    tmp |= value << WM8994_INMIXER2_IN1RN_TO_IN1R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer2_in1rp_to_in1r
* Description    : IN1R PGA Non-Inverting Input Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer2_in1rp_to_in1r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER2_IN1RP_TO_IN1R_MASK;
    tmp |= value << WM8994_INMIXER2_IN1RP_TO_IN1R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer2_in2rn_to_in2r
* Description    : IN2R PGA Inverting Input Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer2_in2rn_to_in2r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER2_IN2RN_TO_IN2R_MASK;
    tmp |= value << WM8994_INMIXER2_IN2RN_TO_IN2R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer2_in2rp_to_in2r
* Description    : IN2R PGA Non-Inverting Input Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer2_in2rp_to_in2r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER2_IN2RP_TO_IN2R_MASK;
    tmp |= value << WM8994_INMIXER2_IN2RP_TO_IN2R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer2_in1ln_to_in1l
* Description    : IN1L PGA Inverting Input Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer2_in1ln_to_in1l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER2_IN1LN_TO_IN1L_MASK;
    tmp |= value << WM8994_INMIXER2_IN1LN_TO_IN1L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer2_in1lp_to_in1l
* Description    : IN1L PGA Non-Inverting Input Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer2_in1lp_to_in1l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER2_IN1LP_TO_IN1L_MASK;
    tmp |= value << WM8994_INMIXER2_IN1LP_TO_IN1L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer2_in2ln_to_in2l
* Description    : IN2L PGA Inverting Input Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer2_in2ln_to_in2l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER2_IN2LN_TO_IN2L_MASK;
    tmp |= value << WM8994_INMIXER2_IN2LN_TO_IN2L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer2_in2lp_to_in2l
* Description    : IN2L PGA Non-Inverting Input Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer2_in2lp_to_in2l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER2_IN2LN_TO_IN2L_MASK;
    tmp |= value << WM8994_INMIXER2_IN2LP_TO_IN2L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer3_mixoutl_mixinl_vol
* Description    : Record Path MIXOUTL to MIXINL Gain and Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer3_mixoutl_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER3_MIXOUTL_MIXINL_VOL_MASK;
    tmp |= value << WM8994_INMIXER3_MIXOUTL_MIXINL_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_3, &tmp, 2);
  }
  
  return ret;
}


/*******************************************************************************
* Function Name  : wm8994_inmixer3_in1l_mixinl_vol
* Description    : IN1L PGA Output to MIXINL Gain
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer3_in1l_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER3_IN1L_MIXINL_VOL_MASK;
    tmp |= value << WM8994_INMIXER3_IN1L_MIXINL_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer3_in1l_to_mixinl
* Description    : IN1L PGA Output to MIXINL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer3_in1l_to_mixinl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER3_IN1L_TO_MIXINL_MASK;
    tmp |= value << WM8994_INMIXER3_IN1L_TO_MIXINL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer3_in2l_mixinl_vol
* Description    : IN2L PGA Output to MIXINL Gain
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer3_in2l_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER3_IN2L_MIXINL_VOL_MASK;
    tmp |= value << WM8994_INMIXER3_IN2L_MIXINL_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer3_in2l_to_mixinl
* Description    : IN2L PGA Output to MIXINL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer3_in2l_to_mixinl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_3, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER3_IN2L_TO_MIXINL_MASK;
    tmp |= value << WM8994_INMIXER3_IN2L_TO_MIXINL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_3, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer4_mixoutr_mixinr_vol
* Description    : Record Path MIXOUTR to MIXINR Gain and Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer4_mixoutr_mixinr_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER4_MIXOUTR_MIXINR_VOL_MASK;
    tmp |= value << WM8994_INMIXER4_MIXOUTR_MIXINR_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_4, &tmp, 2);
  }
  
  return ret;
}


/*******************************************************************************
* Function Name  : wm8994_inmixer4_in1r_mixinr_vol
* Description    : IN1R PGA Output to MIXINR Gain
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer4_in1r_mixinr_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER4_IN1R_MIXINR_VOL_MASK;
    tmp |= value << WM8994_INMIXER4_IN1R_MIXINR_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer4_in1r_to_mixinr
* Description    : IN1R PGA Output to MIXINR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer4_in1r_to_mixinr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER4_IN1R_TO_MIXINR_MASK;
    tmp |= value << WM8994_INMIXER4_IN1R_TO_MIXINR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer4_in2r_mixinr_vol
* Description    : IN2R PGA Output to MIXINR Gain
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer4_in2r_mixinr_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER4_IN2R_MIXINR_VOL_MASK;
    tmp |= value << WM8994_INMIXER4_IN2R_MIXINR_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer4_in2r_to_mixinr
* Description    : IN2R PGA Output to MIXINR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer4_in2r_to_mixinr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_4, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER4_IN2R_TO_MIXINR_MASK;
    tmp |= value << WM8994_INMIXER4_IN2R_TO_MIXINR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_4, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer5_in2lrp_mixinl_vol
* Description    : RXVOICE Differential Input (VRXP-VRXN) to MIXINL
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer5_in2lrp_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER5_IN2LRP_MIXINL_VOL_MASK;
    tmp |= value << WM8994_INMIXER5_IN2LRP_MIXINL_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_5, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer5_in1lp_mixinl_vol
* Description    : IN1LP Pin (PGA Bypass) to MIXINL Gain and Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer5_in1lp_mixinl_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_5, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER5_IN1LP_MIXINL_VOL_MASK;
    tmp |= value << WM8994_INMIXER5_IN1LP_MIXINL_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_5, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer6_in2lrp_mixinr_vol
* Description    : RXVOICE Differential Input (VRXP-VRXN) to MIXINR
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer6_in2lrp_mixinr_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_6, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER6_IN2LRP_MIXINR_VOL_MASK;
    tmp |= value << WM8994_INMIXER6_IN2LRP_MIXINR_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_6, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_inmixer6_in1rp_mixinr_vol
* Description    : IN1LP Pin (PGA Bypass) to MIXINR Gain and Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_inmixer6_in1rp_mixinr_vol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_INPUT_MIXER_6, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_INMIXER6_IN1RP_MIXINR_VOL_MASK;
    tmp |= value << WM8994_INMIXER6_IN1RP_MIXINR_VOL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_INPUT_MIXER_6, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer1_dac1l_to_mixoutl
* Description    : Left DAC1 to MIXOUTL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer1_dac1l_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER1_DAC1L_TO_MIXOUTL_MASK;
    tmp |= value << WM8994_OUTMIXER1_DAC1L_TO_MIXOUTL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer1_in2lp_to_mixoutl
* Description    : IN2LP to MIXOUTL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer1_in2lp_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER1_IN2LP_TO_MIXOUTL_MASK;
    tmp |= value << WM8994_OUTMIXER1_IN2LP_TO_MIXOUTL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer1_in1l_to_mixoutl
* Description    : IN1L PGA Output to MIXOUTL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer1_in1l_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER1_IN1L_TO_MIXOUTL_MASK;
    tmp |= value << WM8994_OUTMIXER1_IN1L_TO_MIXOUTL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer1_in1r_to_mixoutl
* Description    : IN1R PGA Output to MIXOUTL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer1_in1r_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER1_IN1R_TO_MIXOUTL_MASK;
    tmp |= value << WM8994_OUTMIXER1_IN1R_TO_MIXOUTL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer1_in2ln_to_mixoutl
* Description    : IN2LN to MIXOUTL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer1_in2ln_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER1_IN2LN_TO_MIXOUTL_MASK;
    tmp |= value << WM8994_OUTMIXER1_IN2LN_TO_MIXOUTL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer1_in2rn_to_mixoutl
* Description    : IN2RN to MIXOUTL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer1_in2rn_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER1_IN2RN_TO_MIXOUTL_MASK;
    tmp |= value << WM8994_OUTMIXER1_IN2RN_TO_MIXOUTL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer1_mixinl_to_mixoutl
* Description    : MIXINL Output (Left ADC bypass) to MIXOUTL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer1_mixinl_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER1_MIXINL_TO_MIXOUTL_MASK;
    tmp |= value << WM8994_OUTMIXER1_MIXINL_TO_MIXOUTL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer1_mixinr_to_mixoutl
* Description    : MIXINR Output (Right ADC bypass) to MIXOUTL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer1_mixinr_to_mixoutl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER1_MIXINR_TO_MIXOUTL_MASK;
    tmp |= value << WM8994_OUTMIXER1_MIXINR_TO_MIXOUTL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer1_dac1l_to_hpout1l
* Description    : HPOUT1LVOL (Left Headphone Output PGA) Input Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer1_dac1l_to_hpout1l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER1_DAC1L_TO_HPOUT1L_MASK;
    tmp |= value << WM8994_OUTMIXER1_DAC1L_TO_HPOUT1L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer2_dac1r_to_mixoutr
* Description    : Right DAC1 to MIXOUTR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer2_dac1r_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER2_DAC1R_TO_MIXOUTR_MASK;
    tmp |= value << WM8994_OUTMIXER2_DAC1R_TO_MIXOUTR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer2_in2rp_to_mixoutr
* Description    : IN2RP to MIXOUTR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer2_in2rp_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER2_IN2RP_TO_MIXOUTR_MASK;
    tmp |= value << WM8994_OUTMIXER2_IN2RP_TO_MIXOUTR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer2_in1r_to_mixoutr
* Description    : IN1R PGA Output to MIXOUTR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer2_in1r_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER2_IN1R_TO_MIXOUTR_MASK;
    tmp |= value << WM8994_OUTMIXER2_IN1R_TO_MIXOUTR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer2_in1l_to_mixoutr
* Description    : IN1L PGA Output to MIXOUTR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer2_in1l_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER2_IN1L_TO_MIXOUTR_MASK;
    tmp |= value << WM8994_OUTMIXER2_IN1L_TO_MIXOUTR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer2_in2rn_to_mixoutr
* Description    : IN2RN to MIXOUTR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer2_in2rn_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER2_IN2RN_TO_MIXOUTR_MASK;
    tmp |= value << WM8994_OUTMIXER2_IN2RN_TO_MIXOUTR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer2_in2ln_to_mixoutr
* Description    : IN2LN to MIXOUTR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer2_in2ln_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER2_IN2LN_TO_MIXOUTR_MASK;
    tmp |= value << WM8994_OUTMIXER2_IN2LN_TO_MIXOUTR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer2_mixinr_to_mixoutr
* Description    : MIXINR Output (Right ADC bypass) to MIXOUTR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer2_mixinr_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER2_MIXINR_TO_MIXOUTR_MASK;
    tmp |= value << WM8994_OUTMIXER2_MIXINR_TO_MIXOUTR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer2_mixinl_to_mixoutr
* Description    : MIXINL Output (Left ADC bypass) to MIXOUTR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer2_mixinl_to_mixoutr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER2_MIXINL_TO_MIXOUTR_MASK;
    tmp |= value << WM8994_OUTMIXER2_MIXINL_TO_MIXOUTR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_outmixer2_dac1r_to_hpout1r
* Description    : HPOUT1RVOL (Right Headphone Output PGA) Input Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_outmixer2_dac1r_to_hpout1r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OUTMIXER2_DAC1R_TO_HPOUT1R_MASK;
    tmp |= value << WM8994_OUTMIXER2_DAC1R_TO_HPOUT1R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixer_dac1r_to_spkmixr
* Description    : Right DAC1 to SPKMIXR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixer_dac1r_to_spkmixr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXER_DAC1R_TO_SPKMIXR_MASK;
    tmp |= value << WM8994_SPKMIXER_DAC1R_TO_SPKMIXR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixer_dac1l_to_spkmixl
* Description    : Left DAC1 to SPKMIXL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixer_dac1l_to_spkmixl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXER_DAC1L_TO_SPKMIXL_MASK;
    tmp |= value << WM8994_SPKMIXER_DAC1L_TO_SPKMIXL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixer_mixoutr_to_spkmixr
* Description    : Right Mixer Output to SPKMIXR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixer_mixoutr_to_spkmixr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXER_MIXOUTR_TO_SPKMIXR_MASK;
    tmp |= value << WM8994_SPKMIXER_MIXOUTR_TO_SPKMIXR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixer_mixoutl_to_spkmixl
* Description    : Left Mixer Output to SPKMIXL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixer_mixoutl_to_spkmixl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXER_MIXOUTL_TO_SPKMIXL_MASK;
    tmp |= value << WM8994_SPKMIXER_MIXOUTL_TO_SPKMIXL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixer_in1rp_to_spkmixr
* Description    : IN1RP to SPKMIXR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixer_in1rp_to_spkmixr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXER_IN1RP_TO_SPKMIXR_MASK;
    tmp |= value << WM8994_SPKMIXER_IN1RP_TO_SPKMIXR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixer_in1lp_to_spkmixl
* Description    : IN1LP to SPKMIXL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixer_in1lp_to_spkmixl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXER_IN1LP_TO_SPKMIXL_MASK;
    tmp |= value << WM8994_SPKMIXER_IN1LP_TO_SPKMIXL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixer_mixinr_to_spkmixr
* Description    : MIXINR (Right ADC bypass) to SPKMIXR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixer_mixinr_to_spkmixr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXER_MIXINR_TO_SPKMIXR_MASK;
    tmp |= value << WM8994_SPKMIXER_MIXINR_TO_SPKMIXR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixer_mixinl_to_spkmixl
* Description    : MIXINL (Left ADC bypass) to SPKMIXL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixer_mixinl_to_spkmixl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXER_MIXINL_TO_SPKMIXL_MASK;
    tmp |= value << WM8994_SPKMIXER_MIXINL_TO_SPKMIXL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixer_dac2r_to_spkmixr
* Description    : Right DAC2 to SPKMIXR Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixer_dac2r_to_spkmixr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXER_DAC2R_TO_SPKMIXR_MASK;
    tmp |= value << WM8994_SPKMIXER_DAC2R_TO_SPKMIXR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_spkmixer_dac2l_to_spkmixl
* Description    : Left DAC2 to SPKMIXL Mute
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_spkmixer_dac2l_to_spkmixl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_SPKMIXER_DAC2L_TO_SPKMIXL_MASK;
    tmp |= value << WM8994_SPKMIXER_DAC2L_TO_SPKMIXL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_OUTPUT_MIXER_2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_antipop2_vmid_disch
* Description    : Connects VMID to ground
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_antipop2_vmid_disch(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANTIPOP2_VMID_DISCH_MASK;
    tmp |= value << WM8994_ANTIPOP2_VMID_DISCH_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_antipop2_bias_src
* Description    : Selects the bias current source
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_antipop2_bias_src(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANTIPOP2_BIAS_SRC_MASK;
    tmp |= value << WM8994_ANTIPOP2_BIAS_SRC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_antipop2_startup_bias_ena
* Description    : Enables the Start-Up bias current generator
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_antipop2_startup_bias_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANTIPOP2_STARTUP_BIAS_ENA_MASK;
    tmp |= value << WM8994_ANTIPOP2_STARTUP_BIAS_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_antipop2_vmid_buf_ena
* Description    : VMID Buffer Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_antipop2_vmid_buf_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANTIPOP2_VMID_BUF_ENA_MASK;
    tmp |= value << WM8994_ANTIPOP2_VMID_BUF_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_antipop2_vmid_ramp
* Description    : VMID soft start enable / slew rate control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_antipop2_vmid_ramp(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANTIPOP2_VMID_RAMP_MASK;
    tmp |= value << WM8994_ANTIPOP2_VMID_RAMP_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_antipop2_micb1_disch
* Description    : Microphone Bias 1 Discharge
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_antipop2_micb1_disch(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANTIPOP2_MICB1_DISCH_MASK;
    tmp |= value << WM8994_ANTIPOP2_MICB1_DISCH_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_antipop2_micb2_disch
* Description    : Microphone Bias 2 Discharge
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_antipop2_micb2_disch(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANTIPOP2_MICB2_DISCH_MASK;
    tmp |= value << WM8994_ANTIPOP2_MICB2_DISCH_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANTIPOP2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_charge_pump1_cp_ena
* Description    : Enable charge-pump digits
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_charge_pump1_cp_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CHARGE_PUMP1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CHARGE_PUMP1_CP_ENA_MASK;
    tmp |= value << WM8994_CHARGE_PUMP1_CP_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CHARGE_PUMP1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_charge_pump2_cp_disch
* Description    : Charge Pump Discharge Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_charge_pump2_cp_disch(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CHARGE_PUMP2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CHARGE_PUMP2_CP_DISCH_MASK;
    tmp |= value << WM8994_CHARGE_PUMP2_CP_DISCH_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CHARGE_PUMP2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_class_w_cp_dyn_pwr
* Description    : Enable dynamic charge pump power control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_class_w_cp_dyn_pwr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CLASS_W, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CLASS_W_CP_DYN_PWR_MASK;
    tmp |= value << WM8994_CLASS_W_CP_DYN_PWR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CLASS_W, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_class_w_cp_dyn_src_sel
* Description    : Selects the digital audio source for envelope tracking
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_class_w_cp_dyn_src_sel(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CLASS_W, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CLASS_W_CP_DYN_SRC_SEL_MASK;
    tmp |= value << WM8994_CLASS_W_CP_DYN_SRC_SEL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CLASS_W, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dc_servo1_dcs_ena_chan_0
* Description    : DC Servo enable for HPOUT1L
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dc_servo1_dcs_ena_chan_0(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DC_SERVO1_DCS_ENA_CHAN_0_MASK;
    tmp |= value << WM8994_DC_SERVO1_DCS_ENA_CHAN_0_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dc_servo1_dcs_ena_chan_1
* Description    : DC Servo enable for HPOUT1R
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dc_servo1_dcs_ena_chan_1(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DC_SERVO1_DCS_ENA_CHAN_1_MASK;
    tmp |= value << WM8994_DC_SERVO1_DCS_ENA_CHAN_1_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dc_servo1_dcs_trig_dac_wr_0
* Description    : Writing 1 to this bit selects DAC Write DC Servo mode for HPOUT1L.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dc_servo1_dcs_trig_dac_wr_0(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DC_SERVO1_DCS_TRIG_DAC_WR_0_MASK;
    tmp |= value << WM8994_DC_SERVO1_DCS_TRIG_DAC_WR_0_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dc_servo1_dcs_trig_dac_wr_1
* Description    : Writing 1 to this bit selects DAC Write DC Servo mode for HPOUT1R.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dc_servo1_dcs_trig_dac_wr_1(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DC_SERVO1_DCS_TRIG_DAC_WR_1_MASK;
    tmp |= value << WM8994_DC_SERVO1_DCS_TRIG_DAC_WR_1_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dc_servo1_dcs_trig_startup_0
* Description    : Writing 1 to this bit selects Start-Up DC Servo mode for HPOUT1L.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dc_servo1_dcs_trig_startup_0(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DC_SERVO1_DCS_TRIG_STARTUP_0_MASK;
    tmp |= value << WM8994_DC_SERVO1_DCS_TRIG_STARTUP_0_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dc_servo1_dcs_trig_startup_1
* Description    : Writing 1 to this bit selects Start-Up DC Servo mode for HPOUT1R.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dc_servo1_dcs_trig_startup_1(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DC_SERVO1_DCS_TRIG_STARTUP_1_MASK;
    tmp |= value << WM8994_DC_SERVO1_DCS_TRIG_STARTUP_1_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dc_servo1_dcs_trig_series_0
* Description    : Writing 1 to this bit selects a series of DC offset corrections
*                  for HPOUT1L.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dc_servo1_dcs_trig_series_0(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DC_SERVO1_DCS_TRIG_SERIES_0_MASK;
    tmp |= value << WM8994_DC_SERVO1_DCS_TRIG_SERIES_0_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dc_servo1_dcs_trig_series_1
* Description    : Writing 1 to this bit selects a series of DC offset corrections
*                  for HPOUT1R.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dc_servo1_dcs_trig_series_1(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DC_SERVO1_DCS_TRIG_SERIES_1_MASK;
    tmp |= value << WM8994_DC_SERVO1_DCS_TRIG_SERIES_1_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dc_servo1_dcs_trig_single_0
* Description    : Writing 1 to this bit selects a single DC offset corrections
*                  for HPOUT1L.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dc_servo1_dcs_trig_single_0(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DC_SERVO1_DCS_TRIG_SINGLE_0_MASK;
    tmp |= value << WM8994_DC_SERVO1_DCS_TRIG_SINGLE_0_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dc_servo1_dcs_trig_single_1
* Description    : Writing 1 to this bit selects a single DC offset corrections
*                  for HPOUT1R.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dc_servo1_dcs_trig_single_1(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DC_SERVO1_DCS_TRIG_SINGLE_1_MASK;
    tmp |= value << WM8994_DC_SERVO1_DCS_TRIG_SINGLE_1_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DC_SERVO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_analog_hp_hpout1r_dly
* Description    : Enables HPOUT1R intermediate stage
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_analog_hp_hpout1r_dly(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANALOG_HP_HPOUT1R_DLY_MASK;
    tmp |= value << WM8994_ANALOG_HP_HPOUT1R_DLY_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_analog_hp_hpout1r_outp
* Description    : Enables HPOUT1R output stage
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_analog_hp_hpout1r_outp(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANALOG_HP_HPOUT1R_OUTP_MASK;
    tmp |= value << WM8994_ANALOG_HP_HPOUT1R_OUTP_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_analog_hp_hpout1r_rmv_short
* Description    : Removes HPOUT1R short
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_analog_hp_hpout1r_rmv_short(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANALOG_HP_HPOUT1R_RMV_SHORT_MASK;
    tmp |= value << WM8994_ANALOG_HP_HPOUT1R_RMV_SHORT_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_analog_hp_hpout1l_dly
* Description    : Enables HPOUT1L intermediate stage
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_analog_hp_hpout1l_dly(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANALOG_HP_HPOUT1L_DLY_MASK;
    tmp |= value << WM8994_ANALOG_HP_HPOUT1L_DLY_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_analog_hp_hpout1l_outp
* Description    : Enables HPOUT1L output stage
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_analog_hp_hpout1l_outp(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANALOG_HP_HPOUT1L_OUTP_MASK;
    tmp |= value << WM8994_ANALOG_HP_HPOUT1L_OUTP_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_analog_hp_hpout1l_rmv_short
* Description    : Removes HPOUT1L short
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_analog_hp_hpout1l_rmv_short(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ANALOG_HP_HPOUT1L_RMV_SHORT_MASK;
    tmp |= value << WM8994_ANALOG_HP_HPOUT1L_RMV_SHORT_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_ANALOG_HP, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_wseq_ctrl1_start_index
* Description    : Sequence Start Index. This field determines the memory location 
*                  of the first command in the selected sequence. 
*                  There are 127 Write Sequencer RAM addresses
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_wseq_ctrl1_start_index(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_WRITE_SEQ_CTRL1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_WSEQ_CTRL1_START_INDEX_MASK;
    tmp |= value << WM8994_WSEQ_CTRL1_START_INDEX_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_WRITE_SEQ_CTRL1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_wseq_ctrl1_start
* Description    : Writing a 1 to this bit starts the write sequencer at the
*                  index location selected by WSEQ_START_INDEX. The sequence 
*                  continues until it reaches an Ã¢â‚¬Å“End of sequenceÃ¢â‚¬Â� flag. At the end
*                  of the sequence, this bit will be reset by the Write Sequencer.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_wseq_ctrl1_start(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_WRITE_SEQ_CTRL1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_WSEQ_CTRL1_START_MASK;
    tmp |= value << WM8994_WSEQ_CTRL1_START_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_WRITE_SEQ_CTRL1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_wseq_ctrl1_abort
* Description    : Writing a 1 to this bit aborts the current sequence and returns
*                  control of the device back to the serial control interface.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_wseq_ctrl1_abort(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_WRITE_SEQ_CTRL1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_WSEQ_CTRL1_ABORT_MASK;
    tmp |= value << WM8994_WSEQ_CTRL1_ABORT_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_WRITE_SEQ_CTRL1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_wseq_ctrl1_ena
* Description    : Write Sequencer Enable.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_wseq_ctrl1_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_WRITE_SEQ_CTRL1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_WSEQ_CTRL1_ENA_MASK;
    tmp |= value << WM8994_WSEQ_CTRL1_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_WRITE_SEQ_CTRL1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_clocking1_ena
* Description    : AIF1CLK Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_clocking1_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CLOCKING1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CLOCKING1_ENA_MASK;
    tmp |= value << WM8994_AIF1_CLOCKING1_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_CLOCKING1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_clocking1_div
* Description    : AIF1CLK Divider
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_clocking1_div(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CLOCKING1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CLOCKING1_DIV_MASK;
    tmp |= value << WM8994_AIF1_CLOCKING1_DIV_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_CLOCKING1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_clocking1_inv
* Description    : AIF1CLK Invert
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_clocking1_inv(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CLOCKING1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CLOCKING1_INV_MASK;
    tmp |= value << WM8994_AIF1_CLOCKING1_INV_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_CLOCKING1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_clocking1_src
* Description    : AIF1CLK Source Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_clocking1_src(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CLOCKING1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CLOCKING1_SRC_MASK;
    tmp |= value << WM8994_AIF1_CLOCKING1_SRC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_CLOCKING1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_clocking1_sysclk_src
* Description    : SYSCLK Source Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_clocking1_sysclk_src(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CLOCKING1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CLOCKING1_SYSCLK_SRC_MASK;
    tmp |= value << WM8994_CLOCKING1_SYSCLK_SRC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CLOCKING1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_clocking1_sysdspclk_ena
* Description    : Digital Mixing Processor Clock Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_clocking1_sysdspclk_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CLOCKING1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CLOCKING1_SYSDSPCLK_ENA_MASK;
    tmp |= value << WM8994_CLOCKING1_SYSDSPCLK_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CLOCKING1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_clocking1_aif2dspclk_ena
* Description    : AIF2 Processing Clock Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_clocking1_aif2dspclk_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CLOCKING1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CLOCKING1_AIF2DSPCLK_ENA_MASK;
    tmp |= value << WM8994_CLOCKING1_AIF2DSPCLK_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CLOCKING1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_clocking1_aif1dspclk_ena
* Description    : AIF1 Processing Clock Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_clocking1_aif1dspclk_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CLOCKING1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CLOCKING1_AIF1DSPCLK_ENA_MASK;
    tmp |= value << WM8994_CLOCKING1_AIF1DSPCLK_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CLOCKING1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_clocking1_toclk_ena
* Description    : Slow Clock (TOCLK) Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_clocking1_toclk_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CLOCKING1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CLOCKING1_TOCLK_ENA_MASK;
    tmp |= value << WM8994_CLOCKING1_TOCLK_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CLOCKING1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_clocking2_opclk_div
* Description    : GPIO Output Clock (OPCLK) Divider
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_clocking2_opclk_div(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CLOCKING2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CLOCKING2_OPCLK_DIV_MASK;
    tmp |= value << WM8994_CLOCKING2_OPCLK_DIV_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CLOCKING2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_clocking2_dbclk_div
* Description    : De-bounce Clock (DBCLK) Divider
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_clocking2_dbclk_div(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CLOCKING2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CLOCKING2_DBCLK_DIV_MASK;
    tmp |= value << WM8994_CLOCKING2_DBCLK_DIV_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CLOCKING2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_clocking2_toclk_div
* Description    : Slow Clock (TOCLK ) Divider
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_clocking2_toclk_div(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_CLOCKING2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_CLOCKING2_TOCLK_DIV_MASK;
    tmp |= value << WM8994_CLOCKING2_TOCLK_DIV_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_CLOCKING2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_clk_rate
* Description    : Selects the AIF1CLK / fs ratio
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_clk_rate(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_RATE, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CLK_RATE_MASK;
    tmp |= value << WM8994_AIF1_CLK_RATE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_RATE, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_sr
* Description    : Selects the AIF1 Sample Rate (fs)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_sr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_RATE, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_SR_MASK;
    tmp |= value << WM8994_AIF1_SR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_RATE, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_sr_r
* Description    : Get the AIF1 Sample Rate (fs)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_sr_r(wm8994_ctx_t *ctx, uint16_t *value)
{
  int32_t ret;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_RATE, value, 2);

  if(ret == 0)
  {  
  *value &= WM8994_AIF1_SR_MASK;
  *value = *value >> WM8994_AIF1_SR_POSITION;
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_control1_fmt
* Description    : Set AIF1 Digital Audio Interface Format
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_control1_fmt(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CONTROL1_FMT_MASK;
    tmp |= value << WM8994_AIF1_CONTROL1_FMT_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_control1_fmt_r
* Description    : Get AIF1 Digital Audio Interface Format
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_control1_fmt_r(wm8994_ctx_t *ctx, uint16_t *value)
{
  int32_t ret;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CONTROL1, value, 2);

  if(ret == 0)
  {  
  *value &= WM8994_AIF1_CONTROL1_FMT_MASK;
  *value = *value >> WM8994_AIF1_CONTROL1_FMT_POSITION;
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_control1_wl
* Description    : Set AIF1 Digital Audio Interface Word length
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_control1_wl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CONTROL1_WL_MASK;
    tmp |= value << WM8994_AIF1_CONTROL1_WL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_control1_wl_r
* Description    : Get AIF1 Digital Audio Interface Word length
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_control1_wl_r(wm8994_ctx_t *ctx, uint16_t *value)
{
  int32_t ret;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CONTROL1, value, 2);

  if(ret == 0)
  {  
  *value &= WM8994_AIF1_CONTROL1_WL_MASK;
  *value = *value >> WM8994_AIF1_CONTROL1_WL_POSITION;
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_control1_lrclk_inv
* Description    : Right, left and I2S modes Ã¢â‚¬â€œ LRCLK1 polarity
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_control1_lrclk_inv(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CONTROL1_LRCLK_INV_MASK;
    tmp |= value << WM8994_AIF1_CONTROL1_LRCLK_INV_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_control1_bclk_inv
* Description    : BCLK1 Invert
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_control1_bclk_inv(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CONTROL1_BCLK_INV_MASK;
    tmp |= value << WM8994_AIF1_CONTROL1_BCLK_INV_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_control1_adc_tdm
* Description    : AIF1 transmit (ADC) TDM Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_control1_adc_tdm(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CONTROL1_ADC_TDM_MASK;
    tmp |= value << WM8994_AIF1_CONTROL1_ADC_TDM_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_control1_adcr_src
* Description    : AIF1 Right Digital Audio interface source
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_control1_adcr_src(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CONTROL1_ADCR_SRC_MASK;
    tmp |= value << WM8994_AIF1_CONTROL1_ADCR_SRC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_control1_adcl_src
* Description    : AIF1 Left Digital Audio interface source
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_control1_adcl_src(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_CONTROL1_ADCR_SRC_MASK;
    tmp |= value << WM8994_AIF1_CONTROL1_ADCL_SRC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_CONTROL1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_ms_lrclk_frc
* Description    : Forces LRCLK1 and ADCLRCLK1 to be enabled when all AIF1 audio 
*                  channels are disabled.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_ms_lrclk_frc(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_MASTER_SLAVE, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_MS_LRCLK_FRC_MASK;
    tmp |= value << WM8994_AIF1_MS_LRCLK_FRC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_MASTER_SLAVE, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_ms_clk_frc
* Description    : Forces BCLK1, LRCLK1 and ADCLRCLK1 to be enabled when all AIF1 
*                  audio channels are disabled.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_ms_clk_frc(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_MASTER_SLAVE, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_MS_CLK_FRC_MASK;
    tmp |= value << WM8994_AIF1_MS_CLK_FRC_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_MASTER_SLAVE, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_ms_mstr
* Description    : AIF1 Audio Interface Master Mode Select.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_ms_mstr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_MASTER_SLAVE, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_MS_MSTR_MASK;
    tmp |= value << WM8994_AIF1_MS_MSTR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_MASTER_SLAVE, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc1_left_vol_adc1l
* Description    : AIF1ADC1 (Left) output path (AIF1, Timeslot 0) Digital Volume.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc1_left_vol_adc1l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC1_LEFT_VOL_ADC1L_MASK;
    tmp |= value << WM8994_AIF1_ADC1_LEFT_VOL_ADC1L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc1_left_vol_adc1l_r
* Description    : Get AIF1ADC1 (Left) output path (AIF1, Timeslot 0) Digital Volume.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc1_left_vol_adc1l_r(wm8994_ctx_t *ctx, uint16_t *value)
{
  int32_t ret;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_LEFT_VOL, value, 2);
  
  if(ret == 0)
  {  
  *value &= WM8994_AIF1_ADC1_LEFT_VOL_ADC1L_MASK;
  *value = *value >> WM8994_AIF1_ADC1_LEFT_VOL_ADC1L_POSITION;
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc1_left_vol_vu
* Description    : AIF1ADC1 output path (AIF1, Timeslot 0) Volume Update.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc1_left_vol_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC1_LEFT_VOL_VU_MASK;
    tmp |= value << WM8994_AIF1_ADC1_LEFT_VOL_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc1_right_vol_adc1r
* Description    : AIF1ADC1 (Right) output path (AIF1, Timeslot 0) Digital Volume.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc1_right_vol_adc1r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC1_RIGHT_VOL_ADC1R_MASK;
    tmp |= value << WM8994_AIF1_ADC1_RIGHT_VOL_ADC1R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc1_right_vol_vu
* Description    : AIF1ADC1 output path (AIF1, Timeslot 0) Volume Update.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc1_right_vol_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC1_RIGHT_VOL_VU_MASK;
    tmp |= value << WM8994_AIF1_ADC1_RIGHT_VOL_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc2_left_vol_adc2l
* Description    : AIF1ADC2 (Left) output path (AIF1, Timeslot 0) Digital Volume.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc2_left_vol_adc2l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC2_LEFT_VOL_ADC2L_MASK;
    tmp |= value << WM8994_AIF1_ADC2_LEFT_VOL_ADC2L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc2_left_vol_vu
* Description    : AIF1ADC2 output path (AIF1, Timeslot 0) Volume Update.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc2_left_vol_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC2_LEFT_VOL_VU_MASK;
    tmp |= value << WM8994_AIF1_ADC2_LEFT_VOL_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc2_right_vol_adc2r
* Description    : AIF1ADC2 (Right) output path (AIF1, Timeslot 0) Digital Volume.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc2_right_vol_adc2r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC2_RIGHT_VOL_ADC2R_MASK;
    tmp |= value << WM8994_AIF1_ADC2_RIGHT_VOL_ADC2R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc2_right_vol_vu
* Description    : AIF1ADC2 output path (AIF1, Timeslot 0) Volume Update.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc2_right_vol_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC2_RIGHT_VOL_VU_MASK;
    tmp |= value << WM8994_AIF1_ADC2_RIGHT_VOL_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc1_filters_adc1r_hpf
* Description    : AIF1ADC1 (Right) output path (AIF1, Timeslot 0) Digital
*                  HPF Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc1_filters_adc1r_hpf(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_FILTERS, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC1_FILTERS_ADC1R_HPF_MASK;
    tmp |= value << WM8994_AIF1_ADC1_FILTERS_ADC1R_HPF_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_FILTERS, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc1_filters_adc1l_hpf
* Description    : AIF1ADC1 (Left) output path (AIF1, Timeslot 0) Digital
*                  HPF Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc1_filters_adc1l_hpf(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_FILTERS, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC1_FILTERS_ADC1L_HPF_MASK;
    tmp |= value << WM8994_AIF1_ADC1_FILTERS_ADC1L_HPF_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_FILTERS, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc1_filters_hpf_cut
* Description    : AIF1ADC1 output path (AIF1, Timeslot 0) Digital HPF
*                  cut-off frequency (fc)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc1_filters_hpf_cut(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_FILTERS, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC1_FILTERS_HPF_CUT_MASK;
    tmp |= value << WM8994_AIF1_ADC1_FILTERS_HPF_CUT_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_FILTERS, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc1_filters_4fs
* Description    : Enable AIF1ADC ultrasonic mode (4FS) output,bypassing all AIF1 
*                  baseband output filtering
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc1_filters_4fs(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_FILTERS, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC1_FILTERS_4FS_MASK;
    tmp |= value << WM8994_AIF1_ADC1_FILTERS_4FS_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_FILTERS, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc2_filters_adc2r_hpf
* Description    : AIF1ADC2 (Right) output path (AIF1, Timeslot 1) Digital
*                  HPF Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc2_filters_adc2r_hpf(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_FILTERS, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC2_FILTERS_ADC2R_HPF_MASK;
    tmp |= value << WM8994_AIF1_ADC2_FILTERS_ADC2R_HPF_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_FILTERS, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc2_filters_adc2l_hpf
* Description    : AIF1ADC2 (Left) output path (AIF1, Timeslot 1) Digital
*                  HPF Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc2_filters_adc2l_hpf(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_FILTERS, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC2_FILTERS_ADC2L_HPF_MASK;
    tmp |= value << WM8994_AIF1_ADC2_FILTERS_ADC2L_HPF_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_FILTERS, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc2_filters_hpf_cut
* Description    : AIF1ADC2 output path (AIF1, Timeslot 0) Digital HPF
*                  cut-off frequency (fc)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc2_filters_hpf_cut(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_FILTERS, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC2_FILTERS_HPF_CUT_MASK;
    tmp |= value << WM8994_AIF1_ADC2_FILTERS_HPF_CUT_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_FILTERS, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_adc2_filters_4fs
* Description    : Enable AIF1ADC ultrasonic mode (4FS) output,bypassing all AIF1 
*                  baseband output filtering
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_adc2_filters_4fs(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_FILTERS, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_ADC2_FILTERS_4FS_MASK;
    tmp |= value << WM8994_AIF1_ADC2_FILTERS_4FS_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_FILTERS, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_filter1_deemp
* Description    : AIF1DAC1 input path (AIF1, Timeslot 0) De-Emphasis Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_filter1_deemp(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_FILTER1_DEEMP_MASK;
    tmp |= value << WM8994_AIF1_DAC1_FILTER1_DEEMP_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_filter1_unmute_ramp
* Description    : AIF1DAC1 input path (AIF1, Timeslot 0) Unmute Ramp select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_filter1_unmute_ramp(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_FILTER1_UNMUTE_RAMP_MASK;
    tmp |= value << WM8994_AIF1_DAC1_FILTER1_UNMUTE_RAMP_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_filter1_muterate
* Description    : AIF1DAC1 input path (AIF1, Timeslot 0) Soft Mute Ramp Rate
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_filter1_muterate(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_FILTER1_MUTERATE_MASK;
    tmp |= value << WM8994_AIF1_DAC1_FILTER1_MUTERATE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_filter1_mono
* Description    : AIF1DAC1 input path (AIF1, Timeslot 0) Mono Mix Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_filter1_mono(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_FILTER1_MONO_MASK;
    tmp |= value << WM8994_AIF1_DAC1_FILTER1_MONO_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_filter1_mute
* Description    : AIF1DAC1 input path (AIF1, Timeslot 0) Soft Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_filter1_mute(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_FILTER1_MUTE_MASK;
    tmp |= value << WM8994_AIF1_DAC1_FILTER1_MUTE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_FILTER1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_filter1_deemp
* Description    : AIF1DAC2 input path (AIF1, Timeslot 1) De-Emphasis Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_filter1_deemp(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_FILTER1_DEEMP_MASK;
    tmp |= value << WM8994_AIF1_DAC2_FILTER1_DEEMP_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_filter1_unmute_ramp
* Description    : AIF1DAC2 input path (AIF1, Timeslot 1) Unmute Ramp select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_filter1_unmute_ramp(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_FILTER1_UNMUTE_RAMP_MASK;
    tmp |= value << WM8994_AIF1_DAC2_FILTER1_UNMUTE_RAMP_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_filter1_muterate
* Description    : AIF1DAC2 input path (AIF1, Timeslot 1) Soft Mute Ramp Rate
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_filter1_muterate(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_FILTER1_MUTERATE_MASK;
    tmp |= value << WM8994_AIF1_DAC2_FILTER1_MUTERATE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_filter1_mono
* Description    : AIF1DAC2 input path (AIF1, Timeslot 1) Mono Mix Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_filter1_mono(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_FILTER1_MONO_MASK;
    tmp |= value << WM8994_AIF1_DAC2_FILTER1_MONO_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_filter1_mute
* Description    : AIF1DAC2 input path (AIF1, Timeslot 1) Soft Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_filter1_mute(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_FILTER1_MUTE_MASK;
    tmp |= value << WM8994_AIF1_DAC2_FILTER1_MUTE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_FILTER1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc1_adc1r_drc_ena
* Description    : Enable DRC in AIF1ADC1 (Right) record path (AIF1, Timeslot 0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc1_adc1r_drc_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC1_ADC1R_DRC_ENA_MASK;
    tmp |= value << WM8994_AIF1DRC1_ADC1R_DRC_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc1_adc1l_drc_ena
* Description    : Enable DRC in AIF1ADC1 (Leftht) record path (AIF1, Timeslot 0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc1_adc1l_drc_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC1_ADC1L_DRC_ENA_MASK;
    tmp |= value << WM8994_AIF1DRC1_ADC1L_DRC_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc1_dac1_drc_ena
* Description    : Enable DRC in AIF1DAC1 playback path (AIF1, Timeslot 0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc1_dac1_drc_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC1_DAC1_DRC_ENA_MASK;
    tmp |= value << WM8994_AIF1DRC1_DAC1_DRC_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc1_anticlip
* Description    : AIF1 DRC1 Anti-clip Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc1_anticlip(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC1_ANTICLIP_MASK;
    tmp |= value << WM8994_AIF1DRC1_ANTICLIP_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc1_qr
* Description    : AIF1 DRC1 Quick-release Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc1_qr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC1_QR_MASK;
    tmp |= value << WM8994_AIF1DRC1_QR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc1_knee2_op_ena
* Description    : AIF1 DRC1 KNEE2_OP Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc1_knee2_op_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC1_KNEE2_OP_ENA_MASK;
    tmp |= value << WM8994_AIF1DRC1_KNEE2_OP_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc1_sig_det
* Description    : AIF1 DRC1 Signal Detect Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc1_sig_det(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC1_SIG_DET_MASK;
    tmp |= value << WM8994_AIF1DRC1_SIG_DET_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc1_sig_det_mode
* Description    : AIF1 DRC1 Signal Detect Mode
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc1_sig_det_mode(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC1_SIG_DET_MODE_MASK;
    tmp |= value << WM8994_AIF1DRC1_SIG_DET_MODE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc1_ng_ena
* Description    : AIF1 DRC1 Noise Gate Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc1_ng_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC1_NG_ENA_MASK;
    tmp |= value << WM8994_AIF1DRC1_NG_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc1_sig_det_pk
* Description    : AIF1 DRC1 Signal Detect Peak Threshold.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc1_sig_det_pk(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC1_SIG_DET_PK_MASK;
    tmp |= value << WM8994_AIF1DRC1_SIG_DET_PK_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc1_sig_det_rms
* Description    : AIF1 DRC1 Signal Detect RMS Threshold.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc1_sig_det_rms(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC1_SIG_DET_RMS_MASK;
    tmp |= value << WM8994_AIF1DRC1_SIG_DET_RMS_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc2_adc2r_drc_ena
* Description    : Enable DRC in AIF1ADC2 (Right) record path (AIF1, Timeslot 0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc2_adc2r_drc_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC2_ADC2R_DRC_ENA_MASK;
    tmp |= value << WM8994_AIF1DRC2_ADC2R_DRC_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc2_adc2l_drc_ena
* Description    : Enable DRC in AIF1ADC2 (Leftht) record path (AIF1, Timeslot 0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc2_adc2l_drc_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC2_ADC2L_DRC_ENA_MASK;
    tmp |= value << WM8994_AIF1DRC2_ADC2L_DRC_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc2_dac2_drc_ena
* Description    : Enable DRC in AIF1DAC2 playback path (AIF1, Timeslot 0)
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc2_dac2_drc_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC2_DAC2_DRC_ENA_MASK;
    tmp |= value << WM8994_AIF1DRC2_DAC2_DRC_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc2_anticlip
* Description    : AIF1 DRC2 Anti-clip Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc2_anticlip(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC2_ANTICLIP_MASK;
    tmp |= value << WM8994_AIF1DRC2_ANTICLIP_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc2_qr
* Description    : AIF1 DRC2 Quick-release Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc2_qr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC2_QR_MASK;
    tmp |= value << WM8994_AIF1DRC2_QR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc2_knee2_op_ena
* Description    : AIF1 DRC2 KNEE2_OP Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc2_knee2_op_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC2_KNEE2_OP_ENA_MASK;
    tmp |= value << WM8994_AIF1DRC2_KNEE2_OP_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc2_sig_det
* Description    : AIF1 DRC2 Signal Detect Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc2_sig_det(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC2_SIG_DET_MASK;
    tmp |= value << WM8994_AIF1DRC2_SIG_DET_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc2_sig_det_mode
* Description    : AIF1 DRC2 Signal Detect Mode
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc2_sig_det_mode(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC2_SIG_DET_MODE_MASK;
    tmp |= value << WM8994_AIF1DRC2_SIG_DET_MODE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc2_ng_ena
* Description    : AIF1 DRC2 Noise Gate Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc2_ng_ena(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC2_NG_ENA_MASK;
    tmp |= value << WM8994_AIF1DRC2_NG_ENA_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc2_sig_det_pk
* Description    : AIF1 DRC2 Signal Detect Peak Threshold.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc2_sig_det_pk(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC2_SIG_DET_PK_MASK;
    tmp |= value << WM8994_AIF1DRC2_SIG_DET_PK_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1drc2_sig_det_rms
* Description    : AIF1 DRC2 Signal Detect RMS Threshold.
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1drc2_sig_det_rms(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1DRC2_SIG_DET_RMS_MASK;
    tmp |= value << WM8994_AIF1DRC2_SIG_DET_RMS_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac1_mixer_vol_adcl
* Description    : Sidetone STL to DAC1L and DAC1R Volume
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac1_mixer_vol_adcl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC1_MIXER_VOL_ADCL_MASK;
    tmp |= value << WM8994_DAC1_MIXER_VOL_ADCL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac1_mixer_vol_adcr
* Description    : Sidetone STL to DAC1L and DAC1R Volume
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac1_mixer_vol_adcr(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC1_MIXER_VOL_ADCR_MASK;
    tmp |= value << WM8994_DAC1_MIXER_VOL_ADCR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DRC2, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_lmrdac1l_to_dac1l
* Description    : Enable AIF1 (Timeslot 0, Left) to DAC1L
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_lmrdac1l_to_dac1l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_LMRDAC1L_TO_DAC1L_MASK;
    tmp |= value << WM8994_AIF1_DAC1_LMRDAC1L_TO_DAC1L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_lmrdac2l_to_dac1l
* Description    : Enable AIF1 (Timeslot 1, Left) to DAC1L
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_lmrdac2l_to_dac1l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_LMRDAC2L_TO_DAC1L_MASK;
    tmp |= value << WM8994_AIF1_DAC1_LMRDAC2L_TO_DAC1L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_lmrdacl_to_dac1l
* Description    : Enable AIF2 (Left) to DAC1L
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_lmrdacl_to_dac1l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_LMRDACL_TO_DAC1L_MASK;
    tmp |= value << WM8994_AIF1_DAC1_LMRDACL_TO_DAC1L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_lmradcl_to_dac1l
* Description    : Enable Sidetone STL to DAC1L
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_lmradcl_to_dac1l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_LMRADCL_TO_DAC1L_MASK;
    tmp |= value << WM8994_AIF1_DAC1_LMRADCL_TO_DAC1L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_lmradcr_to_dac1l
* Description    : Enable Sidetone STR to DAC1L
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_lmradcr_to_dac1l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_LMRADCR_TO_DAC1L_MASK;
    tmp |= value << WM8994_AIF1_DAC1_LMRADCR_TO_DAC1L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_rmrdac1r_to_dac1r
* Description    : Enable AIF1 (Timeslot 0, Right) to DAC1R
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_rmrdac1r_to_dac1r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_RMRDAC1R_TO_DAC1R_MASK;
    tmp |= value << WM8994_AIF1_DAC1_RMRDAC1R_TO_DAC1R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_rmrdac2r_to_dac1r
* Description    : Enable AIF1 (Timeslot 1, Right) to DAC1R
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_rmrdac2r_to_dac1r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_RMRDAC2R_TO_DAC1R_MASK;
    tmp |= value << WM8994_AIF1_DAC1_RMRDAC2R_TO_DAC1R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_rmrdacr_to_dac1r
* Description    : Enable AIF2 (Right) to DAC1R
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_rmrdacr_to_dac1r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_RMRDACR_TO_DAC1R_MASK;
    tmp |= value << WM8994_AIF1_DAC1_RMRDACR_TO_DAC1R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_rmradcl_to_dac1r
* Description    : Enable Sidetone STL to DAC1R
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_rmradcl_to_dac1r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_RMRADCL_TO_DAC1R_MASK;
    tmp |= value << WM8994_AIF1_DAC1_RMRADCL_TO_DAC1R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac1_rmradcr_to_dac1r
* Description    : Enable Sidetone STR to DAC1R
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac1_rmradcr_to_dac1r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC1_RMRADCR_TO_DAC1R_MASK;
    tmp |= value << WM8994_AIF1_DAC1_RMRADCR_TO_DAC1R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC1_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_lmrdac1l_to_dac2l
* Description    : Enable AIF1 (Timeslot 0, Left) to DAC2L
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_lmrdac1l_to_dac2l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_LMRDAC1L_TO_DAC2L_MASK;
    tmp |= value << WM8994_AIF1_DAC2_LMRDAC1L_TO_DAC2L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_lmrdac2l_to_dac2l
* Description    : Enable AIF1 (Timeslot 1, Left) to DAC2L
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_lmrdac2l_to_dac2l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_LMRDAC2L_TO_DAC2L_MASK;
    tmp |= value << WM8994_AIF1_DAC2_LMRDAC2L_TO_DAC2L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_lmrdacl_to_dac2l
* Description    : Enable AIF2 (Left) to DAC2L
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_lmrdacl_to_dac2l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_LMRDACL_TO_DAC2L_MASK;
    tmp |= value << WM8994_AIF1_DAC2_LMRDACL_TO_DAC2L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_lmradcl_to_dac2l
* Description    : Enable Sidetone STL to DAC2L
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_lmradcl_to_dac2l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_LMRADCL_TO_DAC2L_MASK;
    tmp |= value << WM8994_AIF1_DAC2_LMRADCL_TO_DAC2L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_lmradcr_to_dac2l
* Description    : Enable Sidetone STR to DAC2L
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_lmradcr_to_dac2l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_LMRADCR_TO_DAC2L_MASK;
    tmp |= value << WM8994_AIF1_DAC2_LMRADCR_TO_DAC2L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_rmrdac1r_to_dac2r
* Description    : Enable AIF1 (Timeslot 0, Right) to DAC2R
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_rmrdac1r_to_dac2r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_RMRDAC1R_TO_DAC2R_MASK;
    tmp |= value << WM8994_AIF1_DAC2_RMRDAC1R_TO_DAC2R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_rmrdac2r_to_dac2r
* Description    : Enable AIF1 (Timeslot 1, Right) to DAC2R
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_rmrdac2r_to_dac2r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_RMRDAC2R_TO_DAC2R_MASK;
    tmp |= value << WM8994_AIF1_DAC2_RMRDAC2R_TO_DAC2R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_rmrdacr_to_dac2r
* Description    : Enable AIF2 (Right) to DAC2R
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_rmrdacr_to_dac2r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_RMRDACR_TO_DAC2R_MASK;
    tmp |= value << WM8994_AIF1_DAC2_RMRDACR_TO_DAC2R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_rmradcl_to_dac2r
* Description    : Enable Sidetone STL to DAC2R
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_rmradcl_to_dac2r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_RMRADCL_TO_DAC2R_MASK;
    tmp |= value << WM8994_AIF1_DAC2_RMRADCL_TO_DAC2R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_aif1_dac2_rmradcr_to_dac2r
* Description    : Enable Sidetone STR to DAC2R
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_aif1_dac2_rmradcr_to_dac2r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_AIF1_DAC2_RMRADCR_TO_DAC2R_MASK;
    tmp |= value << WM8994_AIF1_DAC2_RMRADCR_TO_DAC2R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_DAC2_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_adc1lmr_aif2dacl_to_aif1adc1l
* Description    : Enable AIF2 (Left) to AIF1 (Timeslot 0, Left) output
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_adc1lmr_aif2dacl_to_aif1adc1l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ADC1LMR_AIF2DACL_TO_AIF1ADC1L_MASK;
    tmp |= value << WM8994_ADC1LMR_AIF2DACL_TO_AIF1ADC1L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_adc1lmr_adc1l_to_aif1adc1l
* Description    : Enable ADCL / DMIC1 (Left) to AIF1 (Timeslot 0, Left) output
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_adc1lmr_adc1l_to_aif1adc1l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ADC1LMR_ADC1L_TO_AIF1ADC1L_MASK;
    tmp |= value << WM8994_ADC1LMR_ADC1L_TO_AIF1ADC1L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_adc1rmr_aif2dacl_to_aif1adc1r
* Description    : Enable AIF2 (Right) to AIF1 (Timeslot 0, Right) output
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_adc1rmr_aif2dacl_to_aif1adc1r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ADC1RMR_AIF2DACL_TO_AIF1ADC1R_MASK;
    tmp |= value << WM8994_ADC1RMR_AIF2DACL_TO_AIF1ADC1R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_adc1rmr_adc1r_to_aif1adc1r
* Description    : Enable ADCR / DMIC1 (Right) to AIF1 (Timeslot 0, Right) output
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_adc1rmr_adc1r_to_aif1adc1r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC1_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ADC1RMR_ADC1R_TO_AIF1ADC1R_MASK;
    tmp |= value << WM8994_ADC1RMR_ADC1R_TO_AIF1ADC1R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC1_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_adc2lmr_aif2dacl_to_aif1adc2l
* Description    : Enable AIF2 (Left) to AIF1 (Timeslot 1, Left) output
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_adc2lmr_aif2dacl_to_aif1adc2l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ADC2LMR_AIF2DACL_TO_AIF1ADC2L_MASK;
    tmp |= value << WM8994_ADC2LMR_AIF2DACL_TO_AIF1ADC2L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_adc2lmr_adc2l_to_aif1adc2l
* Description    : Enable ADCL / DMIC1 (Left) to AIF1 (Timeslot 1, Left) output
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_adc2lmr_adc2l_to_aif1adc2l(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_LMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ADC2LMR_ADC2L_TO_AIF1ADC2L_MASK;
    tmp |= value << WM8994_ADC2LMR_ADC2L_TO_AIF1ADC2L_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_LMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_adc2rmr_aif2dacl_to_aif1adc2r
* Description    : Enable AIF2 (Right) to AIF1 (Timeslot 1, Right) output
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_adc2rmr_aif2dacl_to_aif1adc2r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ADC2RMR_AIF2DACL_TO_AIF1ADC2R_MASK;
    tmp |= value << WM8994_ADC2RMR_AIF2DACL_TO_AIF1ADC2R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_adc2rmr_adc2r_to_aif1adc2r
* Description    : Enable ADCR / DMIC1 (Right) to AIF1 (Timeslot 1, Right) output
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_adc2rmr_adc2r_to_aif1adc2r(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_AIF1_ADC2_RMR, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_ADC2RMR_ADC2R_TO_AIF1ADC2R_MASK;
    tmp |= value << WM8994_ADC2RMR_ADC2R_TO_AIF1ADC2R_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_AIF1_ADC2_RMR, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac1_left_vol_vset
* Description    : DAC1L Digital Volume
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac1_left_vol_vset(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC1_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC1_LEFT_VOL_VSET_MASK;
    tmp |= value << WM8994_DAC1_LEFT_VOL_VSET_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC1_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac1_left_vol_vu
* Description    : DAC1L and DAC1R Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac1_left_vol_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC1_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC1_LEFT_VOL_VU_MASK;
    tmp |= value << WM8994_DAC1_LEFT_VOL_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC1_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac1_left_vol_mute
* Description    : DAC1L Soft Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac1_left_vol_mute(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC1_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC1_LEFT_VOL_MUTE_MASK;
    tmp |= value << WM8994_DAC1_LEFT_VOL_MUTE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC1_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac1_right_vol_vset
* Description    : DAC1R Digital Volume
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac1_right_vol_vset(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC1_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC1_RIGHT_VOL_VSET_MASK;
    tmp |= value << WM8994_DAC1_RIGHT_VOL_VSET_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC1_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac1_right_vol_vu
* Description    : DAC1L and DAC1R Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac1_right_vol_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC1_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC1_RIGHT_VOL_VU_MASK;
    tmp |= value << WM8994_DAC1_RIGHT_VOL_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC1_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac1_right_vol_mute
* Description    : DAC1R Soft Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac1_right_vol_mute(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC1_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC1_RIGHT_VOL_MUTE_MASK;
    tmp |= value << WM8994_DAC1_RIGHT_VOL_MUTE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC1_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac2_left_vol_vset
* Description    : DAC2L Digital Volume
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac2_left_vol_vset(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC2_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC2_LEFT_VOL_VSET_MASK;
    tmp |= value << WM8994_DAC2_LEFT_VOL_VSET_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC2_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac2_left_vol_vu
* Description    : DAC2L and DAC2R Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac2_left_vol_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC2_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC2_LEFT_VOL_VU_MASK;
    tmp |= value << WM8994_DAC2_LEFT_VOL_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC2_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac2_left_vol_mute
* Description    : DAC2L Soft Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac2_left_vol_mute(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC2_LEFT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC2_LEFT_VOL_MUTE_MASK;
    tmp |= value << WM8994_DAC2_LEFT_VOL_MUTE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC2_LEFT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac2_right_vol_vset
* Description    : DAC2R Digital Volume
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac2_right_vol_vset(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC2_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC2_RIGHT_VOL_VSET_MASK;
    tmp |= value << WM8994_DAC2_RIGHT_VOL_VSET_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC2_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac2_right_vol_vu
* Description    : DAC2L and DAC2R Volume Update
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac2_right_vol_vu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC2_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC2_RIGHT_VOL_VU_MASK;
    tmp |= value << WM8994_DAC2_RIGHT_VOL_VU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC2_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_dac2_right_vol_mute
* Description    : DAC2R Soft Mute Control
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_dac2_right_vol_mute(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC2_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_DAC2_RIGHT_VOL_MUTE_MASK;
    tmp |= value << WM8994_DAC2_RIGHT_VOL_MUTE_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC2_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_oversampling_dac_osr128
* Description    : DAC Oversample Rate Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_oversampling_dac_osr128(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC2_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OVERSAMPLING_DAC_OSR128_MASK;
    tmp |= value << WM8994_OVERSAMPLING_DAC_OSR128_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC2_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_oversampling_adc_osr128
* Description    : ADC / Digital Microphone Oversample Rate Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_oversampling_adc_osr128(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_DAC2_RIGHT_VOL, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_OVERSAMPLING_ADC_OSR128_MASK;
    tmp |= value << WM8994_OVERSAMPLING_ADC_OSR128_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_DAC2_RIGHT_VOL, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_gpio1_gp1_fn
* Description    : GPIO1 Pin Function
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_gpio1_gp1_fn(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_GPIO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_GPIO1_GP1_FN_MASK;
    tmp |= value << WM8994_GPIO1_GP1_FN_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_GPIO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_gpio1_gp1_lvl
* Description    : GPIO1 level
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_gpio1_gp1_lvl(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_GPIO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_GPIO1_GP1_LVL_MASK;
    tmp |= value << WM8994_GPIO1_GP1_LVL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_GPIO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_gpio1_gp1_db
* Description    : GPIO1 Input De-bounce
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_gpio1_gp1_db(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_GPIO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_GPIO1_GP1_DB_MASK;
    tmp |= value << WM8994_GPIO1_GP1_DB_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_GPIO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_gpio1_gp1_op_cfg
* Description    : GPIO1 Output Configuration
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_gpio1_gp1_op_cfg(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_GPIO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_GPIO1_GP1_OP_CFG_MASK;
    tmp |= value << WM8994_GPIO1_GP1_OP_CFG_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_GPIO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_gpio1_gp1_pol
* Description    : GPIO1 Polarity Select
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_gpio1_gp1_pol(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_GPIO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_GPIO1_GP1_POL_MASK;
    tmp |= value << WM8994_GPIO1_GP1_POL_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_GPIO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_gpio1_gp1_pd
* Description    : GPIO1 Pull-Down Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_gpio1_gp1_pd(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_GPIO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_GPIO1_GP1_PD_MASK;
    tmp |= value << WM8994_GPIO1_GP1_PD_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_GPIO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_gpio1_gp1_pu
* Description    : GPIO1 Pull-Up Enable
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_gpio1_gp1_pu(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_GPIO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_GPIO1_GP1_PU_MASK;
    tmp |= value << WM8994_GPIO1_GP1_PU_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_GPIO1, &tmp, 2);
  }
  
  return ret;
}

/*******************************************************************************
* Function Name  : wm8994_gpio1_gp1_dir
* Description    : GPIO1 Pin Direction
* Input          : uint16_t
* Output         : None
* Return         : Status [WM8994_ERROR, WM8994_OK]
*******************************************************************************/
int32_t  wm8994_gpio1_gp1_dir(wm8994_ctx_t *ctx, uint16_t value)
{
  int32_t ret;
  uint16_t tmp = 0;
  
  ret = wm8994_read_reg(ctx, WM8994_GPIO1, &tmp, 2);

  if(ret == 0)
  {
    tmp &= ~WM8994_GPIO1_GP1_DIR_MASK;
    tmp |= value << WM8994_GPIO1_GP1_DIR_POSITION;
    
    ret = wm8994_write_reg(ctx, WM8994_GPIO1, &tmp, 2);
  }
  
  return ret;
}

/******************************************************************************/
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
