/*
 * timer_app.h
 *
 *  Created on: Sep 26, 2022
 *      Author: peijun.dai
 */

#ifndef INC_TIMER_APP_H_
#define INC_TIMER_APP_H_

#include "led_app.h"

#define TIMER_OPEN                  0x01         /// 定时器开启
#define TIMER_CLOSE                 0x00       /// 定时器关闭

#define ISP_SEND_PERIOD_TIME            1      /// 内部通信发送周期时长，单位为毫秒

typedef struct timer_app
{
    unsigned char Switch;                         /// 开关
    unsigned short Value;                         /// 定时值
}Timer;

extern Timer T_appISP_sendPeriod;               /// 内部通信发送信息周期时长

extern Timer T_appLed_off[LED_HW_MAX]; 							///<
extern Timer T_appLed_on[LED_HW_MAX];                           ///<

extern void timer_init(void);
extern void timer_open(Timer *t);
extern void timer_close(Timer *t);

extern unsigned short mSecond_delay_counter;                 /// 毫秒延时计数器
extern unsigned short second_delay_counter;                  /// 秒延时计数器

#endif /* INC_TIMER_APP_H_ */
