
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_exti.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "gpio.h"
#include "BodySensor.h"
#include "HumanInterface.h"

// 人体出门判断时，两个传感器激活时间差阈值，目前为10秒
#define BODYSS_TIMEOUT_THR 100000
// 延时60秒再开启中断，等待探头初始化
#define BODYSS_INIT_DELAY 60000

// 门端中断定义
#define DOORSIDE_INT_IRQ EXTI1_IRQn
#define DOORSIDE_INT_LINE LL_EXTI_LINE_1
// 房端中断定义
#define ROOMSIDE_INT_IRQ EXTI4_IRQn
#define ROOMSIDE_INT_LINE LL_EXTI_LINE_4

void BodySS_InitDelayCallBack(void const *argument);

static X_BODYSS_INFO gxBodySSInfo;

static osTimerId gxBodySSInitTimerID;
osTimerDef(BodySSInitTimer, BodySS_InitDelayCallBack);

void BodySS_InitDelayCallBack(void const *argument)
{
  LL_EXTI_EnableRisingTrig_0_31(DOORSIDE_INT_LINE);
  LL_EXTI_EnableRisingTrig_0_31(ROOMSIDE_INT_LINE);
  LL_EXTI_DisableFallingTrig_0_31(DOORSIDE_INT_LINE);
  LL_EXTI_DisableFallingTrig_0_31(ROOMSIDE_INT_LINE);
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
  __HAL_GPIO_EXTI_CLEAR_IT(DOORSIDE_INT_Pin);

  HAL_NVIC_SetPriority(ROOMSIDE_INT_IRQ, 5, 0);
  HAL_NVIC_DisableIRQ(ROOMSIDE_INT_IRQ);
  __HAL_GPIO_EXTI_CLEAR_IT(ROOMSIDE_INT_Pin);

  gxBodySSInfo.eDSStatus = eLow;
  gxBodySSInfo.u32DSETime = 0;
  gxBodySSInfo.u32DSSTime = 0;

  gxBodySSInfo.eRSStatus = eLow;
  gxBodySSInfo.u32RSETime = 0;
  gxBodySSInfo.u32RSSTime = 0;

  gxBodySSInitTimerID = osTimerCreate(osTimer(BodySSInitTimer), osTimerOnce, NULL);
  // 延时开中断，等待探头初始化
  osTimerStart(gxBodySSInitTimerID, BODYSS_INIT_DELAY);
  HI_ShowCountDown(0, 0, (BODYSS_INIT_DELAY / 1000));
}

X_BODYSS_INFO *BodySS_GetInfo(void)
{
  return &gxBodySSInfo;
}

bool BodySS_IsHumanOut(void)
{
  bool bRet;
  uint32_t u32Tmp;

  bRet = false;
  if ((gxBodySSInfo.eDSStatus == eHigh) && (gxBodySSInfo.eRSStatus == eHigh))
  {
    if (gxBodySSInfo.u32RSSTime <= gxBodySSInfo.u32DSSTime)
    {
      u32Tmp = gxBodySSInfo.u32DSSTime - gxBodySSInfo.u32RSSTime;
    }
    else
    {
      u32Tmp = UINT32_MAX - gxBodySSInfo.u32RSSTime + gxBodySSInfo.u32DSSTime;
    }

    if (u32Tmp < BODYSS_TIMEOUT_THR)
    {
      bRet = true;
    }
  }

  return bRet;
}

bool BodySS_IsHumanIn(void)
{
  bool bRet;
  uint32_t u32Tmp;

  bRet = false;
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

    if (u32Tmp < BODYSS_TIMEOUT_THR)
    {
      bRet = true;
    }
  }

  return bRet;
}
