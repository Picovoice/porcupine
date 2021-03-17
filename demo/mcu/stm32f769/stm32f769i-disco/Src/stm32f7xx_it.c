
#include "stm32f7xx_it.h"
#include "stm32f7xx_hal.h"
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_audio.h"
#include "stm32f769i_discovery_sd.h"

extern SAI_HandleTypeDef haudio_out_sai;
extern DFSDM_Filter_HandleTypeDef       hAudioInTopLeftFilter;
extern DFSDM_Filter_HandleTypeDef       hAudioInTopRightFilter;
extern DFSDM_Filter_HandleTypeDef       hAudioInButtomLeftFilter;
extern DFSDM_Filter_HandleTypeDef       hAudioInButtomRightFilter;
extern SDRAM_HandleTypeDef sdramHandle;
extern SD_HandleTypeDef uSdHandle;
extern UART_HandleTypeDef huart;
extern __IO uint32_t SdmmcTest;
extern __IO uint32_t SdramTest;

void NMI_Handler(void) {
}

void HardFault_Handler(void) {
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}


void DebugMon_Handler(void)
{
}


void PendSV_Handler(void)
{
}


void SysTick_Handler(void)
{
  HAL_IncTick();
}

void DMA2_Stream0_IRQHandler(void)
{
  if(SdmmcTest == 1)
  {
    HAL_DMA_IRQHandler(uSdHandle.hdmarx);
  }
  else if(SdramTest == 1)
  {
    HAL_DMA_IRQHandler(sdramHandle.hdma);
  }
  else
  {
    HAL_DMA_IRQHandler(hAudioInTopLeftFilter.hdmaReg);
  }
}

void DMA2_Stream5_IRQHandler(void)
{
  if(SdmmcTest == 1)
  {
  HAL_DMA_IRQHandler(uSdHandle.hdmatx);
  }
  else
  {
   HAL_DMA_IRQHandler(hAudioInTopRightFilter.hdmaReg);
  }
}


void AUDIO_OUT_SAIx_DMAx_IRQHandler(void)
{
  HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

/**
  * @brief  This function handles DMA2 Stream 6 interrupt request.
  * @param  None
  * @retval None
  */
void AUDIO_DFSDMx_DMAx_BUTTOM_LEFT_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hAudioInButtomLeftFilter.hdmaReg);
}

/**
  * @brief  This function handles DMA2 Stream 7 interrupt request.
  * @param  None
  * @retval None
  */
void AUDIO_DFSDMx_DMAx_BUTTOM_RIGHT_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hAudioInButtomRightFilter.hdmaReg);
}

/**
  * @brief  This function handles SDMMC2 interrupt request.
  * @param  None
  * @retval None
  */
void BSP_SDMMC_IRQHandler(void)
{
  HAL_SD_IRQHandler(&uSdHandle);
}

void USART1_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart);
}
