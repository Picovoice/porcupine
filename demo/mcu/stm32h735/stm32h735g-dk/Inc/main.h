
#ifndef __MAIN_H
#define __MAIN_H

#include "stm32h7xx_hal.h"
#include "micCapture_saiPdm.h"

extern SAI_HandleTypeDef hsai_BlockA4;
extern DMA_HandleTypeDef hdma_sai4_a;

extern CRC_HandleTypeDef hcrc;
extern UART_HandleTypeDef huart;

void pv_pcm_process(int16_t *record_pcm_buffer);
void pv_error_handler(void);

#endif /* __MAIN_H */
