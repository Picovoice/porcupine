#ifndef __STM32F7xx_IT_H
#define __STM32F7xx_IT_H

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void DMA2_Stream0_IRQHandler(void);
void DMA2_Stream5_IRQHandler(void);
void AUDIO_DFSDMx_DMAx_BUTTOM_LEFT_IRQHandler(void);
void AUDIO_DFSDMx_DMAx_BUTTOM_RIGHT_IRQHandler(void);
void AUDIO_OUT_SAIx_DMAx_IRQHandler(void);
void BSP_SDMMC_IRQHandler(void);
void USART1_IRQHandler(void);


#endif /* __STM32F7xx_IT_H */
