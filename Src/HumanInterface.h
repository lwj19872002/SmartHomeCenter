
#ifndef __HUMANINTERFACE_H
#define __HUMANINTERFACE_H

#include <stdbool.h>

typedef struct {
  bool  bShowCD;
  uint8_t u8CDX;
  uint8_t u8CDY;
  uint8_t u8CDVal;
} X_HI_INFO;

void HI_Init(void);
void HI_ShowStrEN(uint8_t x, uint8_t y, uint8_t str[], uint8_t TextSize, uint8_t maxLen);
bool HI_ShowCountDown(uint8_t x, uint8_t y, uint8_t val);

#endif
