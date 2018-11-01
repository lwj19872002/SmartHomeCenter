
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
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); 
//                  str[] -- Ҫ��ʾ���ַ���; 
//                  TextSize -- �ַ���С(1:6*8 ; 2:8*16)
//                  maxLen -- ��������ַ������ȣ����16
// Description    : ��ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
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
