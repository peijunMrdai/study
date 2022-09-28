/*
 * dma_uart1_app.h
 *
 *  Created on: Sep 26, 2022
 *      Author: peijun.dai
 */

#ifndef INC_DMA_UART1_APP_H_
#define INC_DMA_UART1_APP_H_

#include "std_isp.h"

//#define ISP_MSG_MAX         256                /// 内部通信交互数据最大长度

extern unsigned short isp_dma_send(void *msg, unsigned short len);          /// 内部通信串口 通过DMA发送数据

extern unsigned char isp_recv_msg[ISP_MSG_MAX];       /// 内部通信 接收数据存储空间
extern int isp_recv_len;                        /// 内部通信 接收数据长度
extern unsigned char isp_send_msg[ISP_MSG_MAX];       /// 内部通信 发送数据存储空间
extern int isp_send_len;                        /// 内部通信 发送数据长度

#endif /* INC_DMA_UART1_APP_H_ */
