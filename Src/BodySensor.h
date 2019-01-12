
#ifndef __BODYSENSOR_H
#define __BODYSENSOR_H

#include <stdbool.h>

typedef enum
{
  eLow = 0x00,
  eHigh,
} E_BODYSS_STATUS;

typedef enum
{
  eNone = 0x00,
  eHaveBody,
} E_BODY_STATUS;

typedef struct
{
  // 门端探头状态
  E_BODYSS_STATUS eDSStatus;
  // 门端探头变高时的时间
  uint32_t u32DSSTime;
  // 门端探头变低时的时间
  uint32_t u32DSETime;
  // 是否当前有人进入门端探测范围
  E_BODY_STATUS eDStatus;
  // 门端有人探测延时
  uint32_t u32DDelay;

  // 房端探头状态
  E_BODYSS_STATUS eRSStatus;
  // 房端探头变高时的时间
  uint32_t u32RSSTime;
  // 房端探头变低时的时间
  uint32_t u32RSETime;
  // 是否当前有人进入房端探测范围
  E_BODY_STATUS eRStatus;
  // 房端有人探测延时
  uint32_t u32RDelay;
} X_BODYSS_INFO;

void BodySS_Init(void);
X_BODYSS_INFO *BodySS_GetInfo(void);
bool BodySS_IsHumanOut(void);
bool BodySS_IsHumanIn(void);
void BodySS_Thread(void);
void BodySS_Reset(void);

#endif
