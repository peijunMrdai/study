/*
 * led_app.h
 *
 *  Created on: Sep 26, 2022
 *      Author: peijun.dai
 */

#ifndef INC_LED_APP_H_
#define INC_LED_APP_H_

typedef enum ledId
{
    LED_NONE                = 0x00,

    LED_RUN                 = 0x01,

    LED_HW_MAX              = 0x02,

    LED_ALL                 = 0xFF,
}__led_id;

typedef enum ledState
{
    LED_ON              = 0x00,
    LED_OFF             = 0x01,
}__led_state;

typedef enum LedbizStyle
{
    LED_BIZ_BLOCK           = 0x00,                     /// 阻塞
    LED_BIZ_UNBLOCK         = 0x01,                     /// 非阻塞
}__led_biz_style;

typedef struct LedBusiness
{
    __led_id id;                                    /// LED编号

    unsigned char cycles;                           /// 循环数

    unsigned short t_off;                           /// 熄灭时长
    unsigned short t_on;                            /// 点亮时长

    __led_biz_style style;                          /// 执行类型
}__led_biz;

extern void set_led_state(__led_id id, __led_state state);
extern void execute_led_biz(__led_biz *biz);

#endif /* INC_LED_APP_H_ */
