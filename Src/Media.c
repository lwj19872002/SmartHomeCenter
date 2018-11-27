/*
 * Media.c
 *
 *  Created on: 2018Äê11ÔÂ25ÈÕ
 *      Author: LWJ
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "OLED_I2C.h"
#include "Media.h"
#include "cmsis_os.h"

void Media_PlayTimerCallBack(void const *argument);

static osTimerId gxMediaPlayTimerID;
osTimerDef(MediaPlayTimer, Media_PlayTimerCallBack);

static bool gsbIsMediaStart;

void Media_init(void)
{
	gsbIsMediaStart = false;
	gxMediaPlayTimerID = osTimerCreate(osTimer(MediaPlayTimer), osTimerOnce, NULL);
}

void Media_PlayTimerCallBack(void const *argument)
{
	Media_Stop();
}

inline void Media_Start(void)
{
	if(gsbIsMediaStart)
	{
		return;
	}

	HAL_GPIO_WritePin(MEDIA_CTRL_GPIO_Port, MEDIA_CTRL_Pin, GPIO_PIN_RESET);
	osTimerStart(gxMediaPlayTimerID, 30000);
	gsbIsMediaStart = true;
}

inline void Media_Stop(void)
{
	HAL_GPIO_WritePin(MEDIA_CTRL_GPIO_Port, MEDIA_CTRL_Pin, GPIO_PIN_SET);
	gsbIsMediaStart = false;
}
