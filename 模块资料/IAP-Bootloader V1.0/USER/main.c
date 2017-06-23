#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "stmflash.h"
#include "BSP.h"
#include "iap.h"
#include "crc.h"
#include "softreset.h"
#include "timer.h"

//
//		远程IAP bootloader V4.0
//		程序设计：马天瑞
//		

#define BootTime 20  //等待BootTime/2秒后进入APP

void Delay(u32 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=12;  //自己定义
      while(i--) ;    
   }
}

int main(void)
{		 
//	uint8_t i;
	uint8_t update_flag = 0;	//升级标志
	uint8_t test_flag = 0;	//第一次更新测试标志位
	
//	for(i=0;i<100;i++)
//	Delay(5000);
//	RCC_DeInit();
//	NVIC_DeInit();
	
	delay_init();
	BSPInit();
	NVIC_Configuration(); 
	CRC_Config();				
	
		   	 
	uart1_init(9600);
	uart5_init(9600);		//LCD
	uart4_init(115200);	//DTU
	
	
	TIM3_Int_Init(4999,7199);//10Khz的计数频率，计数到5000为500ms  
	TIM4_Int_Init(4999,7199);//TIM3为BootTime TIM4为WatchTime（time.c定义）

	delay_ms(5000);	

	LCD_DISPLAY(80,60,"远程固件升级  bootloader V4.0");
	LCD_DISPLAY(80,80,"等待升级命令，10秒后进入APP...");
	printf("***************************************\r\n");
	printf("重启成功，进入Bootloader...\r\n");	
	delay_ms(5000);	
	delay_ms(5000);	
	delay_ms(5000);	
	printf("等待升级命令，20秒后进入APP...\r\n");
	printf("请打开上位机，准备更新程序 ...\r\n");
	printf("***************************************\r\n");
	delay_ms(5000);
	
	

	while(1)
	{

		
			while((Tick_Cnt<BootTime) && (update_flag == 0))			//以轮询的方式读取串口(readBootTime() != 0)  && 
		{
			if(Iap_wait() == 8)
			{
				USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
				update_flag = 1;
				LCD_DISPLAY(80,80,"收到升级命令，等待固件程序...");
				printf("收到升级命令，等待固件程序\r\n");
				TIM_Cmd(TIM4, ENABLE);	//开定时器监视			
				STMFLASH_WriteHalfWord(IAP_UPD_OK, 0xff);	//取消升级成功标志
			}
		}
		
		if(update_flag)
		{
			IAP_WRITE();
			update_flag = 0;	//更新完成
			test_flag = 1;		//开启测试标志位
		}
		else 
		{
			TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE );//关闭定时器中断，防止跳转失败
			TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE );
			LCD_CLEAR_LENGTH(80,60,100);	//清屏
			LCD_CLEAR_LENGTH(80,80,100);
			if((STMFLASH_ReadHalfWord(IAP_UPD_OK) == 0x55) || test_flag == 1)
			{
				test_flag = 0;
				Iap_load(APP_ADDR);		//启动升级程序
			}
			else
				Iap_load(APP_ADDR2);	//启动备用程序
		}
	}

}


