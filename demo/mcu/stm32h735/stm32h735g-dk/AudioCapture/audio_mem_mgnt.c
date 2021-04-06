/**
******************************************************************************
* @file    audio_mem_mgnt.c
* @author  STM
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
#include "audio_mem_mgnt.h"
#include <stdlib.h>     /* malloc, free, rand */
#define AUDIO_MEM_UNUSED_VAR(x) ((void)(x))

#if defined(AUDIO_CONF_TRACK_MALLOC)
#define AUDIO_MAX_MEMBLOCKS_NB 3000

typedef struct
{
  int32_t size;
  void    *ptr;
} AudioMemBlock_t;

typedef struct
{
  int32_t         totalSize;
  AudioMemBlock_t memBlocks[AUDIO_MAX_MEMBLOCKS_NB];
} AudioMemoryCurrent_t;

int32_t              AFE_currentBlock = 0;
AudioMemoryCurrent_t AudioMem;
#endif

/* cache flush */
#define _CCSIDR_LSSHIFT(x)  (((x) & SCB_CCSIDR_LINESIZE_Msk) >> SCB_CCSIDR_LINESIZE_Pos)

void CPU_CACHE_Flush(uint8_t *const buffer, uint32_t const length_in_bytes)
{
#ifdef __DCACHE_PRESENT
  uint32_t ccsidr;
  uint32_t smask;
  uint32_t sshift;
  uint32_t ways;
  uint32_t wshift;
  uint32_t ssize;
  uint32_t sets;
  uint32_t sw;
  uint32_t start;
  uint32_t end;

  start = (uint32_t)buffer;
  end   = start + (length_in_bytes * 4);
  /* Get the characteristics of the D-Cache */

  ccsidr = SCB->CCSIDR;
  smask  = CCSIDR_SETS(ccsidr);          /* (Number of sets) - 1 */
  sshift = _CCSIDR_LSSHIFT(ccsidr) + 4;   /* log2(cache-line-size-in-bytes) */
  ways   = CCSIDR_WAYS(ccsidr);          /* (Number of ways) - 1 */

  /* Calculate the bit offset for the way field in the DCCISW register by
   * counting the number of leading zeroes.  For example:
   *
   *   Number of  Value of ways  Field
   *   Ways       'ways'         Offset
   *     2         1             31
   *     4         3             30
   *     8         7             29
   *   ...
   */

  wshift = __CLZ(ways) & 0x1f;

  /* Clean and invalidate the D-Cache over the range of addresses */

  ssize  = (1 << sshift);
  start &= ~(ssize - 1);
  __DSB();

  do
  {
    int32_t tmpways = ways;

    /* Isolate the cache line associated with this address.  For example
     * if the cache line size is 32 bytes and the cache size is 16KB, then
     *
     *   sshift = 5      : Offset to the beginning of the set field
     *   smask  = 0x007f : Mask of the set field
     */

    sets = ((uint32_t)start >> sshift) & smask;

    /* Clean and invalidate each way for this cacheline */

    do
    {
      sw = ((tmpways << wshift) | (sets << sshift));
      SCB->DCCISW=sw;

    }
    while (tmpways--);

    /* Increment the address by the size of one cache line. */

    start += ssize;
  }
  while (start < end);

  __DSB();
  __ISB();
#endif
}


__weak  void *AudioMalloc(size_t const size, int32_t const type)
{
  void *ptr;

#if defined(AUDIO_CONF_TRACK_MALLOC)
  if (AFE_currentBlock < AUDIO_MAX_MEMBLOCKS_NB)
  {
    AudioMem.totalSize                       += size;
    AudioMem.memBlocks[AFE_currentBlock].size = size;
    AudioMem.memBlocks[AFE_currentBlock].ptr  = malloc(size);
    ptr                                       = AudioMem.memBlocks[AFE_currentBlock++].ptr;
  }
  else
  {
    /* more memory blocks than estimated are needed */
    ptr = NULL;
  }
#else
//  AUDIO_MEM_UNUSED_VAR(type);
  ptr = malloc(size);
#endif

  return ptr;
}

__weak void AudioFree(void *const pMemToFree, int32_t const type)
{
  if (pMemToFree != NULL)
  {
#if defined(AUDIO_CONF_TRACK_MALLOC)
    int32_t i;

    for (i = 0; i < AFE_currentBlock; i++)
    {
      if (AudioMem.memBlocks[i].ptr == pMemToFree)
      {
        AudioMem.totalSize -= AudioMem.memBlocks[i].size;
        free(pMemToFree);
        AFE_currentBlock--;
        break;
      }
    }
    /* This block hasn't been allocated through AFE_malloc() */
    return; /*Error*/
#else
    free(pMemToFree);
#endif
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
