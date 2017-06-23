

#include "App.h"


#define	DEBUG_USART	USART1

int fputc(int ch, FILE *f)
{
	USART_SendData(DEBUG_USART, (unsigned char) ch);
	while (!(DEBUG_USART->SR & USART_FLAG_TXE));
	return (ch);
}

unsigned char buf[] = "hello world \r\n";
unsigned char rBuf[16];
GPIO_InitTypeDef  GPIO_InitStructure;
	unsigned char ch = 0;
void main()
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x3000); 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	SysTick_Config(72);	// sysTick  1Mhz

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	while(1)
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_2,(BitAction)1);
		Delay_us(1000000);
		GPIO_WriteBit(GPIOA,GPIO_Pin_2,(BitAction)0);
		Delay_us(1000000);
	}
}

