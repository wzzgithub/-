#ifndef _BSP_H_
#define _BSP_H_

#include "stm32f10x.h"   
#include "stm32f10x_rcc.h"    //SysTick 定时器相关


#include "LCD.h"


/*---Zigbee---*/
#define ZIGBEE_RUN GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)//RUN
#define ZIGBEE_NET GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)//NET
#define ZIGBEE_ALM GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5)//ALM


#define TF_ON           GPIOC->BSRR = GPIO_Pin_8	 
#define TF_OFF          GPIOC->BRR  = GPIO_Pin_8

#define ZIGBEE_ON         GPIOC->BSRR = GPIO_Pin_7	 
#define ZIGBEE_OFF        GPIOC->BRR  = GPIO_Pin_7

#define ZIGBEE_SLP_OFF       GPIOD->BSRR = GPIO_Pin_15	 
#define ZIGBEE_SLP_ON        GPIOD->BRR  = GPIO_Pin_15

#define LCD_ON            GPIOD->BSRR = GPIO_Pin_0
#define LCD_OFF           GPIOD->BRR = GPIO_Pin_0
/*---LED---*/
#define GREEN_LED_ON        GPIOB->BSRR = GPIO_Pin_1	 
#define GREEN_LED_OFF		GPIOB->BRR  = GPIO_Pin_1

#define RED_LED_ON         	GPIOB->BSRR = GPIO_Pin_0	 
#define RED_LED_OFF        	GPIOB->BRR  = GPIO_Pin_0 

#define MAIN_LED_ON			GPIOE->BSRR = GPIO_Pin_0	 
#define MAIN_LED_OFF        GPIOE->BRR  = GPIO_Pin_0

#define LED0 PEout(0)// PE0

#define SENSOR_LED_ON		GPIOE->BSRR = GPIO_Pin_1	 
#define SENSOR_LED_OFF      GPIOE->BRR  = GPIO_Pin_1

#define ADC1_DR_Address    ((u32)0x4001244C)

extern struct tm Atime;
 
extern void BSPInit(void);

extern void GPIO_Configuration(void);



#endif
