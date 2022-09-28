/*
 * dma_uart1_app.c
 *
 *  Created on: Sep 26, 2022
 *      Author: peijun.dai
 */

#include "main.h"
#include "dma_uart1_app.h"

unsigned char isp_recv_msg[ISP_MSG_MAX];       /// 内部通信 接收数据存储空间
int isp_recv_len = 0;                    /// 内部通信 接收数据长度
unsigned char isp_send_msg[ISP_MSG_MAX];       /// 内部通信 发送数据存储空间
int isp_send_len = 0;					/// 内部通信 发送数据长度

unsigned short isp_dma_send(void *msg, unsigned short len)
{
    /* 判断长度 */
	if( !len )
		return 0;

	HAL_UART_Transmit_DMA(&huart1, (unsigned char *)msg, len);

    return len;
}
