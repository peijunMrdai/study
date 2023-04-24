/*
 * queue_isp.h
 *
 *  Created on: Sep 26, 2022
 *      Author: peijun.dai
 */

#ifndef INC_QUEUE_ISP_H_
#define INC_QUEUE_ISP_H_

#define INIT_Q_SUCCESS		    0x00			/// 初始化队列头节点 成功
#define INIT_Q_FAIL             0x01			/// 初始化队列头节点 失败

#define Q_EMPTY                 0x00			/// 队列为空
#define Q_NOEMPTY			    0x01			/// 队列不为空

#define INSERT_Q_SUCCESS	    0x00			/// 插入队列节点 成功
#define INSERT_Q_FAIL		    0x01			/// 插入队列节点 失败

#define DELETE_Q_SUCCESS	    0x00			/// 删除队列节点 成功
#define DELETE_Q_FAIL		    0x01			/// 删除队列节点 失败

#define UPDATE_SUCCESS		    0x00			/// 更新节点数据 成功
#define UPDATE_FAIL             0x01			/// 更新节点数据 失败

#define CLR_SUCCESS             0x00			/// 清空节点数据 成功
#define CLR_FAIL			    0x01			/// 清空节点数据 失败

#define PUSH_Q_SUCCESS		    0x00			/// 压入堆栈空间成功
#define PUSH_Q_FAIL             0x01			/// 压入堆栈空间失败

#define POP_Q_SUCCESS		    0x00			/// 取出堆栈空间成功
#define POP_Q_FAIL			    0x01			/// 取出堆栈空间失败

#define ISP_SEND_QMAX           5              /// 内部通信发送信息队列最大值
#define ISP_SEND_DATA_MAX       256             /// 内部通信发送数据长度最大值

/// 内部通信发送信息节点
typedef struct isp_send_node
{
	int len;			    /// 数据长度
	char data[ISP_SEND_DATA_MAX];			/// 数据内容存储地址指针
}ispSendNode;

/// 内部通信发送信息队列
typedef struct isp_send_queue
{
    int front;		            /// 前一个节点地址指针
	int rear;		            /// 后一个节点地址指针

	unsigned char N;		            /// 已插入数据个数

	ispSendNode *node;		    /// 发送信息内容存储首地址
}ispSendQueue;

extern ispSendQueue ispSQ;          /// 内部通信发送信息队列
extern ispSendNode ispSN[ISP_SEND_QMAX];         /// 内部通信发送信息节点

extern unsigned char ispSendQueue_init(ispSendQueue *queue, ispSendNode *node);
extern unsigned char ispSendQueue_isEmpty(ispSendQueue *queue);
extern unsigned char ispSendQueue_push(ispSendQueue *queue, int len, char *data);
extern unsigned char ispSendQueue_pop(ispSendQueue *queue);
extern ispSendNode *ispSendQueue_getNode(ispSendQueue *queue);

#endif /* INC_QUEUE_ISP_H_ */
