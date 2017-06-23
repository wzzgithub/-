#ifndef 	__IAP_H
#define 	__IAP_H

#include "stm32f10x.h"
#include "usart.h"
#include "stmflash.h"


//ʹ��28K����IAP�ĳ���ռ䣬								
//ͬʱ����IAP�����1K�ռ����������Ϣ�洢
//����app������ʼ��ַΪ										
																		
#define	IAP_INFO														0x800FC00	//0800 FC00

#define	IAP_UPD_OK													0x800FCF0	//0800 FC00

#define 	APP_ADDR													0x8010000	//0801 0000

#define 	APP_ADDR2													0x8030000	//0801 0000

#define	IAP_WRITE()					Iap_Write(APP_ADDR)

#define	IAP_LOAD()					Iap_load(APP_ADDR)



typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.

void Iap_Write(uint32_t addr);
void Iap_load(uint32_t addr);

uint8_t Iap_wait(void);

#endif


