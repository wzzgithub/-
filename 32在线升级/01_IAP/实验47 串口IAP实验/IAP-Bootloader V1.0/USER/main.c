#include "delay.h"
#include "key.h"
#include "sys.h"

#include "usart.h"
#include "stmflash.h"
#include "iap.h"
 
 
/************************************************
 ALIENTEK战舰STM32开发板实验47
 IAP实验 Bootloader V1.0 代码 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


int main(void)
{		
	u8 t;
	//u8 key;
	u16 oldcount=0;				//老的串口接收数据值
	u16 applenth=0;				//接收到的app代码长度
	u8 clearflag=0;  

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	uart_init(9600);	//串口初始化为115200
	delay_init();	   	 	//延时初始化 
	while(1)
	{
	 	if(USART_RX_CNT)
		{
			if(oldcount==USART_RX_CNT)//新周期内,没有收到任何数据,认为本次数据接收完成.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("用户程序接收完成!\r\n");
				printf("代码长度:%dBytes\r\n",applenth);
				clearflag=1;
				
			}
			else 
				oldcount=USART_RX_CNT;
		}
			t++;
			delay_ms(10000);
			if(t==30)
			{
				t=0;
				iap_load_app(FLASH_APP1_ADDR);
			}
	
		if(clearflag==1)
		{
					clearflag=0;
					printf("开始更新固件...\r\n");	
					if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
					{	 
						iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//更新FLASH代码   
						printf("固件更新完成!\r\n");	
					}
					delay_ms(1000);
					
					if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
					{	 
						printf("开始执行FLASH用户代码!!\r\n");
						iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
					}
		}
				
	}   	   
}






