/*
 * std_isp.c
 *
 *  Created on: Sep 26, 2022
 *      Author: peijun.dai
 */

#include <string.h>
#include "std_isp.h"
#include "timer_app.h"
#include "queue_isp.h"
#include "dma_uart1_app.h"
#include "main.h"

ItEvent it_isp_recv = EVENT_INIT;               /// DMA接收中断事件
ItEvent it_isp_send = EVENT_INIT;               /// DMA发送中断事件

unsigned char COMM2WaitPack[ISP_MSG_MAX] = {0};         /// 等待组成通讯数据包的缓冲区
int current_wait_pack_len = 0;
unsigned char jointMsg_dump[ISP_MSG_MAX] = {0};         /// 组包信息缓存区
int jointMsg_len = 0;
unsigned char recovMsg_dump[ISP_MSG_MAX] = {0};         /// 反转义信息缓存区
int recovMsg_len = 0;
unsigned char tranfMsg_dump[ISP_MSG_MAX] = {0};         /// 转义信息缓存区
int tranfMsg_len = 0;
textMsgFormat msg;

/**
 * @brief 计算 数据长度
 * @details
 * @param unsigned char *len 长度存储地址
 * @param int num 字节数
 * @return null
 * 
 * @note 特定实现函数
*/
int dataLen_calc(unsigned char *length, int num)
{
    int i, j;  
    int sum = 0; 

    for( i = 0; i <= num-1; i++ )  
    {   
        j = num-1-i;
        sum += (int)((unsigned char)length[i] & 0x0f) << 8*j;
        sum += (int)((unsigned char)length[i] & 0xf0) << 8*j;
    } 

    return sum; 
}

/**
 * @brief 线程 内部通信数据接收流程
 * @details
 * @param null
 * @return null
 * 
 * @note
*/
void textMsg_recv_thread(void)
{
    /// 判断DMA中断触发状态
    if( it_isp_recv == EVENT_RECV_IT )
    {
         /// 将接收信息进行拼接、组包等处理
//         if( textMsg_recv_joint(isp_recv_msg, isp_recv_len, jointMsg_dump, &jointMsg_len) == JOINT_SUCCESS )
//         {
//             textMsg_recv_recover(jointMsg_dump, jointMsg_len, recovMsg_dump, &recovMsg_len);
//             textMsg_recv_deal(recovMsg_dump, recovMsg_len, &msg);
//         }

        /// Test login
        ispSendQueue_push(&ispSQ, isp_recv_len, isp_recv_msg);

        __HAL_DMA_DISABLE(&hdma_usart1_rx);                         /// 关闭串口DMA接收
        memset(isp_recv_msg, 0x00, ISP_MSG_MAX);                    /// 清空接收内容
        isp_recv_len = 0;
        __HAL_DMA_ENABLE(&hdma_usart1_rx);                          /// 使能串口DMA接收

        __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);                    /// 使能串口接收中断
        __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);                    /// 使能串口空闲中断
        HAL_UART_Receive_DMA(&huart1, isp_recv_msg, ISP_MSG_MAX);       /// 设置窗口DMA接收存储空间

        it_isp_recv = EVENT_INIT;
    }
}

/**
 * @brief 线程 内部通信数据发送流程
 * @details
 * @param null
 * @return null
 * 
 * @note
*/
void textMsg_send_thread(void)
{
    ispSendNode *msg;

    if( T_appISP_sendPeriod.Switch == TIMER_CLOSE )
    {
        /// 判断内部通信发送队列是否有数据
        if( ispSendQueue_isEmpty(&ispSQ) == Q_NOEMPTY )
        {
            /// 取出队列中的数据
            msg = ispSendQueue_getNode(&ispSQ);

            textMsg_send_transfer(msg->data, msg->len, tranfMsg_dump, &tranfMsg_len);

            /// 将数据内容进行发送
            isp_dma_send(tranfMsg_dump, tranfMsg_len);

            ispSendQueue_pop(&ispSQ);

            timer_open(&T_appISP_sendPeriod);
        }
    }
    else if( T_appISP_sendPeriod.Value >= ISP_SEND_PERIOD_TIME )
    {
        timer_close(&T_appISP_sendPeriod);
    }
}

/**
 * @brief 拼接 报文信息
 * @details 
 * @param unsigned char *src 需要拼接数据
 * @param int len 需要拼接数据长度
 * @param unsigned char *dest 拼接后数据
 * @param int dlen 拼接后数据长度
 * 
 * @return int 拼接结果
 * 
 * @note
*/
int textMsg_recv_joint(unsigned char *src, int slen, unsigned char *dest, int *dlen)
{
    int ret = JOINT_SUCCESS;
	int i = 0;
	int head = -1;		/// 帧头位置
	int tail = -1;		/// 帧尾位置
    int key_number = 0; 
	unsigned char tmp_buf[ISP_MSG_MAX];     /// 临时缓存,用于主缓存区拷贝
    int temp_msg_len;

    temp_msg_len = current_wait_pack_len;
    if( temp_msg_len+slen > ISP_MSG_MAX )
    {
        memset(COMM2WaitPack, 0, ISP_MSG_MAX);
        temp_msg_len = current_wait_pack_len = 0;
    }

    memcpy(COMM2WaitPack+temp_msg_len, src, slen);        /// 将源数据拼接到等待组成通讯数据包的缓冲区
    
    current_wait_pack_len += slen;

    /// 错误信息过多缓存，导致缓存过多，可能导致访问越界，所以做清空处理
    if( current_wait_pack_len >= ISP_MSG_MAX-1 )
    {
        memset(COMM2WaitPack, 0, ISP_MSG_MAX);
        current_wait_pack_len = 0;
    }

    for( i=0; i<current_wait_pack_len; i++ )
	{
		/* 数据是否存在帧头 */
		if( COMM2WaitPack[i] == STX )
		{
			if( i == 0 )
			{
				head = i;
			}
			else
			{
				if( COMM2WaitPack[i-1] != DLE )
					head = i;
			}
            key_number++;
		}

        /* 数据是否存在DLE */
        if( COMM2WaitPack[i] == DLE )
        {
           key_number++;
        }

		/* 数据是否存在ETX */
		if( COMM2WaitPack[i] == ETX )
		{
            key_number++;
            if( key_number % 2 == 0 )
            {
                tail = i;
                key_number = 0;
                break;
            }
		}
	}
	
	if( head >= 0 )        /// 帧头存在
    {
        if( tail >= 0 )        /// 帧尾存在
        {
            if( tail > head )   /// 帧尾大于帧头
            {
                if( (tail+1) == current_wait_pack_len )    /// 帧尾位置+2等于信息长度
                {
                    memcpy(dest, COMM2WaitPack+head, tail-head+1);
                    *dlen = tail-head+1;
                    memset(COMM2WaitPack, 0, ISP_MSG_MAX);
                    current_wait_pack_len = 0;
                }
                else    /// 帧尾位置+2不等于信息长度
                {
                    memcpy(dest, COMM2WaitPack+head+1, tail-head+1);
                    *dlen = tail-head+1;
                    memset(tmp_buf, 0, ISP_MSG_MAX);
                    memcpy(tmp_buf, COMM2WaitPack+tail+1, current_wait_pack_len-tail-1);
                    current_wait_pack_len = current_wait_pack_len - tail - 1;
                    memset(COMM2WaitPack, 0, ISP_MSG_MAX);
                    memcpy(COMM2WaitPack, tmp_buf, ISP_MSG_MAX-1);
                } 
            }
            else
            {
                memset(tmp_buf, 0, ISP_MSG_MAX);
                memcpy(tmp_buf, COMM2WaitPack+head, current_wait_pack_len-head);  /// 取帧头数据到结尾数据
                current_wait_pack_len -= head;
                memset(COMM2WaitPack, 0, ISP_MSG_MAX);
                memcpy(COMM2WaitPack, tmp_buf, ISP_MSG_MAX-1);
                ret = JOINT_FAIL;
            }   
        }
        else    /// 帧尾不存在
        {
            ret = JOINT_FAIL;
        }
    }
    else    /// 帧头不存在
    {
        memset(COMM2WaitPack, 0, ISP_MSG_MAX);
        current_wait_pack_len = 0;
        if( tail >= 0 )        //帧尾存在
        {
            ret = JOINT_FAIL;         
        }           
        else    //判断数据流存储状态不可用
        {
            ret = JOINT_FAIL;
        }
    }

	return ret;
}

/**
 * @brief 恢复 报文信息
 * @details 将报文数据根据通信定义进行反转义恢复
 * @param unsigned char *src 需要恢复数据
 * @param int len 需要恢复数据长度
 * @param unsigned char *dest 恢复后数据
 * @param int dlen 恢复后数据长度
 * @return null
 * 
 * @note
*/
void textMsg_recv_recover(unsigned char *src, int slen, unsigned char *dest, int *dlen)
{
    int i = 0;
    int j = 0;

    *dlen = slen;
    dest[j] = src[i];
    j++;
    for( i = 1; i < slen-1; i++ )
    {
        if( (src[i] == DLE) && (src[i+1] == STX) )
        {
            dest[j] = src[i+1];
            i++;
            (*dlen)--;
        }
        else if( (src[i] == DLE) && (src[i+1] == ETX) )
        {
            dest[j] = src[i+1];
            i++;
            (*dlen)--;
        }  
        else if( (src[i] == DLE) && (src[i+1] == DLE) )
        {
            dest[j] = src[i+1];
            i++;
            (*dlen)--;
        }  
        else
        {
            dest[j] = src[i];
        }
        j++;
    }
    dest[j] = src[i]; 
}

/**
 * @brief 转义 报文信息
 * @details 将报文数据根据通信定义进行转义
 * @param unsigned char *src 需要转义数据
 * @param int len 需要转义数据长度
 * @param unsigned char *dest 转义后数据
 * @param int dlen 转义后数据长度
 * @return null
 * 
 * @note
*/
void textMsg_send_transfer(unsigned char *src, int slen, unsigned char *dest, int *dlen)
{
    int i = 0;
    int j = 0;

    *dlen = slen;
    dest[j] = src[i];

    j++;
    for( i = 1; i < slen-1; i++ )
    {
        if( (src[i] == STX) || (src[i] == ETX) || (src[i] == DLE) )
        {
            dest[j] = DLE;
            dest[j+1] = src[i];
            j += 2;
            (*dlen)++;
        } 
        else
        {
            dest[j] = src[i];
            j++;
        }
    }
    dest[j] = src[i];
}

/**
 * @brief 处理 内部通信接收数据
 * @details 
 * @param unsigned char *src 数据
 * @param int len 数据长度
 * @param textMsgFormat *fmt 处理后的数据格式化
 * @return null
 * 
 * @note
*/
void textMsg_recv_deal(unsigned char *src, int slen, textMsgFormat *fmt)
{
    int msg_len = 0;          /// 命令报文长度
    unsigned char buf[64] = {0x00};
    unsigned short pulse = 0x0000;

    msg_len = dataLen_calc(src+1, 2);   /// 数据域长度
    if( (slen-11) != msg_len )
    {
        return;
    }

    /// 信息存储

    /// 校验信息

    /// 根据不同命令进行事件处理
    switch( *(src+8) )
    {
        default:
            break;
    }
}

/**
 * @brief 组包 内部通信发送数据内容
 * @details 
 * @param unsigned char cmd 命令代码
 * @param unsigned char *data 组包后的数据
 * @return null
 * 
 * @note
*/
void textMsg_send_pack(unsigned char cmd, unsigned char *data)
{
    unsigned char buf[64] = {0x00};
    int i = 0;
    unsigned short crc;
    unsigned short leakSnrStat = 0x0000;

    memset(buf, 0x00, 64);

    switch( cmd )
    {
        default:
            return;
            break;
    }

    for( i=0; i<64; i++ )
    {
        *(data+i) = buf[i];
    }
}
