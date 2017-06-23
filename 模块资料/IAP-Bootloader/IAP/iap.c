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

// 从addr起烧录程序

void Iap_Write(uint32_t addr)
{
	uint16_t temp = 0;
	uint16_t data_size = 0;																		//烧录大小
	uint16_t data_len = 0;																		//数据长度
	uint16_t index = 0;																			//数据块索引
	uint32_t crc_val = 0;
	uint32_t addr_now = addr;																//写入地址
	uint8_t data_write[DATA_SIZE] = {0};														//数据缓冲
	
	//准备烧录，数据大小为1K
	while(1)
	{
		Watch_Cnt=0;
		//清除监视
		while((buf_cnt & 0x8000) == 0);													//等待数据接收完毕
		//解析数据
		data_len = (uint16_t)usart_buf[1] << 8 | usart_buf[0];				//获取data有效长度
		for(temp = 2; temp < data_len + 2; temp++)								//从第二位开始拷贝数据
		{
			data_write[temp - 2] = usart_buf[temp];
		}
		index = usart_buf[DATA_SIZE + 3] << 8 | usart_buf[DATA_SIZE + 2];			//获取索引
		crc_val =  usart_buf[DATA_SIZE + 7] << 24 | usart_buf[DATA_SIZE + 6] << 16 | 	//获取CRC值
						  usart_buf[DATA_SIZE + 5] << 8 | usart_buf[DATA_SIZE + 4] ;
		
		addr_now = addr + 1024 * index;		//准备写入地址
		//使用 USART_BUF_SIZE / 4 无法得到长度
		temp = USART_BUF_SIZE ;
		temp /= 4;
		if(!CRC_GetVal((uint32_t *)usart_buf, temp, crc_val))
		{
			printf("Again\n");
			buf_cnt = 0;
			continue;
		}
		//开始写数据
		if(data_len < 1024)																		//写入剩下的数据
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
			LCD_DISPLAY(80,80,"接收固件程序并升级，请稍等...");		//LCD显示
			printf("Next\n");
		}
	}		//烧录完成
	
	//清空串口缓存
	buf_cnt = 0;
	for(temp = 0; temp < USART_BUF_SIZE; temp++)usart_buf[temp] = 0;
}


//设置栈顶地址
//addr:栈顶地址
//__asm void MSR_MSP(u32 addr) 
//{
//    MSR MSP, r0 			//set Main Stack value
//    BX r14
//}

void Iap_load(uint32_t addr)
{
	if(((*(vu32*)addr) & 0x2FFE0000) == 0x20000000)		
	{
		jump = (iapfun) *(vu32*)(addr + 4);					//强制转化为函数
		MSR_MSP(*(vu32*)addr);
		jump();
	}
	else 
	{

		printf("Error\n");

		while(1);
	}
}

//收到 :updata; 则启动更新

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


