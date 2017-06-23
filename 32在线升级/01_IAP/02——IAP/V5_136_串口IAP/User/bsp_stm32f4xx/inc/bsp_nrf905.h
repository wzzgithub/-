/*
*********************************************************************************************************
*	                                  
*	ģ������ : nRF905����ģ��
*	�ļ����� : bsp_nrf905.c
*	��    �� : V1.0
*	˵    �� : nRF905����
*	�޸ļ�¼ :
*		�汾��    ����          ����          ˵��
*		v1.0    2015-02-28     Eric2013	      �װ�
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_NRF905_H_
#define __BSP_NRF905_H_

#include "stm32f4xx.h"

#define nRF905_BufferSize 	32
#define nRF905_AddressSize   4
#define NOP                0xFF						   /* SPI ȫ˫��ʱ�Ŀղ��� */

extern uint8_t nRF905_TxAddress[nRF905_AddressSize];   /* ���͵�ַ�������� */
extern uint8_t nRF905_RxAddress[nRF905_AddressSize];   /* ���յ�ַ�������� */

/********************* nRF905 SPI�ӿ� ����ָ�� �궨�� *****************************************/

#define nRF905_WC 		0x00        // �������üĴ��� (write RF-Configeration Register)
#define nRF905_RC		0x10		// ��ȡ���üĴ��� (read  RF-Configeration Register)

#define nRF905_WAR		0x01		//�����Զ��ش� AUTO_RETRAN ģʽ (set:0x20\ restset:0x00)
#define nRF905_RAR		0x11		//��ȡ�Զ��ش� AUTO_RETRAN ģʽ (set:0x20\ restset:0x00)

#define nRF905_WTRX_AFW 0x02		// �����շ���ַ��ʽ�Ŀ�ȣ��ֽڸ�����(Bit[7]:û��; TX_AFW[2:0] Bit[3]:û��; RX_AFW[2:0] Ĭ��ֵ 0100_0100 : 0x44)
#define nRF905_RTRX_AFW 0x12        // ��ȡ�շ���ַ��ʽ�Ŀ�ȣ��ֽڸ�����(Bit[7]:û��; TX_AFW[2:0] Bit[3]:û��; RX_AFW[2:0] Ĭ��ֵ0100_0100 : 0x44 )

#define nRF905_WRX_PW	 0x03		//���ý������ݵ�rx-payload�Ĵ�������Ч���ݵĿ�� (Bit[7:6]û��RX_PWR[5:0] Ĭ��ֵ 0010_0000 : 0x20)
#define nRF905_RRX_PW	 0x13		//��ȡ�������ݵ�rx-payload�Ĵ�������Ч���ݵĿ�� (Bit[7:6]û��RX_PWR[5:0] Ĭ��ֵ 0010_0000 : 0x20)

#define nRF905_WTX_PW 	0x04		//���ý������ݵ�tx-payload�Ĵ�������Ч���ݵĿ�� (Bit[7:6]û��RX_PWR[5:0] Ĭ��ֵ 0010_0000 : 0x20)	
#define nRF905_RTX_PW 	0x14		//��ȡ�������ݵ�tx-payload�Ĵ�������Ч���ݵĿ�� (Bit[7:6]û��RX_PWR[5:0] Ĭ��ֵ 0010_0000 : 0x20)

#define nRF905_WRA		0x05		// ����λ�����üĴ�����5-8��4���ֽڵ�Rx-Address (write RX-Address)
#define nRF905_RRA		0x15		// ��ȡλ�����üĴ�����5-8��4���ֽڵ�Rx-Address	(read Rx-Address)

#define nRF905_WCRC 	0x09 		// ����CRC У���ģʽ
#define nRF905_RCRC 	0x19 		// ��ȡCRC У���ģʽ

#define nRF905_WTP		0x20		// ��TX-Payload �Ĵ���д����Ч���� (write the payload data to the Tx-payload register)
#define nRF905_RTP 		0x21		// ��Tx-payload �Ĵ����ж�����Ч���� read the payload data in the Tx-payload register)

#define nRF905_WTA 		0x22		// ��Tx-Address �Ĵ���д�뷢�͵�ַ����Ŀ���ַ(write the destination address in the Tx-Address register)
#define nRF905_RTA		0x23		// ��Tx-Address	�Ĵ�����ȡ���͵�ַ��(read the destination address from the Tx-Address register)

#define nRF905_RRP		0x24		// ��Tx-Payload �Ĵ����ж�ȡ���յ�����Ч����(read the payload data from the Rx-Payload register)


/* ���ⲿ�ļ����õĺ��� */
void nRF905_InitHard(void);
void nRF905_EXTI_DoIt(void);
void nRF905_SetTxAddress(uint8_t *_pBuf, uint8_t _ucLen);
void nRF905_SetRxAddress(uint8_t *_pBuf, uint8_t _ucLen);
void nRF905_SendPacket(uint8_t *_pBuf, uint8_t _ucLen);
void nRF905_ReceivePacket(uint8_t *_pBuf, uint8_t _ucLen);
uint8_t nRF905_ConnectCheck(void);

#endif  

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
