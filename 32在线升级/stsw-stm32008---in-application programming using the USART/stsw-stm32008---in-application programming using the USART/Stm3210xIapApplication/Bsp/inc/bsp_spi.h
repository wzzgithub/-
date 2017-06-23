#ifndef	_BSP_SPI_H__
#define	_BSP_SPI_H__

#include "stm32f10x.h"

//#define		trailing edge	


/*****************************************************
*	Hardware spi define~
*****************************************************/
#define		SPI_HW_PIN_CLK_1				GPIO_Pin_13
#define		SPI_HW_PIN_MOSI_1				GPIO_Pin_15
#define		SPI_HW_PIN_MISO_1				GPIO_Pin_14
#define		SPI_HW_PIN_CS_1					GPIO_Pin_12

#define		SPI_HW_PORT_CLK_1				GPIOB
#define		SPI_HW_PORT_MOSI_1				GPIOB
#define		SPI_HW_PORT_MISO_1				GPIOB
#define		SPI_HW_PORT_CS_1				GPIOB

/*****************************************************
*	Software spi define~
*****************************************************/
#define		SPI_SW_PIN_CLK_1				GPIO_Pin_13
#define		SPI_SW_PIN_MOSI_1				GPIO_Pin_15
#define		SPI_SW_PIN_MISO_1				GPIO_Pin_14
#define		SPI_SW_PIN_CS_1					GPIO_Pin_12

#define		SPI_SW_PORT_CLK_1				GPIOB
#define		SPI_SW_PORT_MOSI_1				GPIOB
#define		SPI_SW_PORT_MISO_1				GPIOB
#define		SPI_SW_PORT_CS_1				GPIOB

//------------------------------------------------------------



#define		SPI_CLK_HIGHT			GPIO_SetBits(SPI_SW_PORT_CLK_1,   SPI_SW_PIN_CLK_1 )
#define		SPI_CLK_LOW				GPIO_ResetBits(SPI_SW_PORT_CLK_1, SPI_SW_PIN_CLK_1 )
#define		SPI_MOSI_HIGHT			GPIO_SetBits(SPI_SW_PORT_MOSI_1,  SPI_SW_PIN_MOSI_1 )
#define		SPI_MOSI_LOW			GPIO_ResetBits(SPI_SW_PORT_MOSI_1,SPI_SW_PIN_MOSI_1 )
#define		SPI_MISO_HIGHT			GPIO_SetBits(SPI_SW_PORT_MISO_1,  SPI_SW_PIN_MISO_1 )
#define		SPI_MISO_LOW			GPIO_ResetBits(SPI_SW_PORT_MISO_1,SPI_SW_PIN_MISO_1 )
#define		SPI_CS_1_HIGHT			GPIO_SetBits(SPI_SW_PORT_CS_1,	  SPI_SW_PIN_CS_1 )
#define		SPI_CS_1_LOW			GPIO_ResetBits(SPI_SW_PORT_CS_1,  SPI_SW_PIN_CS_1 )

#define	READ_MISO			(GPIO_ReadInputDataBit(SPI_SW_PORT_MISO_1,SPI_SW_PIN_MISO_1) ? 1 : 0)

//void SPI_SoftwareWriteByte(unsigned char ch);
//unsigned char SPI_SoftwareReadByte(void);
//void SPI_SoftwareConfiguration(void);



//void SPI_Configuration(void);
//void SPI_HW_WriteByte(unsigned char ch);
//unsigned char SPI_HW_ReadByte(void);
//unsigned char SPI_HW_SendByte(unsigned char  byte);

/*************************************************************************************************
*	Software perpherial function 
*************************************************************************************************/
void SPI_SW_Configuration(void);
unsigned char  SPI_SW_ReadWriteByte(unsigned char byte); 
/*************************************************************************************************
*	Hardware perpherial function 
*************************************************************************************************/
void SPI_HW_Configuration(void);
unsigned char SPI_HW_ReadWriteByte(unsigned char byte);


#endif


