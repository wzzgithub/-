#include "iap.h"
#include "CRC.h"
#include "LCD.h"
#include "delay.h"
#include "timer.h"

iapfun jump;

#define USART_BUF_SIZE 	1024+8
#define DATA_SIZE				 1024

uint8_t usart_buf[USART_BUF_SIZE] = {0};
uint16_t buf_cnt = 0;

// data_len_L  data_len_H  data(no more than 1K)   index_L index_H CRC

// ��addr����¼����

void Iap_Write(uint32_t addr)
{
	uint16_t temp = 0;
	uint16_t data_size = 0;																		//��¼��С
	uint16_t data_len = 0;																		//���ݳ���
	uint16_t index = 0;																			//���ݿ�����
	uint32_t crc_val = 0;
	uint32_t addr_now = addr;																//д���ַ
	uint8_t data_write[DATA_SIZE] = {0};														//���ݻ���
	
	//׼����¼�����ݴ�СΪ1K
	while(1)
	{
		Watch_Cnt=0;
		//�������
		while((buf_cnt & 0x8000) == 0);													//�ȴ����ݽ������
		//��������
		data_len = (uint16_t)usart_buf[1] << 8 | usart_buf[0];				//��ȡdata��Ч����
		for(temp = 2; temp < data_len + 2; temp++)								//�ӵڶ�λ��ʼ��������
		{
			data_write[temp - 2] = usart_buf[temp];
		}
		index = usart_buf[DATA_SIZE + 3] << 8 | usart_buf[DATA_SIZE + 2];			//��ȡ����
		crc_val =  usart_buf[DATA_SIZE + 7] << 24 | usart_buf[DATA_SIZE + 6] << 16 | 	//��ȡCRCֵ
						  usart_buf[DATA_SIZE + 5] << 8 | usart_buf[DATA_SIZE + 4] ;
		
		addr_now = addr + 1024 * index;		//׼��д���ַ
		//ʹ�� USART_BUF_SIZE / 4 �޷��õ�����
		temp = USART_BUF_SIZE ;
		temp /= 4;
		if(!CRC_GetVal((uint32_t *)usart_buf, temp, crc_val))
		{
			printf("Again\n");
			buf_cnt = 0;
			continue;
		}
		//��ʼд����
		if(data_len < 1024)																		//д��ʣ�µ�����
		{
			if(data_len % 2 != 0)data_len += 1;
			STMFLASH_Write(addr_now, (uint16_t *)data_write, data_len / 2);		//
			data_size += 1;
			STMFLASH_Write(IAP_INFO, &data_len, 1);
			printf("OK\n");
			break;
		}
		else
		{
			STMFLASH_Write(addr_now, (uint16_t*)data_write, data_len / 2);
			data_size += 1;
			
			buf_cnt = 0;
			for(temp = 0; temp < USART_BUF_SIZE; temp++)usart_buf[temp] = 0;
			LCD_DISPLAY(80,80,"���չ̼��������������Ե�...");		//LCD��ʾ
			printf("Next\n");
		}
	}		//��¼���
	
	//��մ��ڻ���
	buf_cnt = 0;
	for(temp = 0; temp < USART_BUF_SIZE; temp++)usart_buf[temp] = 0;
}


//����ջ����ַ
//addr:ջ����ַ
//__asm void MSR_MSP(u32 addr) 
//{
//    MSR MSP, r0 			//set Main Stack value
//    BX r14
//}

void Iap_load(uint32_t addr)
{
	if(((*(vu32*)addr) & 0x2FFE0000) == 0x20000000)		
	{
		jump = (iapfun) *(vu32*)(addr + 4);					//ǿ��ת��Ϊ����
		MSR_MSP(*(vu32*)addr);
		jump();
	}
	else 
	{

		printf("Error\n");

		while(1);
	}
}

//�յ� :updata; ����������

uint8_t Iap_wait(void)
{
	uint8_t temp = 0;
	static uint8_t flag = 0;
	while (USART_GetFlagStatus(UART4, USART_IT_RXNE) != RESET)
	{
		temp = (uint8_t)USART_ReceiveData(UART4);
		
		switch(flag)
		{
			case 0: if(temp == ':')flag = 1;break;
			case 1:
			case 2: 																				
			case 3:																				
			case 4:																				
			case 5:																				
			case 6: if(temp != ';')flag++;	  break;								
			case 7: if(temp == ';')flag = 8;break; 							
			default:break;
		}
		if(flag == 8)break;
	}
	return flag;
}


