/*
*********************************************************************************************************
*	                                  
*	模块名称 : nRF905驱动模块
*	文件名称 : bsp_nrf905.c
*	版    本 : V1.0
*	说    明 : nRF905驱动
*	修改记录 :
*		版本号    日期          作者          说明
*		v1.0    2015-02-28     Eric2013	      首版
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"


/*
	nRF905模块可以直接插到STM32-V5开发板nRF905模块的排母接口上。

    nRF905模块   STM32F407开发板
	  VCC   ------  3.3V
	  GND   ------  GND
      SCK   ------  PB3/SPI3_SCK
      MISO  ------  PB4/SPI3_MISO
      MOSI  ------  PB5/SPI3_MOSI
	  TRX_CE -----  PA5
	  AM    ------  PB9
	  DR    ------  PB8
	  CD    ------  PI7
	  PWR_UP -----  PB6
	  TX_EN ------  PA4
      CSN   ------  PC2
*/

/* SPI接口的SCK MOSI MISO端口定义 */
#define nRF905_SPI_USING    SPI1
#define nRF905_RCC_SPI  	RCC_APB2Periph_SPI1
#define nRF905_PORT_SPI		GPIOB
#define nRF905_PIN_SCK		GPIO_Pin_3
#define nRF905_PIN_MISO		GPIO_Pin_4
#define nRF905_PIN_MOSI		GPIO_Pin_5

#define nRF905_PinSource_SCK	 GPIO_PinSource3
#define nRF905_PinSource_MISO	 GPIO_PinSource4
#define nRF905_PinSource_MOSI	 GPIO_PinSource5

#define nRF905_AF_SPI    GPIO_AF_SPI1

/* GPIO相关引脚配置 */
#define nRF905_RCC_ALL  		(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOI)

#define nRF905_TX_EN_PORT       GPIOA
#define nRF905_TX_EN_PIN		GPIO_Pin_4		/*** PA4 TX_EN ***/

#define nRF905_CE_PORT          GPIOA
#define nRF905_CE_PIN  			GPIO_Pin_5      /*** PA5 TRX_CE***/

#define nRF905_PWR_UP_PORT		GPIOB     
#define nRF905_PWR_UP_PIN		GPIO_Pin_6      /*** PB6 PWR_UP ***/

#define nRF905_CSN_PORT			GPIOC
#define nRF905_CSN_PIN			GPIO_Pin_2		/*** PC2 CSN ***/ 

#define nRF905_CD_PORT			GPIOI
#define nRF905_CD_PIN			GPIO_Pin_7		/*** PI7 CD ***/

#define nRF905_AM_PORT			GPIOB
#define nRF905_AM_PIN 			GPIO_Pin_9      /*** PB9 AM ***/

/* 中断引脚 */
#define nRF905_DR_PORT			GPIOB
#define nRF905_DR_PIN			GPIO_Pin_8		/*** PB8 DR ***/
#define nRF905_EXIT_IRQn	    EXTI9_5_IRQn
#define nRF905_EXIT_Line	    EXTI_Line8	
#define nRF905_EXIT_PortSourceGPIO	    EXTI_PortSourceGPIOB
#define nRF905_EXTI_PinSource    	    EXTI_PinSource8	

/* PWR_UP 引脚 */
#define nRF905_POWER_ON()       GPIOB->BSRRL = nRF905_PWR_UP_PIN
#define nRF905_POWER_OFF()      GPIOB->BSRRH  = nRF905_PWR_UP_PIN

/* 片选引脚 */
#define nRF905SE_CSN_1() 	 	GPIOC->BSRRL = nRF905_CSN_PIN     
#define nRF905SE_CSN_0()   		GPIOC->BSRRH =  nRF905_CSN_PIN

/* CE引脚 */
#define nRF905_CE_1()     			GPIOA->BSRRL = nRF905_CE_PIN
#define nRF905_CE_0()    		    GPIOA->BSRRH =  nRF905_CE_PIN
#define nRF905_Standby()			nRF905_CE_0()
#define nRF905_SHOCKBURST_TRX()		nRF905_CE_1()

/* TX_EN引脚 */
#define nRF905_TX_EN_ON()           GPIOA->BSRRL = nRF905_TX_EN_PIN
#define nRF905_TX_EN_OFF()          GPIOA->BSRRH =  nRF905_TX_EN_PIN
#define nRF905_MODE_TX()	        nRF905_TX_EN_ON()
#define nRF905_MODE_RX()	        nRF905_TX_EN_OFF()

#define nRF905_IRQ_Read()           (GPIOB->IDR & nRF905_DR_PIN)

/*
	由于V5开发板本身的问题，下面的引脚一定要配置，防止TFT屏上的spi flash影响nRF24L01的通信
	配置TFT接口中的PWM脚为为推挽输出模式：
	PWM = 1  这个模式支持STM32读写RA8875外挂的串行Flash
	PWM = 0 这是正常工作模式，由RA8875 DMA读取外挂的串行Flash

	这里要选择将PWM设置为1。
*/
#define RA8875_PORT_PWM	    GPIOF
#define RA8875_PIN_PWM	    GPIO_Pin_6

/************************ nRF905相关寄存器配置 ***************/
typedef struct 
{
   /* 配置寄存器长度 */
   uint8_t nRF905_ConfigLength;
	
   /* 配置寄存器 */  
   uint8_t nRF905_ConfigBuf[10];

}nRF905_InitTypeDef;


uint8_t nRF905_ConfigBuf[11] =
{
  10,                               /* 配置命令 数组的长度 */
  0x4c,                             /* CH_NO,配置频段在430MHZ */
  0x0c,                             /* 输出功率为10db,不重发，节电为正常模式 */
  0x44,                             /* 地址宽度设置为4字节 */
  0x20,0x20,                        /* 接收发送有效数据长度为32字节 */
  0xCC,0xCC,0xCC,0xCC,              /* 接收地址 */
  0x58,                             /* CRC充许，8位CRC校验，外部时钟信号不使能，16M晶振 */
};

/* 变量 **********************************************************/
uint8_t *nRF905_RxBuffer;
uint8_t  nRF905_RxBufferSize;

uint8_t nRF905_TxAddress[nRF905_AddressSize] = {0x80, 0x81, 0x82, 0x83}; /* 发送地址 */
uint8_t nRF905_RxAddress[nRF905_AddressSize] = {0x31, 0x32, 0x33, 0x30}; /* 本机地址 */

__IO uint8_t nRF905_DRPinFlag = 0; /* 发送和接收中断，没有进入中断时值为0，进入后值置为1 */
uint8_t g_ucNRF905TxRxMode = 0;    /* nRF905工作在接收模式值为0，工作在发送模式值为1 */

/* 文件内部函数 **************************************************/
static void nRF905_SetTxMode(void);
static void nRF905_SetRxMode(void);
static void nRF905_WriteBuf(uint8_t _ucCmd, uint8_t *_pBuf, uint8_t _ucLen);
static void nRF905_ReadBuf(uint8_t _ucCmd, uint8_t *_pBuf, uint8_t _ucLen);

/*
*********************************************************************************************************
*	函 数 名: nRF905_CfgSpiHard
*	功能说明: 配置STM32内部SPI硬件的工作模式、速度等参数，用于访问nRF905。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void nRF905_CfgSpiHard(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;

	/*
		PB5/SPI1_MISO <== MISO
		PB4/SPI1_MOSI ==> MOSI
		PB3/SPI1_SCK  ==> SCLK
		PC2           ==> CSN     (片选)
   */

	/* 打开相关模块的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/* 配置 SCK, MISO , MOSI 为复用功能 */
	/* 使能SPI1时钟 */
	RCC_APB2PeriphClockCmd(nRF905_RCC_SPI, ENABLE);
	
	GPIO_PinAFConfig(nRF905_PORT_SPI, nRF905_PinSource_SCK,  nRF905_AF_SPI);
	GPIO_PinAFConfig(nRF905_PORT_SPI, nRF905_PinSource_MISO, nRF905_AF_SPI);
	GPIO_PinAFConfig(nRF905_PORT_SPI, nRF905_PinSource_MOSI, nRF905_AF_SPI);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = nRF905_PIN_SCK | nRF905_PIN_MISO | nRF905_PIN_MOSI;
	GPIO_Init(nRF905_PORT_SPI, &GPIO_InitStructure);
	
	/* SPI 配置(对应nrf905的spi协议) */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/* 选择2线全双工模式 */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						/* CPU的SPI作为主设备 */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					/* 8个数据 */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;							/* CLK引脚空闲状态电平 = 0 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						/* 数据采样在第1个边沿(上升沿) */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  							/* 软件控制片选 */
	
	/*
	STM32F4XX 时钟计算.
		HCLK = 168M
		PCLK1 = HCLK / 4 = 42M
		PCLK2 = HCLK / 2 = 84M

		SPI2、SPI3 在 PCLK1, 时钟42M
		SPI1       在 PCLK2, 时钟84M

		STM32F4 支持的最大SPI时钟为 37.5 Mbits/S, 因此需要分频。下面使用的是SPI1。
		设置波特率预分频系数 84MHz / 8 = 10.5MHz，NRF905支持的最大SPI时钟为10MHz
	*/
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					/* 最高位先传输 */
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(nRF905_SPI_USING,&SPI_InitStructure);

	/* SPI 使能 */
	SPI_Cmd(nRF905_SPI_USING,ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: nRF905_InitHard
*	功能说明: SPI配置
*	形    参：无
*	返 回 值: 无		                   			  
*********************************************************************************************************
*/
void nRF905_InitHard(void)
{
	nRF905_InitTypeDef *nRF905_InitStructurePtr;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	
	/**********************第1步：配置GPIO ********************************/
	/* 时钟使能 **/
	RCC_AHB1PeriphClockCmd(nRF905_RCC_ALL, ENABLE);

	/*  nRF905 CE, Tx_EN, PWR_UP引脚配置  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = nRF905_CE_PIN;
	GPIO_Init(nRF905_CE_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = nRF905_TX_EN_PIN;
	GPIO_Init(nRF905_TX_EN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = nRF905_PWR_UP_PIN;
	GPIO_Init(nRF905_PWR_UP_PORT, &GPIO_InitStructure);

	/*  nRF905 CSN 引脚配置  */
	GPIO_InitStructure.GPIO_Pin = nRF905_CSN_PIN;
	GPIO_Init(nRF905_CSN_PORT, &GPIO_InitStructure);
	
	/*
	  由于V5开发板本身的问题，下面的引脚一定要配置，防止TFT屏上的spi flash影响nRF24L01的通信
	  配置TFT接口中的PWM脚为为推挽输出模式：
	  PWM = 1  这个模式支持STM32读写RA8875外挂的串行Flash
	  PWM = 0 这是正常工作模式，由RA8875 DMA读取外挂的串行Flash
	  
	  这里要选择将PWM设置为1。
	*/
	GPIO_InitStructure.GPIO_Pin = RA8875_PIN_PWM;
	GPIO_Init(RA8875_PORT_PWM, &GPIO_InitStructure);
	GPIO_SetBits(RA8875_PORT_PWM, RA8875_PIN_PWM);

	/*  nRF905 CD, AM 引脚配置  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 		
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_InitStructure.GPIO_Pin = nRF905_AM_PIN ;	
	GPIO_Init(nRF905_AM_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = nRF905_CD_PIN;	
	GPIO_Init(nRF905_CD_PORT, &GPIO_InitStructure);

	/*  nRF905 DR引脚配置  */
	GPIO_InitStructure.GPIO_Pin = nRF905_DR_PIN;
	GPIO_Init(nRF905_DR_PORT, &GPIO_InitStructure);	
	
	
	/**********************第2步：配置SPI接口 ********************************/
	nRF905_CfgSpiHard();
	

	/**********************第3步：配置外部中断NVIC ****************************/
	/* 使能通道中断 */
	NVIC_InitStructure.NVIC_IRQChannel = nRF905_EXIT_IRQn;    
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/**********************第4步：配置外部中断 *******************************/
	/* 使能SYSCFG时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* 连接 EXTI Line8 到 PB8 引脚 */
	SYSCFG_EXTILineConfig(nRF905_EXIT_PortSourceGPIO, nRF905_EXTI_PinSource);

	/* 配置 EXTI LineXXX */
	EXTI_InitStructure.EXTI_Line = nRF905_EXIT_Line;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	/**********************第5步：初始化nRF905 *******************************/
	nRF905_POWER_ON();
	
	nRF905_InitStructurePtr = (nRF905_InitTypeDef *) nRF905_ConfigBuf;
	
	/* 初始化nFR905的相关寄存器 */
	nRF905_WriteBuf(nRF905_WC, nRF905_InitStructurePtr->nRF905_ConfigBuf,
							   nRF905_InitStructurePtr->nRF905_ConfigLength);
	
	/**********************第6步：设置nRF905工作模式 ***************************/
	/* 开机默认工作在接收模式 */
	nRF905_SetTxAddress(nRF905_RxAddress, nRF905_AddressSize);//设置发送地址
	nRF905_SetRxAddress(nRF905_TxAddress, nRF905_AddressSize);//设置本机地址
	
	/**********************第7步：开机默认工作在接收模式 ***********************/
	g_ucNRF905TxRxMode = 0;
	nRF905_SetRxMode();

}

/*
*********************************************************************************************************
*	函 数 名: nRF905_SetTxMode
*	功能说明: 将NRF905模块设置为发送模式.
*             在空闲模式时把发送数据和发送地址写入相应寄存器，然后设置发送模式
*	形    参：无
*	返 回 值: 无		                   			  
*********************************************************************************************************
*/
static void nRF905_SetTxMode(void)
{
	nRF905_MODE_TX();	
	nRF905_SHOCKBURST_TRX();
}

/*
*********************************************************************************************************
*	函 数 名: nRF905_SetRxMode
*	功能说明: 将NRF905模块设置为接收模式
*	形    参：无
*	返 回 值: 无		                   			  
*********************************************************************************************************
*/
static void nRF905_SetRxMode(void)
{
	nRF905_MODE_RX();	
	nRF905_SHOCKBURST_TRX();
	/* CE_ON至少持续650us，650us后nRF905开始检测空中信息 */
}

/*
*********************************************************************************************************
*	函 数 名: nRF905_WriteReadByte
*	功能说明: 用于向NRF读/写一字节数据
*	形    参: 写入的数据
*	返 回 值: 读取得的数据		        
*********************************************************************************************************
*/
static uint8_t nRF905_WriteReadByte(uint8_t dat)
{  	
	/* 当SPI发送缓冲器非空时等待 */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	
	/* 通过SPI发送一字节数据 */
	SPI_I2S_SendData(SPI1, dat);		
	
	/* 当SPI接收缓冲器为空时等待 */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* 通过SPI接收一字节数据 */
	return SPI_I2S_ReceiveData(SPI1);
}

/*
*********************************************************************************************************
*	函 数 名: nRF905_ReadBuf
*	功能说明: 用于nRF905的寄存器中读出一串数据
*	形    参: _ucRegAddr: nRF905的命令+寄存器地址。
 		      _pBuf：     数据缓冲地址。
		      _ucLen:     _pBuf据长度
*	返 回 值: 无	        
*********************************************************************************************************
*/
static void nRF905_ReadBuf(uint8_t _ucCmd, uint8_t *_pBuf, uint8_t _ucLen)
{
	uint8_t i;
	
	/* 进入空闲模式操作SPI， 空闲模式或者掉电模式才能操作SPI */
	nRF905_Standby();
	
	nRF905SE_CSN_0();
	
	/* 发送命令 */
	nRF905_WriteReadByte(_ucCmd);

	/* 读取数据 */
	for(i = 0; i < _ucLen; i++) 
	{
		_pBuf[i] = nRF905_WriteReadByte(NOP);
	}
	
	nRF905SE_CSN_1();
}

/*
*********************************************************************************************************
*	函 数 名: nRF905_WriteBuf
*	功能说明: 用于向nRF905的寄存器中写入一串数据
*	形    参：_ucRegAddr: nRF905的命令+寄存器地址。
 		      _pBuf：     数据缓冲地址。
		      _ucLen:     _pBuf的数据长度。
*	返 回 值: 无	        
*********************************************************************************************************
*/
static void nRF905_WriteBuf(uint8_t _ucCmd, uint8_t *_pBuf, uint8_t _ucLen)
{
	uint8_t i;
	
	/* 进入空闲模式操作SPI， 空闲模式或者掉电模式才能操作SPI */
	nRF905_Standby();
	
	nRF905SE_CSN_0();
	
	/* 发送命令 */
	nRF905_WriteReadByte(_ucCmd);

	/* 写数据 */
	for(i=0; i<_ucLen; i++) 
	{
		nRF905_WriteReadByte(*_pBuf++);
	}

	nRF905SE_CSN_1();	
}

/*
*********************************************************************************************************
*	函 数 名: nRF905_SetTxAddress
*	功能说明: 写发送地址
*	形    参: _pBuf：     数据缓冲地址。
*		      _ucLen:     _pBuf的数据长度。
*	返 回 值: 无	        
*********************************************************************************************************
*/
void nRF905_SetTxAddress(uint8_t *_pBuf, uint8_t _ucLen)
{
    nRF905_WriteBuf(nRF905_WTA, _pBuf, _ucLen);	
}

/*
*********************************************************************************************************
*	函 数 名: nRF905_SetTxAddress
*	功能说明: 写本机地址
*	形    参: _pBuf：     数据缓冲地址。
*		      _ucLen:     _pBuf的数据长度。
*	返 回 值: 无	        
*********************************************************************************************************
*/
void nRF905_SetRxAddress(uint8_t *_pBuf, uint8_t _ucLen)
{
    nRF905_WriteBuf(nRF905_WRA, _pBuf, _ucLen);		
}

/*
*********************************************************************************************************
*	函 数 名: nRF905_SendPacket
*	功能说明: 发送指定数据，首先设置要发送的数据，然后发送数据
*	形    参: _pBuf：     数据缓冲地址。
*		      _ucLen:     _pBuf的数据长度。
*	返 回 值: 无	        
*********************************************************************************************************
*/
void nRF905_SendPacket(uint8_t *_pBuf, uint8_t _ucLen)
{
	/* 设置要发送的数据 */
	nRF905_WriteBuf(nRF905_WTP, _pBuf, _ucLen);	
	
	/*
	   首先将NRF905模块设置发送模式，等待模块将发送数据寄存器的数据发送完成，
       然后清除发送完成标志，最后设置成空闲模式(在中断里面设置成空闲模式)
	*/
	nRF905_SetTxMode();
	
	/* 这里设置成阻塞方式，不设置超时等待，防止还没有发送完成，再次添加发送数据 */
	while(nRF905_DRPinFlag == 0);

	/* 数据发送完成，将标志再设置为0 */
	DISABLE_INT();  			/* 关中断 */
	nRF905_DRPinFlag = 0;
	ENABLE_INT();  				/* 开中断 */	
}

/*
*********************************************************************************************************
*	函 数 名: nRF905_ReceivePacket
*	功能说明: 接收数据，首先等待模块接收数据完成，然后从模块接收数据寄存器中读取接收到的数据
*	形    参: _pBuf：     数据缓冲地址。
*		      _ucLen:     _pBuf的数据长度。
*	返 回 值: 无	        
*********************************************************************************************************
*/
void nRF905_ReceivePacket(uint8_t *_pBuf, uint8_t _ucLen)
{	
	/*
		首先将NRF905模块设置接收模式，等待接收完成标志，
        然后清除接收完成标志，最后设置成空闲模式(在中断里面读取数据并设置成空闲模式)
	*/
	nRF905_SetRxMode();
	nRF905_RxBuffer= _pBuf;
	nRF905_RxBufferSize = _ucLen;
}

/*
*********************************************************************************************************
*	函 数 名: nRF905_ConnectCheck
*	功能说明: 主要用于nRF905_与MCU是否正常连接
*	形    参: 无
*	返 回 值: 1 表示连接正常，0 表示连接不正常	        
*********************************************************************************************************
*/
uint8_t nRF905_ConnectCheck(void)
{
	uint8_t ucBuf[nRF905_AddressSize];

	/*读出写入的本机地址 */
	nRF905_ReadBuf(nRF905_RRA, ucBuf, nRF905_AddressSize); 
	 
	/* 用于调试目的 */
//	printf("ucBuf[0] = %d, RX_ADDRESS[0] = %d\r\n", ucBuf[0], RX_ADDRESS[0]);
//	printf("ucBuf[1] = %d, RX_ADDRESS[1] = %d\r\n", ucBuf[1], RX_ADDRESS[1]);
//	printf("ucBuf[2] = %d, RX_ADDRESS[2] = %d\r\n", ucBuf[2], RX_ADDRESS[2]);
//	printf("ucBuf[3] = %d, RX_ADDRESS[3] = %d\r\n", ucBuf[3], RX_ADDRESS[3]);
//	printf("ucBuf[4] = %d, RX_ADDRESS[4] = %d\r\n", ucBuf[4], RX_ADDRESS[4]);
	
	/* 比较写入的数据和读出的是否相同 */
	if(strncmp((char *)nRF905_TxAddress, (char *)ucBuf, nRF905_AddressSize) == 0)
	{
		return 1;  /* 相同 */
	}
	else
	{
		return 0;  /* 不相同 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: nRF905_EXTI_ISR
*	功能说明: 发送和接收中断
*	形    参: 无
*	返 回 值: 无	        
*********************************************************************************************************
*/
void nRF905_EXTI_ISR(void) 
{
	/* 设置进入中断标志 */
	nRF905_DRPinFlag = 1;
	
	/* 进入空闲模式 */
	nRF905_Standby();
	
	/* 如果是工作在接收模式进入中断，将接收到数据读取出来 */
	if(g_ucNRF905TxRxMode == 0)
	{
		nRF905_ReadBuf(nRF905_RRP, nRF905_RxBuffer, nRF905_RxBufferSize);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
