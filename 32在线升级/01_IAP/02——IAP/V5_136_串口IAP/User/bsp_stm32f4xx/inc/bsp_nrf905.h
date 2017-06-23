/*
*********************************************************************************************************
*	                                  
*	模块名称 : nRF905驱动模块
*	文件名称 : bsp_nrf905.c
*	版    本 : V1.0
*	说    明 : nRF905驱动
*	修改记录 :
*		版本号    日期          作者          说明
*		v1.0    2015-02-28     Eric2013	      首版
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_NRF905_H_
#define __BSP_NRF905_H_

#include "stm32f4xx.h"

#define nRF905_BufferSize 	32
#define nRF905_AddressSize   4
#define NOP                0xFF						   /* SPI 全双工时的空操作 */

extern uint8_t nRF905_TxAddress[nRF905_AddressSize];   /* 发送地址所在数组 */
extern uint8_t nRF905_RxAddress[nRF905_AddressSize];   /* 接收地址所在数组 */

/********************* nRF905 SPI接口 控制指令 宏定义 *****************************************/

#define nRF905_WC 		0x00        // 设置配置寄存器 (write RF-Configeration Register)
#define nRF905_RC		0x10		// 读取配置寄存器 (read  RF-Configeration Register)

#define nRF905_WAR		0x01		//设置自动重传 AUTO_RETRAN 模式 (set:0x20\ restset:0x00)
#define nRF905_RAR		0x11		//读取自动重传 AUTO_RETRAN 模式 (set:0x20\ restset:0x00)

#define nRF905_WTRX_AFW 0x02		// 设置收发地址格式的宽度（字节个数）(Bit[7]:没用; TX_AFW[2:0] Bit[3]:没用; RX_AFW[2:0] 默认值 0100_0100 : 0x44)
#define nRF905_RTRX_AFW 0x12        // 读取收发地址格式的宽度（字节个数）(Bit[7]:没用; TX_AFW[2:0] Bit[3]:没用; RX_AFW[2:0] 默认值0100_0100 : 0x44 )

#define nRF905_WRX_PW	 0x03		//设置接受数据的rx-payload寄存器的有效数据的宽度 (Bit[7:6]没用RX_PWR[5:0] 默认值 0010_0000 : 0x20)
#define nRF905_RRX_PW	 0x13		//读取接受数据的rx-payload寄存器的有效数据的宽度 (Bit[7:6]没用RX_PWR[5:0] 默认值 0010_0000 : 0x20)

#define nRF905_WTX_PW 	0x04		//设置接受数据的tx-payload寄存器的有效数据的宽度 (Bit[7:6]没用RX_PWR[5:0] 默认值 0010_0000 : 0x20)	
#define nRF905_RTX_PW 	0x14		//读取接受数据的tx-payload寄存器的有效数据的宽度 (Bit[7:6]没用RX_PWR[5:0] 默认值 0010_0000 : 0x20)

#define nRF905_WRA		0x05		// 设置位于配置寄存器第5-8的4个字节的Rx-Address (write RX-Address)
#define nRF905_RRA		0x15		// 读取位于配置寄存器第5-8的4个字节的Rx-Address	(read Rx-Address)

#define nRF905_WCRC 	0x09 		// 设置CRC 校验的模式
#define nRF905_RCRC 	0x19 		// 读取CRC 校验的模式

#define nRF905_WTP		0x20		// 向TX-Payload 寄存器写入有效数据 (write the payload data to the Tx-payload register)
#define nRF905_RTP 		0x21		// 从Tx-payload 寄存器中读出有效数据 read the payload data in the Tx-payload register)

#define nRF905_WTA 		0x22		// 向Tx-Address 寄存器写入发送地址，即目标地址(write the destination address in the Tx-Address register)
#define nRF905_RTA		0x23		// 从Tx-Address	寄存器读取发送地址，(read the destination address from the Tx-Address register)

#define nRF905_RRP		0x24		// 从Tx-Payload 寄存器中读取接收到的有效数据(read the payload data from the Rx-Payload register)


/* 供外部文件调用的函数 */
void nRF905_InitHard(void);
void nRF905_EXTI_DoIt(void);
void nRF905_SetTxAddress(uint8_t *_pBuf, uint8_t _ucLen);
void nRF905_SetRxAddress(uint8_t *_pBuf, uint8_t _ucLen);
void nRF905_SendPacket(uint8_t *_pBuf, uint8_t _ucLen);
void nRF905_ReceivePacket(uint8_t *_pBuf, uint8_t _ucLen);
uint8_t nRF905_ConnectCheck(void);

#endif  

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
