
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_exti.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "gpio.h"
#include "BodySensor.h"
#include "HumanInterface.h"

// ��������ж�ʱ����������������ʱ������ֵ��ĿǰΪ1��
#define BODYSS_TIMEOUT_THR 1000
// ��������ж�ʱ����������������ʱ������ֵ��ĿǰΪ10ms��
#define BODYSS_TIMEMIN_THR 10

// ��ʱ60���ٿ����жϣ��ȴ�̽ͷ��ʼ��
#define BODYSS_INIT_DELAY 60000

// 180��
#define BODYSS_BODY_DELAY (2 * 180)

// �Ŷ��ж϶���
#define DOORSIDE_INT_IRQ EXTI1_IRQn
#define DOORSIDE_INT_LINE LL_EXTI_LINE_1
// �����ж϶���
#define ROOMSIDE_INT_IRQ EXTI4_IRQn
#define ROOMSIDE_INT_LINE LL_EXTI_LINE_4

void BodySS_InitDelayCallBack(void const *argument);

static X_BODYSS_INFO gxBodySSInfo;

static osTimerId gxBodySSInitTimerID;
osTimerDef(BodySSInitTimer, BodySS_InitDelayCallBack);

void BodySS_InitDelayCallBack(void const *argument)
{
  gxBodySSInfo.eDSStatus = eLow;
  gxBodySSInfo.u32DSETime = 0;
  gxBodySSInfo.u32DSSTime = 0;
  gxBodySSInfo.eDStatus = eNone;
  gxBodySSInfo.u32DDelay = 0;

  gxBodySSInfo.eRSStatus = eLow;
  gxBodySSInfo.u32RSETime = 0;
  gxBodySSInfo.u32RSSTime = 0;
  gxBodySSInfo.eRStatus = eNone;
  gxBodySSInfo.u32RDelay = 0;

  LL_EXTI_EnableRisingTrig_0_31(DOORSIDE_INT_LINE);
  LL_EXTI_EnableRisingTrig_0_31(ROOMSIDE_INT_LINE);
  LL_EXTI_DisableFallingTrig_0_31(DOORSIDE_INT_LINE);
  LL_EXTI_DisableFallingTrig_0_31(ROOMSIDE_INT_LINE);

  __HAL_GPIO_EXTI_CLEAR_IT(DOORSIDE_INT_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(ROOMSIDE_INT_Pin);

  HAL_NVIC_EnableIRQ(DOORSIDE_INT_IRQ);
  HAL_NVIC_EnableIRQ(ROOMSIDE_INT_IRQ);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == DOORSIDE_INT_Pin)
  {
    HAL_NVIC_DisableIRQ(DOORSIDE_INT_IRQ);
    if (gxBodySSInfo.eDSStatus == eLow)
    {
      gxBodySSInfo.eDSStatus = eHigh;
      gxBodySSInfo.u32DSSTime = HAL_GetTick();
      LL_EXTI_DisableRisingTrig_0_31(DOORSIDE_INT_LINE);
      LL_EXTI_EnableFallingTrig_0_31(DOORSIDE_INT_LINE);

      gxBodySSInfo.eDStatus = eHaveBody;
      gxBodySSInfo.u32DDelay = BODYSS_BODY_DELAY;
    }
    else
    {
      gxBodySSInfo.eDSStatus = eLow;
      gxBodySSInfo.u32DSETime = HAL_GetTick();
      LL_EXTI_EnableRisingTrig_0_31(DOORSIDE_INT_LINE);
      LL_EXTI_DisableFallingTrig_0_31(DOORSIDE_INT_LINE);
    }
    HAL_NVIC_EnableIRQ(DOORSIDE_INT_IRQ);
  }

  if (GPIO_Pin == ROOMSIDE_INT_Pin)
  {
    HAL_NVIC_DisableIRQ(ROOMSIDE_INT_IRQ);
    if (gxBodySSInfo.eRSStatus == eLow)
    {
      gxBodySSInfo.eRSStatus = eHigh;
      gxBodySSInfo.u32RSSTime = HAL_GetTick();
      LL_EXTI_DisableRisingTrig_0_31(ROOMSIDE_INT_LINE);
      LL_EXTI_EnableFallingTrig_0_31(ROOMSIDE_INT_LINE);

      gxBodySSInfo.eRStatus = eHaveBody;
      gxBodySSInfo.u32RDelay = BODYSS_BODY_DELAY;
    }
    else
    {
      gxBodySSInfo.eRSStatus = eLow;
      gxBodySSInfo.u32RSETime = HAL_GetTick();
      LL_EXTI_EnableRisingTrig_0_31(ROOMSIDE_INT_LINE);
      LL_EXTI_DisableFallingTrig_0_31(ROOMSIDE_INT_LINE);
    }
    HAL_NVIC_EnableIRQ(ROOMSIDE_INT_IRQ);
  }
}

void BodySS_Init(void)
{
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(DOORSIDE_INT_IRQ, 5, 0);
  HAL_NVIC_DisableIRQ(DOORSIDE_INT_IRQ);

  HAL_NVIC_SetPriority(ROOMSIDE_INT_IRQ, 5, 0);
  HAL_NVIC_DisableIRQ(ROOMSIDE_INT_IRQ);

  gxBodySSInitTimerID = osTimerCreate(osTimer(BodySSInitTimer), osTimerOnce, NULL);
  // ��ʱ���жϣ��ȴ�̽ͷ��ʼ��
  osTimerStart(gxBodySSInitTimerID, BODYSS_INIT_DELAY);
  HI_ShowCountDown(0, 0, (BODYSS_INIT_DELAY / 1000));

  gxBodySSInfo.eDStatus = eNone;
  gxBodySSInfo.u32DDelay = 0;
  gxBodySSInfo.eRStatus = eNone;
  gxBodySSInfo.u32RDelay = 0;
}

X_BODYSS_INFO *BodySS_GetInfo(void)
{
  return &gxBodySSInfo;
}

void BodySS_Thread(void)
{
  if((eHaveBody == gxBodySSInfo.eDStatus) && (gxBodySSInfo.u32DDelay > 0))
  {
    gxBodySSInfo.u32DDelay--;
  }
  else
  {
    gxBodySSInfo.eDStatus = eNone;
  }

  if((eHaveBody == gxBodySSInfo.eRStatus) && (gxBodySSInfo.u32RDelay > 0))
  {
    gxBodySSInfo.u32RDelay--;
  }
  else
  {
    gxBodySSInfo.eRStatus = eNone;
  }
}

// �жϵ�ǰ״̬�Ƿ����˳��Ŷ���
bool BodySS_IsHumanOut(void)
{
  bool bRet;
  uint32_t u32Tmp;

  bRet = false;
#if 0
  // ֻ�õ�����������������������²��ж�״̬
  if ((gxBodySSInfo.eDSStatus == eHigh) && (gxBodySSInfo.eRSStatus == eHigh))
  {
    // �������ʱ���
    if (gxBodySSInfo.u32RSSTime <= gxBodySSInfo.u32DSSTime)
    {
      u32Tmp = gxBodySSInfo.u32DSSTime - gxBodySSInfo.u32RSSTime;
    }
    else
    {
      u32Tmp = UINT32_MAX - gxBodySSInfo.u32RSSTime + gxBodySSInfo.u32DSSTime;
    }

    // �ж�ʱ��
    if ((u32Tmp < BODYSS_TIMEOUT_THR) && (u32Tmp > BODYSS_TIMEMIN_THR))
    {
      bRet = true;
    }
  }
#else
  // ������������̽�⵽�������˳���
  if((eHaveBody == gxBodySSInfo.eDStatus) && (eHaveBody == gxBodySSInfo.eRStatus))
  {
    bRet = true;
  }
#endif

  return bRet;
}

// �жϵ�ǰ״̬�Ƿ�����˽��Ŷ���
bool BodySS_IsHumanIn(void)
{
  bool bRet;
  uint32_t u32Tmp;

  bRet = false;
#if 1
  if ((gxBodySSInfo.eDSStatus == eHigh) && (gxBodySSInfo.eRSStatus == eHigh))
  {
    if (gxBodySSInfo.u32RSSTime >= gxBodySSInfo.u32DSSTime)
    {
      u32Tmp = gxBodySSInfo.u32RSSTime - gxBodySSInfo.u32DSSTime;
    }
    else
    {
      u32Tmp = UINT32_MAX - gxBodySSInfo.u32DSSTime + gxBodySSInfo.u32RSSTime;
    }

    if ((u32Tmp < BODYSS_TIMEOUT_THR) && (u32Tmp > BODYSS_TIMEMIN_THR))
    {
      bRet = true;
    }
  }
#else
  if((eHaveBody == gxBodySSInfo.eDStatus) && (eHaveBody == gxBodySSInfo.eRStatus))
  {
    bRet = true;
  }
#endif

  return bRet;
}
