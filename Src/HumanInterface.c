
#include <string.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "OLED_I2C.h"
#include "HumanInterface.h"

static uint8_t gau8Tmp[17];

void HI_Init(void)
{
	OLED_Init();
	OLED_Fill(0x00);
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
