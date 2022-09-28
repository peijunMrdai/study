/*
 * std_isp.h
 *
 *  Created on: Sep 26, 2022
 *      Author: peijun.dai
 */

#ifndef INC_STD_ISP_H_
#define INC_STD_ISP_H_

#define ISP_MSG_MAX 256        /// 内部通信数据最大长度

/// 报文控制字
#define STX     0x02            /// Start of Text，标识数据包的开始
#define ETX     0x03            /// End of Text，标识数据包的结束
#define DLE     0x10            /// Data Link Escape，防止数据二义性

/// 报文格式定义
typedef struct
{
	unsigned char stx;				/// STX 报文起始标志
	unsigned char dlen[2];			/// 数据域长度
	unsigned char packages[2];		/// 总包数
	unsigned char packageNo[2];		/// 当前包序号
	unsigned char module;			/// 驱动归属

	unsigned char code;				/// 命令代码
	unsigned char reSendNo;			/// 命令重发序号
	unsigned char len[2];			/// 可变数据长度
	unsigned char *data;			/// 可变数据部分
	unsigned char res;				/// 响应码

	unsigned char check[2];			/// 校验
	unsigned char etx;				/// ETX 报文结束标志
}textMsgFormat;

/// 信息拼接结果
typedef enum
{
    JOINT_SUCCESS           = 0x00,             /// 拼接成功
    JOINT_FAIL              = 0x01,             /// 拼接失败
}msgJointRes;

/// 通信事件
typedef struct
{
    unsigned char trigger;                      /// 事件触发类型
    unsigned char cmd;                          /// 通信命令
    unsigned char complete;                     /// 事件完成程度类型
    unsigned char res;                          /// 通信结果
}ispEvent;

/// 驱动升级 请求数据
typedef struct
{
    unsigned char size[4];                          /// 驱动大小
    unsigned char crc[2];                           /// 驱动校验
    unsigned char ver[4];                           /// 驱动待升级版本信息
}isp_devUpgrade_req;

/// 驱动帧 请求数据
typedef struct
{
    unsigned char total[2];                         /// 驱动总帧数
    unsigned char cur[2];                           /// 当前帧序号
    unsigned char len;                              /// 当前帧数据长度
    unsigned char *data;                            /// 当前帧数据内容
    unsigned char crc[2];                           /// 校验
    unsigned char timeout;                          /// 应答超时
}isp_upgradeFrame_req;

/// 驱动升级情况 请求数据
typedef struct
{
    unsigned char ver[4];                           /// 驱动待升级版本信息
}isp_upgradeStat_req;

/// UART1 DMA 中断事件
typedef enum
{
	EVENT_INIT			= 0x00,		/// DMA中断事件 初始化
	EVENT_RECV_IT		= 0x01,		/// DMA中断事件 接收完成
	EVENT_SEND_IT		= 0x02,		/// DMA中断事件 发送完成
}ItEvent;

extern ItEvent it_isp_recv;              /// DMA接收中断事件
extern ItEvent it_isp_send;              /// DMA发送中断事件

extern void textMsg_recv_thread(void);
extern void textMsg_send_thread(void);

extern int textMsg_recv_joint(unsigned char *src, int slen, unsigned char *dest, int *dlen);
extern void textMsg_recv_recover(unsigned char *src, int slen, unsigned char *dest, int *dlen);
extern void textMsg_send_transfer(unsigned char *src, int slen, unsigned char *dest, int *dlen);
extern void textMsg_recv_deal(unsigned char *src, int slen, textMsgFormat *fmt);

extern void textMsg_send_pack(unsigned char cmd, unsigned char *data);

#endif /* INC_STD_ISP_H_ */
