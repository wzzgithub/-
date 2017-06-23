#ifndef 	__IAP_H
#define 	__IAP_H

#include "stm32f10x.h"
#include "usart.h"
#include "stmflash.h"


//使用28K用作IAP的程序空间，								
//同时，将IAP的最后1K空间用作相关信息存储
//所以app程序起始地址为										
																		
#define	IAP_INFO														0x800FC00	//0800 FC00

#define	IAP_UPD_OK													0x800FCF0	//0800 FC00

#define 	APP_ADDR													0x8010000	//0801 0000

#define 	APP_ADDR2													0x8030000	//0801 0000

#define	IAP_WRITE()					Iap_Write(APP_ADDR)

#define	IAP_LOAD()					Iap_load(APP_ADDR)



typedef  void (*iapfun)(void);				//定义一个函数类型的参数.

void Iap_Write(uint32_t addr);
void Iap_load(uint32_t addr);

uint8_t Iap_wait(void);

#endif


