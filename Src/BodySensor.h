
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
  // �Ŷ�̽ͷ״̬
  E_BODYSS_STATUS eDSStatus;
  // �Ŷ�̽ͷ���ʱ��ʱ��
  uint32_t u32DSSTime;
  // �Ŷ�̽ͷ���ʱ��ʱ��
  uint32_t u32DSETime;
  // �Ƿ�ǰ���˽����Ŷ�̽�ⷶΧ
  E_BODY_STATUS eDStatus;
  // �Ŷ�����̽����ʱ
  uint32_t u32DDelay;

  // ����̽ͷ״̬
  E_BODYSS_STATUS eRSStatus;
  // ����̽ͷ���ʱ��ʱ��
  uint32_t u32RSSTime;
  // ����̽ͷ���ʱ��ʱ��
  uint32_t u32RSETime;
  // �Ƿ�ǰ���˽��뷿��̽�ⷶΧ
  E_BODY_STATUS eRStatus;
  // ��������̽����ʱ
  uint32_t u32RDelay;
} X_BODYSS_INFO;

void BodySS_Init(void);
X_BODYSS_INFO *BodySS_GetInfo(void);
bool BodySS_IsHumanOut(void);
bool BodySS_IsHumanIn(void);
void BodySS_Thread(void);
void BodySS_Reset(void);

#endif
