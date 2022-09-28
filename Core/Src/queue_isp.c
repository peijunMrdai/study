/*
 * queue_isp.c
 *
 *  Created on: Sep 26, 2022
 *      Author: peijun.dai
 */

#include "queue_isp.h"

ispSendQueue ispSQ;          /// 内部通信发送信息队列
ispSendNode ispSN[ISP_SEND_QMAX] = {0};         /// 内部通信发送信息节点

/**
 * @brief 初始化 内部通信发送队列
 * @details 
 * @param ispSendQueue *queue 发送数据队列指针
 * @param ispSendNode *node 发送数据队列节点指针
 * @return uint8_t 初始化结果
 * 
 * @note
*/
unsigned char ispSendQueue_init(ispSendQueue *queue, ispSendNode *node)
{
    unsigned char res = INIT_Q_SUCCESS;

    queue->front = queue->rear = 0;
	queue->N = 0;
	queue->node = node;

    return res;
}

/**
 * @brief 判断 内部通信发送队列是否为空
 * @details 
 * @param ispSendQueue *queue 发送数据队列指针
 * @return uint8_t 判断结果
 * 
 * @note
*/
unsigned char ispSendQueue_isEmpty(ispSendQueue *queue)
{
    unsigned char res = Q_EMPTY;

	if( queue->N > 0 )
		res = Q_NOEMPTY;

	return res;
}

/**
 * @brief 入队 内部通信发送队列
 * @details 
 * @param ispSendQueue *queue 发送数据队列指针
 * @param int len 数据长度
 * @param char *data 数据内容
 * @return uint8_t 入队结果
 * 
 * @note
*/
unsigned char ispSendQueue_push(ispSendQueue *queue, int len, char *data)
{
    unsigned char res = PUSH_Q_SUCCESS;
	
	/* 将数据插入队列尾部，并移动相应的尾部指针 */
	queue->node[queue->rear].len = len;
	//queue->node[queue->rear].data = data;
	memset(queue->node[queue->rear].data, 0x00, ISP_SEND_DATA_MAX);
	memcpy(queue->node[queue->rear].data, data, len*2);
	queue->rear = (queue->rear+1)%ISP_SEND_QMAX;

	/* 判断插入个数是否大于最大上限值 */
	if( queue->N == ISP_SEND_QMAX )		/// 插入数据个数等于上限
	{
		queue->front = queue->rear;
	}
	else	//不等于
	{
		queue->N++;						/// 计数加1
	}

	return res;
}

/**
 * @brief 出队 内部通信发送队列
 * @details 
 * @param ispSendQueue *queue 发送数据队列指针
 * @return uint8_t 出队结果
 * 
 * @note
*/
unsigned char ispSendQueue_pop(ispSendQueue *queue)
{
    unsigned char res = POP_Q_SUCCESS;

    /// 判断队列中是否存在数据
	if( ispSendQueue_isEmpty(queue) == Q_NOEMPTY )
	{
		queue->node[queue->front].len = 0x00;
		memset(queue->node[queue->front].data, 0x00, ISP_SEND_DATA_MAX);
		queue->front = (queue->front+1)%ISP_SEND_QMAX;
		queue->N--;
	}

	return res;
}

/**
 * @brief 获取 内部通信队列中的首节点数据
 * @details 
 * @param ispSendQueue *queue 发送数据队列指针
 * @return ispSendNode * NULL:不存在该节点;其他:存在该节点
 * 
 * @note
*/
ispSendNode *ispSendQueue_getNode(ispSendQueue *queue)
{
    ispSendNode *node;

    /// 判断队列中是否存在数据
	if( ispSendQueue_isEmpty(queue) == Q_NOEMPTY )
	{
		node = &queue->node[queue->front];
	}

	return node;
}
