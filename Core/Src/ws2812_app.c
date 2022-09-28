/*
 * ws2812_app.c
 *
 *  Created on: Sep 27, 2022
 *      Author: peijun.dai
 */

#include "all_header.h"

unsigned short send_Buf[NUM] = {0};

void WS_Load(void)
{
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_4, (unsigned short *)send_Buf, NUM);
}

void WS_CloseAll(void)
{
	unsigned short i;
	for (i = 0; i < PIXEL_NUM * 24; i++)
		send_Buf[i] = WS0; // 写入逻辑0的占空比
	for (i = PIXEL_NUM * 24; i < NUM; i++)
		send_Buf[i] = 0; // 占空比比为0，全为低电平
	WS_Load();
}

void WS_WriteAll_RGB(unsigned char n_R, unsigned char n_G, unsigned char n_B)
{
	unsigned short i, j;
	unsigned char dat[24];
	// 将RGB数据进行转换
	for (i = 0; i < 8; i++)
	{
		dat[i] = ((n_G & 0x80) ? WS1 : WS0);
		n_G <<= 1;
	}
	for (i = 0; i < 8; i++)
	{
		dat[i + 8] = ((n_R & 0x80) ? WS1 : WS0);
		n_R <<= 1;
	}
	for (i = 0; i < 8; i++)
	{
		dat[i + 16] = ((n_B & 0x80) ? WS1 : WS0);
		n_B <<= 1;
	}
	for (i = 0; i < PIXEL_NUM; i++)
	{
		for (j = 0; j < 24; j++)
		{
			send_Buf[i * 24 + j] = dat[j];
		}
	}
	for (i = PIXEL_NUM * 24; i < NUM; i++)
		send_Buf[i] = 0; // 占空比比为0，全为低电平
	WS_Load();
}

unsigned int WS281x_Color(unsigned char red, unsigned char green, unsigned char blue)
{
	return green << 16 | red << 8 | blue;
}

void WS281x_SetPixelColor(unsigned short n, unsigned int GRBColor)
{
	unsigned char i;
	if (n < PIXEL_NUM)
	{
		for (i = 0; i < 24; ++i)
			send_Buf[24 * n + i] = (((GRBColor << i) & 0X800000) ? WS1 : WS0);
	}
}

void WS281x_SetPixelRGB(unsigned short n, unsigned char red, unsigned char green, unsigned char blue)
{
	unsigned char i;

	if (n < PIXEL_NUM)
	{
		for (i = 0; i < 24; ++i)
			send_Buf[24 * (n-1) + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
	}

    WS_Load();
}

unsigned int Wheel(unsigned char WheelPos)
{
	WheelPos = 255 - WheelPos;
	if (WheelPos < 85)
	{
		return WS281x_Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	if (WheelPos < 170)
	{
		WheelPos -= 85;
		return WS281x_Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	WheelPos -= 170;
	return WS281x_Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbow(unsigned char wait)
{
	unsigned int timestamp = HAL_GetTick();
	unsigned short i;
	static unsigned char j;
	static unsigned int next_time = 0;

	unsigned int flag = 0;
	if (next_time < wait)
	{
		if ((unsigned long long )timestamp + wait - next_time > 0)
			flag = 1;
	}
	else if (timestamp > next_time)
	{
		flag = 1;
	}
	if (flag) // && (timestamp - next_time < wait*5))
	{
		j++;
		next_time = timestamp + wait;
		for (i = 0; i < PIXEL_NUM; i++)
		{
			WS281x_SetPixelColor(i, Wheel((i + j) & 255));
		}
	}
	WS_Load();
}

void rainbowCycle(unsigned char wait)
{
	unsigned int timestamp = HAL_GetTick();
	unsigned short i;
	static unsigned char j;
	static unsigned int next_time = 0;

	static unsigned char loop = 0;
	if (loop == 0)
		next_time = timestamp;
	loop = 1; //首次调用初始化

	if ((timestamp > next_time)) // && (timestamp - next_time < wait*5))
	{
		j++;
		next_time = timestamp + wait;
		for (i = 0; i < PIXEL_NUM; i++)
		{
			WS281x_SetPixelColor(i, Wheel(((i * 256 / (PIXEL_NUM)) + j) & 255));
		}
	}
	WS_Load();
}

