#include "bsp_usart.h"


void InitUsart( )
{
	USART2_Config();
	USART_NVIC_Configuration();
}
/**************************************************************************
|	USART2 
|	TX->PA2		RX->PA3
**************************************************************************/
void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(USART_PORT_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(USART_RCC,ENABLE);

	GPIO_InitStructure.GPIO_Pin = USART_PIN_TX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_PORT, &GPIO_InitStructure);    
	
	GPIO_InitStructure.GPIO_Pin = USART_PIN_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(SERIAL_NO, &USART_InitStructure);
	USART_ITConfig(SERIAL_NO, USART_IT_RXNE, ENABLE);
	USART_Cmd(SERIAL_NO, ENABLE);
}
void USART_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void USART2_IRQHandler(void)
{
//	unsigned char ch;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{ 	
	  //  c=USART1->DR;
		//ch = USART_ReceiveData(SERIAL_NO);
	  	//printf("%c",ch);    //将接受到的数据直接返回打印
		
	} 
	 USART_ClearITPendingBit(USART2,USART_IT_RXNE);
}
void USART1_IRQHandler(void)
{
//	unsigned char ch;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{ 	
	  //  c=USART1->DR;
		//ch = USART_ReceiveData(SERIAL_NO);
	  	//printf("%c",ch);    //将接受到的数据直接返回打印
		
	} 
	 USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}

