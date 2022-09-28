/*
 * ws2812_app.h
 *
 *  Created on: Sep 27, 2022
 *      Author: peijun.dai
 */

#ifndef INC_WS2812_APP_H_
#define INC_WS2812_APP_H_

#ifndef _WS2812_H
#define _WS2812_H
#endif

#define PIXEL_NUM  6
#define NUM (24*PIXEL_NUM + 400)        // Reset 280us / 1.25us = 224
#define WS1 64
#define WS0  36

extern unsigned short send_Buf[NUM];

extern void WS_Load(void);
extern void WS_WriteAll_RGB(unsigned char n_R, unsigned char n_G, unsigned char n_B);
extern void WS_CloseAll(void);

unsigned int WS281x_Color(unsigned char red, unsigned char green, unsigned char blue);
void WS281x_SetPixelColor(unsigned short n, unsigned int GRBColor);
void WS281x_SetPixelRGB(unsigned short n ,unsigned char red, unsigned char green, unsigned char blue);

unsigned int Wheel(unsigned char WheelPos);
void rainbow(unsigned char wait);
void rainbowCycle(unsigned char wait);


#endif /* INC_WS2812_APP_H_ */
