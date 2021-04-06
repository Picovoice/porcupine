/*
* This software component is licensed by ST under Ultimate Liberty license
* SLA0044, the "License"; You may not use this file except in compliance with
* the License. You may obtain a copy of the License at:
*                             www.st.com/SLA0044
*
*/

#ifndef _MICCAPTURE_SAIPDM_H
#define _MICCAPTURE_SAIPDM_H

#include "stdint.h"

#define MIC_CAPTURE_IT_MS 15
extern int16_t PCM_Buffer[];
int mic_capture_start(void);

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
