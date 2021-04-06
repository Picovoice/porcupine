#include "main.h"
#include "stm32h7xx_it.h"

extern DMA_HandleTypeDef hdma_sai4_a;
extern UART_HandleTypeDef huart;

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}
void MemManage_Handler(void)
{
  while (1)
  {
  }
}
void BusFault_Handler(void)
{
  while (1)
  {
  }
}
void UsageFault_Handler(void)
{
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

void DMA1_Stream0_IRQHandler(void)
{
}

void USART3_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart);
}

void BDMA_Channel0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_sai4_a);
}
