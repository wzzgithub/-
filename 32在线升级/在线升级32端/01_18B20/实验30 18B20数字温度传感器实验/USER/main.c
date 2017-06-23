#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "ds18b20.h" 
 
/************************************************
 ALIENTEKս��STM32������ʵ��30
 DS18B20�����¶ȴ����� ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/


 int main(void)
 {	 
	u8 t=0;	
	short temperature;	 
	 NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x3000);	
  

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ 115200
	 DS18B20_Init();
	while(1)
	{	    	 
printf("1111");		
 		if(t%10==0)			//ÿ100ms��ȡһ��
		{	
			temperature=DS18B20_Get_Temp();
			
			printf("%d.%d\n",temperature/10,temperature%10);		   
		}	
		delay_ms(100);
			t++;
			if(t==20)
				t=0;
	}
}

