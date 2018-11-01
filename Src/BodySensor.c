
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_exti.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "gpio.h"
#include "BodySensor.h"

// 人体出门判断时，两个传感器激活时间差阈值，目前为10秒
#define BODYSS_TIMEOUT_THR 100000
// 延时100秒再开启中断，等待探头初始化
#define BODYSS_INIT_DELAY 10000

void BodySS_InitDelayCallBack(void const *argument);

static X_BODYSS_INFO gxBodySSInfo;

static osTimerId gxBodySSInitTimerID;
osTimerDef(BodySSInitTimer, BodySS_InitDelayCallBack);

void BodySS_InitDelayCallBack(void const *argument)
{
  LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_1);
  LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_4);
  LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_1);
  LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_4);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == DOORSIDE_INT_Pin)
  {
    HAL_NVIC_DisableIRQ(EXTI1_IRQn);
    if (gxBodySSInfo.eDSStatus == eLow)
    {
      gxBodySSInfo.eDSStatus = eHigh;
      gxBodySSInfo.u32DSSTime = HAL_GetTick();
      LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_1);
      LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_1);
    }
    else
    {
      gxBodySSInfo.eDSStatus = eLow;
      gxBodySSInfo.u32DSETime = HAL_GetTick();
      LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_1);
      LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_1);
    }
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  }

  if (GPIO_Pin == ROOMSIDE_INT_Pin)
  {
    HAL_NVIC_DisableIRQ(EXTI4_IRQn);
    if (gxBodySSInfo.eRSStatus == eLow)
    {
      gxBodySSInfo.eRSStatus = eHigh;
      gxBodySSInfo.u32RSSTime = HAL_GetTick();
      LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_4);
      LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_4);
    }
    else
    {
      gxBodySSInfo.eRSStatus = eLow;
      gxBodySSInfo.u32RSETime = HAL_GetTick();
      LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_4);
      LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_4);
    }
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  }
}

void BodySS_Init(void)
{
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_DisableIRQ(EXTI1_IRQn);
  __HAL_GPIO_EXTI_CLEAR_IT(DOORSIDE_INT_Pin);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_DisableIRQ(EXTI4_IRQn);
  __HAL_GPIO_EXTI_CLEAR_IT(ROOMSIDE_INT_Pin);

  gxBodySSInfo.eDSStatus = eLow;
  gxBodySSInfo.u32DSETime = 0;
  gxBodySSInfo.u32DSSTime = 0;

  gxBodySSInfo.eRSStatus = eLow;
  gxBodySSInfo.u32RSETime = 0;
  gxBodySSInfo.u32RSSTime = 0;

  gxBodySSInitTimerID = osTimerCreate(osTimer(BodySSInitTimer), osTimerOnce, NULL);
  // 延时100秒再开启中断，等待探头初始化
  osTimerStart(gxBodySSInitTimerID, BODYSS_INIT_DELAY);
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
