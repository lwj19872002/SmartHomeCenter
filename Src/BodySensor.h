
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
  // �Ŷ�̽ͷ״̬
  E_BODYSS_STATUS eDSStatus;
  // �Ŷ�̽ͷ���ʱ��ʱ��
  uint32_t u32DSSTime;
  // �Ŷ�̽ͷ���ʱ��ʱ��
  uint32_t u32DSETime;

  // ����̽ͷ״̬
  E_BODYSS_STATUS eRSStatus;
  // ����̽ͷ���ʱ��ʱ��
  uint32_t u32RSSTime;
  // ����̽ͷ���ʱ��ʱ��
  uint32_t u32RSETime;
} X_BODYSS_INFO;

void BodySS_Init(void);
X_BODYSS_INFO *BodySS_GetInfo(void);
bool BodySS_IsHumanOut(void);
bool BodySS_IsHumanIn(void);

#endif
