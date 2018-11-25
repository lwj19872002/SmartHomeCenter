
#include <string.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "OLED_I2C.h"
#include "HumanInterface.h"
#include "cmsis_os.h"

#define HI_THREAD_TIMER_PERIODIC  1000

static X_HI_INFO gxHIInfo;

static uint8_t gau8Tmp[17];

// thread timer
void HI_ThreadTimerCB(void const *argument);
static osTimerId gxHIThreadTimerID;
osTimerDef(HIThreadTimer, HI_ThreadTimerCB);

void HI_ThreadTimerCB(void const *argument)
{
  uint8_t uacTemp[4];

  if(!gxHIInfo.bShowCD)
  {
    return;
  }

  if(gxHIInfo.u8CDVal > 0)
  {
    gxHIInfo.u8CDVal--;
    if(gxHIInfo.u8CDVal > 0)
    {
      sprintf((char *)uacTemp, "%d", gxHIInfo.u8CDVal);
      HI_ShowStrEN(gxHIInfo.u8CDX, gxHIInfo.u8CDY, uacTemp, 2, 3);
    }
    else
    {
      HI_ShowStrEN(gxHIInfo.u8CDX, gxHIInfo.u8CDY, " ", 2, 3);
      gxHIInfo.bShowCD = false;
    }
  }
}

void HI_Init(void)
{
  memset((uint8_t *)&gxHIInfo, 0x00, sizeof(X_HI_INFO));
  gxHIInfo.bShowCD = false;

	OLED_Init();
	OLED_Fill(0x00);

  gxHIThreadTimerID = osTimerCreate(osTimer(HIThreadTimer), osTimerPeriodic, NULL);
  osTimerStart(gxHIThreadTimerID, HI_THREAD_TIMER_PERIODIC);
}

//--------------------------------------------------------------
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); 
//                  str[] -- 要显示的字符串; 
//                  TextSize -- 字符大小(1:6*8 ; 2:8*16)
//                  maxLen -- 区域最大字符串长度，最大16
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------
void HI_ShowStrEN(uint8_t x, uint8_t y, uint8_t str[], uint8_t TextSize, uint8_t maxLen)
{
  if (maxLen > 16) {
    return;
  }
  
  memset(gau8Tmp, ' ', 17);
  gau8Tmp[maxLen] = '\0';
  OLED_ShowStr(x, y, gau8Tmp, TextSize);
  OLED_ShowStr(x, y, str, TextSize);
}

/***************************************************************
* 在x，y位置显示倒计时数字，val最大255
***************************************************************/
bool HI_ShowCountDown(uint8_t x, uint8_t y, uint8_t val)
{
  if(gxHIInfo.bShowCD)
  {
    return false;
  }

  gxHIInfo.u8CDX = x;
  gxHIInfo.u8CDY = y;
  gxHIInfo.u8CDVal = val;
  gxHIInfo.bShowCD = true;

  return true;
}
