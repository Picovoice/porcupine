/*
* This software component is licensed by ST under Ultimate Liberty license
* SLA0044, the "License"; You may not use this file except in compliance with
* the License. You may obtain a copy of the License at:
*                             www.st.com/SLA0044
*
*/

#include "micCapture_saiPdm.h"
#include "pdm2pcm.h"
#include "micCapture_task.h"
#include "stdio.h"
#define MIC_CAPTURE_PDM_BUFF_SIZE_BYTES          (MIC_CAPTURE_IT_MS * 2 * 2048 / 8) /* 256 kHz frequency expected on SAI ; 2x for pingPong */
#define MIC_CAPTURE_PCM_BUFF_SIZE_SAMPLES        (MIC_CAPTURE_IT_MS * 16)           /* ms @16kHz */
#define MIC_CAPTURE_MEMORY_SECTION_HW_BUFFERS    ".NOCACHE_Section"



#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
  #ifdef MIC_CAPTURE_MEMORY_SECTION_HW_BUFFERS
    __ALIGN_BEGIN uint8_t PDM_Buffer[MIC_CAPTURE_PDM_BUFF_SIZE_BYTES] __ALIGN_END @ MIC_CAPTURE_MEMORY_SECTION_HW_BUFFERS;
  #else
    __ALIGN_BEGIN uint8_t PDM_Buffer[MIC_CAPTURE_PDM_BUFF_SIZE_BYTES] __ALIGN_END;
  #endif


#else /* keil & (__GNUC__)*/
  __attribute__((__section__(MIC_CAPTURE_MEMORY_SECTION_HW_BUFFERS)))
  __ALIGN_BEGIN uint8_t PDM_Buffer[MIC_CAPTURE_PDM_BUFF_SIZE_BYTES] __ALIGN_END;
#endif

int16_t PCM_Buffer[MIC_CAPTURE_PCM_BUFF_SIZE_SAMPLES];



int mic_capture_start(void)
{
  mic_capture_createTask();
  return HAL_SAI_Receive_DMA(&hsai_BlockA4, PDM_Buffer, (uint16_t)MIC_CAPTURE_PDM_BUFF_SIZE_BYTES);
}


/**
  * @brief  Re targets the C library printf function to the USART.
  * @param  None
  * @retval None
  */

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Call the record update function to get the second half */
  /* Invalidate Data Cache to get the updated content of the SRAM*/
  SCB_InvalidateDCache_by_Addr(PDM_Buffer, MIC_CAPTURE_PDM_BUFF_SIZE_BYTES / 2);
  MX_PDM2PCM_Process((uint16_t *)PDM_Buffer, (uint16_t *)PCM_Buffer);
  mic_capture_processTask();
}


void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* Call the record update function to get the second half */
  /* Invalidate Data Cache to get the updated content of the SRAM*/
  SCB_InvalidateDCache_by_Addr(&PDM_Buffer[MIC_CAPTURE_PDM_BUFF_SIZE_BYTES / 2], MIC_CAPTURE_PDM_BUFF_SIZE_BYTES / 2);
  MX_PDM2PCM_Process((uint16_t *)(&PDM_Buffer[MIC_CAPTURE_PDM_BUFF_SIZE_BYTES / 2]), (uint16_t *)PCM_Buffer);
  mic_capture_processTask();
}


void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsai);
  while (1);
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SAI_ErrorCallback could be implemented in the user file
   */
}
