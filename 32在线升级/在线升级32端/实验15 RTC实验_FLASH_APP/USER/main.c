#include "led.h"
#include "delay.h"
#include "sys.h"

#include "usart.h"	
//#include "usmart.h"	 
#include "rtc.h" 
 
/************************************************
 ALIENTEK战舰STM32开发板实验15
 RTC实时时钟实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

 int main(void)
 {	
	 	u8 t=0;
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x3000);	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	 
 
	 
	//SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */
	 
	 
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
 	LED_Init();			     //LED端口初始化		 	
	//usmart_dev.init(SystemCoreClock/1000000);	//初始化USMART	
	RTC_Init();	  			//RTC初始化
	
	
	while(1)
	{				
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			printf("%d\n",calendar.sec);	
			LED0=!LED0;
			LED1=!LED1;
		}	
		delay_ms(10);								  
	};  
 }

