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
//		Զ��IAP bootloader V4.0
//		������ƣ�������
//		

#define BootTime 20  //�ȴ�BootTime/2������APP

void Delay(u32 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=12;  //�Լ�����
      while(i--) ;    
   }
}

int main(void)
{		 
//	uint8_t i;
	uint8_t update_flag = 0;	//������־
	uint8_t test_flag = 0;	//��һ�θ��²��Ա�־λ
	
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
	
	
	TIM3_Int_Init(4999,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms  
	TIM4_Int_Init(4999,7199);//TIM3ΪBootTime TIM4ΪWatchTime��time.c���壩

	delay_ms(5000);	

	LCD_DISPLAY(80,60,"Զ�̹̼�����  bootloader V4.0");
	LCD_DISPLAY(80,80,"�ȴ��������10������APP...");
	printf("***************************************\r\n");
	printf("�����ɹ�������Bootloader...\r\n");	
	delay_ms(5000);	
	delay_ms(5000);	
	delay_ms(5000);	
	printf("�ȴ��������20������APP...\r\n");
	printf("�����λ����׼�����³��� ...\r\n");
	printf("***************************************\r\n");
	delay_ms(5000);
	
	

	while(1)
	{

		
			while((Tick_Cnt<BootTime) && (update_flag == 0))			//����ѯ�ķ�ʽ��ȡ����(readBootTime() != 0)  && 
		{
			if(Iap_wait() == 8)
			{
				USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
				update_flag = 1;
				LCD_DISPLAY(80,80,"�յ���������ȴ��̼�����...");
				printf("�յ���������ȴ��̼�����\r\n");
				TIM_Cmd(TIM4, ENABLE);	//����ʱ������			
				STMFLASH_WriteHalfWord(IAP_UPD_OK, 0xff);	//ȡ�������ɹ���־
			}
		}
		
		if(update_flag)
		{
			IAP_WRITE();
			update_flag = 0;	//�������
			test_flag = 1;		//�������Ա�־λ
		}
		else 
		{
			TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE );//�رն�ʱ���жϣ���ֹ��תʧ��
			TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE );
			LCD_CLEAR_LENGTH(80,60,100);	//����
			LCD_CLEAR_LENGTH(80,80,100);
			if((STMFLASH_ReadHalfWord(IAP_UPD_OK) == 0x55) || test_flag == 1)
			{
				test_flag = 0;
				Iap_load(APP_ADDR);		//������������
			}
			else
				Iap_load(APP_ADDR2);	//�������ó���
		}
	}

}


