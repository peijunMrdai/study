/*
 * timer_app.c
 *
 *  Created on: Sep 26, 2022
 *      Author: peijun.dai
 */

#include "timer_app.h"

unsigned short mSecond_delay_counter;                 /// 毫秒延时计数器
unsigned short second_delay_counter;                  /// 秒延时计数器

Timer T_appISP_sendPeriod;               /// 内部通信发送信息周期时长定时器

Timer T_appLed_off[LED_HW_MAX]; 							///<
Timer T_appLed_on[LED_HW_MAX];                           ///<

void timer_init(void)
{
	int i = 0;

	timer_close(&T_appISP_sendPeriod);

	for( i=0; i<LED_HW_MAX; i++ )
	{
		timer_close(&T_appLed_off);
		timer_close(&T_appLed_on);
	}
}

void timer_open(Timer *t)
{
	t->Value = 0;
	t->Switch = TIMER_OPEN;
}

void timer_close(Timer *t)
{
	t->Switch = TIMER_CLOSE;
	t->Value = 0;
}
