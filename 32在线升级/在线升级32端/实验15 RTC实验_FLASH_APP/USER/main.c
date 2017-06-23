#include "led.h"
#include "delay.h"
#include "sys.h"

#include "usart.h"	
//#include "usmart.h"	 
#include "rtc.h" 
 
/************************************************
 ALIENTEKս��STM32������ʵ��15
 RTCʵʱʱ��ʵ��  
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

 int main(void)
 {	
	 	u8 t=0;
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x3000);	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	 
 
	 
	//SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */
	 
	 
	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��		 	
	//usmart_dev.init(SystemCoreClock/1000000);	//��ʼ��USMART	
	RTC_Init();	  			//RTC��ʼ��
	
	
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

