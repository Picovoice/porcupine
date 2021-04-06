/*
* This software component is licensed by ST under Ultimate Liberty license
* SLA0044, the "License"; You may not use this file except in compliance with
* the License. You may obtain a copy of the License at:
*                             www.st.com/SLA0044
*
*/

#include "micCapture_task.h"
#include "main.h"
#include "stdio.h"
#define mic_capture_processWrite_IRQHandler EXTI1_IRQHandler
#define mic_capture_processWrite_IRQ        EXTI1_IRQn

/* The MIC_CAPTURE_PRIO is the priority of the task that triggers audio
processing , default is 8U because most of our examples have capture DMA IT = 7U*/
#ifndef MIC_CAPTURE_PRIO
  #define MIC_CAPTURE_PRIO             (8U)
#endif

#ifdef MIC_CAPTURE_NOTASK_USED

/**
* @brief  sends AudioIn Pushed message
* @param  None
* @retval None
*/
void mic_capture_processTask(void)
{
  #error "user needs to implement the usr_pcm_process  "
  usr_pcm_process(PCM_Buffer);
}


/**
* @brief  creates task for AudioIn capture
* @param  None
* @retval None
*/
void mic_capture_createTask(void)
{

}

/**
* @brief  terminates task for AudioIn capture
* @param  None
* @retval None
*/
void mic_capture_terminateTask(void)
{

}

#else /*microphone processing done with low prio task*/

void mic_capture_processWrite_IRQHandler(void);
/**
* @brief  sends AudioIn Pushed message
* @param  None
* @retval None
*/
void mic_capture_processTask(void)
{
  NVIC_SetPendingIRQ(mic_capture_processWrite_IRQ);
}

/**
* @brief  AudioIn task
* @param  argument: pointer that is passed to the thread function as start argument.
* @retval None
*/
void mic_capture_processWrite_IRQHandler(void)
{
  NVIC_ClearPendingIRQ(mic_capture_processWrite_IRQ);
  //#error "user needs to implement the usr_pcm_process  "
  //usr_pcm_process(PCM_Buffer);
  pv_pcm_process(PCM_Buffer);
}

/**
* @brief  creates task for AudioIn capture
* @param  None
* @retval None
*/
void mic_capture_createTask(void)
{
  HAL_NVIC_SetPriority(mic_capture_processWrite_IRQ, (int32_t)MIC_CAPTURE_PRIO, 0);
  HAL_NVIC_EnableIRQ(mic_capture_processWrite_IRQ);
}

/**
* @brief  terminates task for AudioIn capture
* @param  None
* @retval None
*/
void mic_capture_terminateTask(void)
{
  HAL_NVIC_SetPriority(mic_capture_processWrite_IRQ, 0x0F, 0);
  HAL_NVIC_DisableIRQ(mic_capture_processWrite_IRQ);
}
#endif
