#include "bsp_spi.h"


/***********************************************************************
**
*	@Function:	初始化SPI输入输出口，用作软件模拟功能
*	@Param:		None
*	@Return: 	None
*	@Modify:	None
**
************************************************************************/
void SPI_SoftwareConfiguration(void)
{
//	SPI_InitTypeDef  SPI_InitStructure;
//	GPIO_InitTypeDef GPIO_InitStructure;

//	/* Enable SPI1 and GPIO clocks */
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
//	/*!< Configure pins: SCK */
//	GPIO_InitStructure.GPIO_Pin = SPI_PIN_CLK;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(SPI_PORT_CLK, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = SPI_PIN_MOSI;
//	GPIO_Init(SPI_PORT_MOSI, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = SPI_PIN_MISO;
//	GPIO_Init(SPI_PORT_MISO, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = SPI_PIN_CS_1;
//	GPIO_Init(SPI_PORT_CS_1, &GPIO_InitStructure);
}
/***********************************************************************
**
*	@Function:	初始化SPI输入输出口，用作软件模拟功能
*	@Param:		None
*	@Return: 	None
*	@Modify:	None
**
************************************************************************/
void SPI_SW_Configuration(void)
{	
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);	//	Enable GPIO clock
	GPIO_InitStructure.GPIO_Pin = SPI_SW_PIN_CLK_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_SW_PORT_CLK_1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI_SW_PIN_MOSI_1;
	GPIO_Init(SPI_SW_PORT_MOSI_1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI_SW_PIN_MISO_1;
	GPIO_Init(SPI_SW_PORT_MISO_1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI_SW_PIN_CS_1;
	GPIO_Init(SPI_SW_PORT_CS_1, &GPIO_InitStructure);
}
unsigned char  SPI_SW_ReadWriteByte(unsigned char byte)
{
	unsigned char i = 0;
	unsigned char ch = 0;
	SPI_CLK_HIGHT;
	for(i = 0; i < 8; i ++)
	{	
		SPI_CLK_HIGHT;
		//	MOSI
		if(byte & 0x80)
			SPI_MOSI_HIGHT;
		else
			SPI_MOSI_LOW;
		//	MISO
		if(READ_MISO)
			ch = ch + 1;
		else 
			ch = ch & 0xfe;
		SPI_CLK_LOW;
		byte = byte << 1;
		ch = ch << 1;
	}
	SPI_CLK_HIGHT;
	return ch;
}
/***********************************************************************
**
*	@Function:	SPI 软件模拟时序 写入1个字节
*	@Param:		ch		待写入的值
*	@Return: 	None
*	@Modify:	None
**
************************************************************************/
void SPI_SoftwareWriteByte(unsigned char ch)
{
	unsigned char count = 0;
	SPI_CLK_HIGHT;
	for(count = 0; count < 8; count ++)
	{	
		SPI_CLK_HIGHT;
		if(ch & 0x80)
			SPI_MOSI_HIGHT;
		else
			SPI_MOSI_LOW;
		SPI_CLK_LOW;
		ch = ch << 1;
	}
	SPI_CLK_HIGHT;
}
/***********************************************************************
**
*	@Function:	SPI 软件模拟时序 读取1个字节   // error   !!!!!!!!!
*	@Param:		None
*	@Return: 	ch : 读取的字节
*	@Modify:	2015年8月5日14:06:47
**
************************************************************************/
unsigned char SPI_SoftwareReadByte()
{
	unsigned char count = 0;
	unsigned char ch = 0;
	SPI_CLK_HIGHT;
	for(count = 0; count < 8; count ++)
	{	
		SPI_CLK_LOW;
		if(READ_MISO)
			ch = ch | 0x01;
		else
			ch = ch & 0xfe;		
		SPI_CLK_HIGHT;
		ch = ch << 1;
	}
	return ch;
}
/***********************************************************************
**
*	@Function:	SPI 硬件初始化配置
*	@Param:		None
*	@Return: 	None
*	@Modify:	None
**
************************************************************************/
void SPI_HW_Configuration(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);	//	Enable GpioB clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);	//	Enable SPI2 clock
	GPIO_InitStructure.GPIO_Pin = SPI_HW_PIN_CLK_1;		//	Configure pin CLK
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = SPI_HW_PIN_MISO_1;	//	Configure pin MISO
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = SPI_HW_PIN_MOSI_1;	//	Confgure pin MOSI
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = SPI_HW_PIN_CS_1;		//	Configure pin CS  (software control )
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
}
/***********************************************************************
**
*	@Function:	SPI 硬件发送或读取一个字节    (spi 时钟同时传输，时钟信号必须由主机发出，发送字节才能产生时钟
*	@Param:		
*		@arg	byte：	写的字节值
*	@Return: 	读字节值
*	@Modify:	None
**
************************************************************************/
unsigned char SPI_HW_ReadWriteByte(unsigned char  byte)
{ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)  == RESET);	// Check TX empty flag 
	SPI_I2S_SendData(SPI2, byte);	//	Send Byte 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI2);	//	Return Read Byte
}
unsigned char SPI_HW_SendByte(unsigned char  byte)
{ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)  == RESET);	// Check TX empty flag 
	SPI_I2S_SendData(SPI2, byte);	//	Send Byte 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI2);	//	Return Read Byte
}
/***********************************************************************
**
*	@Function:	SPI 硬件初始化配置
*	@Param:		None
*	@Return: 	None
*	@Modify:	None
**
************************************************************************/
void SPI_Configuration(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SPI1 and GPIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	/*!< Configure pins: SCK */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*!< Configure pins: MISO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*!< Configure pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*!< Configure CS pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	/* Enable SPI1  */
	SPI_Cmd(SPI2, ENABLE);
}

