#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "ds18b20.h" 
 
/************************************************
 ALIENTEK战舰STM32开发板实验30
 DS18B20数字温度传感器 实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


 int main(void)
 {	 
	u8 t=0;	
	short temperature;	 
	 NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x3000);	
  

	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为 115200
	 DS18B20_Init();
	while(1)
	{	    	 
printf("1111");		
 		if(t%10==0)			//每100ms读取一次
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

