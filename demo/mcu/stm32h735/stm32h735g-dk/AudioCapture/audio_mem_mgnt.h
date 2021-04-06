/**
******************************************************************************
* @file    audio_mem_mgnt.h
* @author  MCD Application Team
* @version V2.2.0
* @date    09-Februray-2021
* @brief   malloc management
******************************************************************************
* @attention
*
* This software component is licensed by ST under Ultimate Liberty license
* SLA0044, the "License"; You may not use this file except in compliance with
* the License. You may obtain a copy of the License at:
*                             www.st.com/SLA0044
*
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AUDIO_MEM_MGNT_H
#define __AUDIO_MEM_MGNT_H



#include <stdint.h>
#include <string.h>

#define AUDIO_MEM_HEAP_FAST     (int32_t)1U
#define AUDIO_MEM_HEAP_SLOW     (int32_t)2U
#define AUDIO_MEM_HEAP_FASTB    (int32_t)3U
#define AUDIO_MEM_NOCACHED      (int32_t)4U


#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */

/* In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 4-bytes aligned */

#if defined   (__GNUC__)        /* GNU Compiler */
  #define __ALIGN_END    __attribute__ ((aligned (4)))
  #define __ALIGN_BEGIN
#else
  #define __ALIGN_END
  #if defined   (__CC_ARM)      /* ARM Compiler */
    #define __ALIGN_BEGIN    __align(4)
  #elif defined (__ICCARM__)    /* IAR Compiler */
    #define __ALIGN_BEGIN
  #elif defined  (__TASKING__)  /* TASKING Compiler */
    #define __ALIGN_BEGIN    __align(4)
  #endif /* __CC_ARM */
#endif /* __GNUC__ */



void CPU_CACHE_Flush(uint8_t *const buffer, uint32_t const length_in_bytes);

void * AudioMalloc(size_t const size, int32_t const type);
void   AudioFree(void *const pMemToFree, int32_t const type);

#endif /* __AUDIO_MEM_MGNT_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
