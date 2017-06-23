#include "delay.h"
#include "key.h"
#include "sys.h"

#include "usart.h"
#include "stmflash.h"
#include "iap.h"
 
 
/************************************************
 ALIENTEKս��STM32������ʵ��47
 IAPʵ�� Bootloader V1.0 ���� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/


int main(void)
{		
	u8 t;
	//u8 key;
	u16 oldcount=0;				//�ϵĴ��ڽ�������ֵ
	u16 applenth=0;				//���յ���app���볤��
	u8 clearflag=0;  

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	uart_init(9600);	//���ڳ�ʼ��Ϊ115200
	delay_init();	   	 	//��ʱ��ʼ�� 
	while(1)
	{
	 	if(USART_RX_CNT)
		{
			if(oldcount==USART_RX_CNT)//��������,û���յ��κ�����,��Ϊ�������ݽ������.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("�û�����������!\r\n");
				printf("���볤��:%dBytes\r\n",applenth);
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
					printf("��ʼ���¹̼�...\r\n");	
					if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
					{	 
						iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//����FLASH����   
						printf("�̼��������!\r\n");	
					}
					delay_ms(1000);
					
					if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
					{	 
						printf("��ʼִ��FLASH�û�����!!\r\n");
						iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
					}
		}
				
	}   	   
}






