
#ifndef __BODYSENSOR_H
#define __BODYSENSOR_H

#include <stdbool.h>

typedef enum
{
  eLow = 0x00,
  eHigh,
} E_BODYSS_STATUS;

typedef struct
{
  // 门端探头状态
  E_BODYSS_STATUS eDSStatus;
  // 门端探头变高时的时间
  uint32_t u32DSSTime;
  // 门端探头变低时的时间
  uint32_t u32DSETime;

  // 房端探头状态
  E_BODYSS_STATUS eRSStatus;
  // 房端探头变高时的时间
  uint32_t u32RSSTime;
  // 房端探头变低时的时间
  uint32_t u32RSETime;
} X_BODYSS_INFO;

void BodySS_Init(void);
X_BODYSS_INFO *BodySS_GetInfo(void);
bool BodySS_IsHumanOut(void);
bool BodySS_IsHumanIn(void);

#endif
