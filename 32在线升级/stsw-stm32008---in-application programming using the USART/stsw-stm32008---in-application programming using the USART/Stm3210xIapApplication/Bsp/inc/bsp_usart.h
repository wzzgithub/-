#ifndef	_USART_H__
#define	_USART_H__

#include "stm32f10x.h"




#define	USART_BAUDRATE	115200

#define USART_PIN_TX	GPIO_Pin_2	
#define USART_PIN_RX	GPIO_Pin_3
#define	USART_PORT		GPIOA


#define	SERIAL_NO		USART2	



#define	USART_RCC		RCC_APB1Periph_USART2
#define	USART_PORT_RCC	RCC_APB2Periph_GPIOA
#define	USART_IRQ		USART2_IRQn



void USART2_Config(void);
void USART_NVIC_Configuration(void);
void InitUsart(void);

#endif
