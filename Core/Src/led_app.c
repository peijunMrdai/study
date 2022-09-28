/*
 * led_app.c
 *
 *  Created on: Sep 26, 2022
 *      Author: peijun.dai
 */

#include "main.h"
#include "led_app.h"
#include "timer_app.h"

void _led_run(__led_biz *biz)
{
    if( T_appLed_on[biz->id-1].Switch == TIMER_CLOSE && T_appLed_off[biz->id-1].Switch == TIMER_CLOSE )
    {
        timer_open(&T_appLed_on[biz->id-1]);
        set_led_state(biz->id, LED_ON);
    }
    else if( T_appLed_on[biz->id-1].Switch == TIMER_OPEN && T_appLed_on[biz->id-1].Value == biz->t_on )
    {
        timer_close(&T_appLed_on[biz->id-1]);
        set_led_state(biz->id, LED_OFF);

        timer_open(&T_appLed_off[biz->id-1]);
    }
    else if( T_appLed_off[biz->id-1].Switch == TIMER_OPEN && T_appLed_off[biz->id-1].Value == biz->t_off )
    {
        timer_close(&T_appLed_off[biz->id-1]);
        biz->cycles--;
    }
}

void set_led_state(__led_id id, __led_state state)
{
    switch( id )
    {
        case LED_RUN:
            HAL_GPIO_WritePin(sysRunLed_GPIO_Port, sysRunLed_Pin, state);
            break;
        default:
            break;
    }
}

void execute_led_biz(__led_biz *biz)
{
    if( biz->style == LED_BIZ_BLOCK )
    {
        while( biz->cycles > 0 )
        {
            _led_run(biz);
        }
    }
    else if( biz->style == LED_BIZ_UNBLOCK )
    {
        if( biz->cycles > 0 )
        {
            _led_run(biz);
        }
    }
}
