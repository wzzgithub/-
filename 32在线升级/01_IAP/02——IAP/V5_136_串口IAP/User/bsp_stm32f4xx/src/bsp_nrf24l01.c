/*
*********************************************************************************************************
*	                                  
*	模块名称 : nRF24L01驱动模块
*	文件名称 : bsp_nrf24l01.c
*	版    本 : V1.0
*	说    明 : nRF24L01驱动
*	修改记录 :
*		版本号    日期          作者          说明
*		v1.0    2015-02-12     Eric2013	      首版
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"

/*
	nRF24L01模块可以直接插到STM32-V5开发板nRF24L01模块的排母接口上。

    nRF24L01模块   STM32F407开发板
	  VCC   ------  3.3V
	  GND   ------  GND
      SCK   ------  PB3/SPI3_SCK
      MISO  ------  PB4/SPI3_MISO
      MOSI  ------  PB5/SPI3_MOSI
      CSN   ------  PF7/NRF24L01_CSN
      IRQ   ------  PH7/NRF24L01_IRQ
      CE    ------  PA4/NRF905_TX_EN/NRF24L01_CE/DAC1_OUT
*/

/* 定义GPIO端口 */
#define nRF24L01_RCC_SPI  RCC_APB2Periph_SPI1
#define nRF24L01_RCC_ALL  (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOH)

#define nRF24L01_PORT_CE   GPIOA
#define nRF24L01_PIN_CE    GPIO_Pin_4

#define nRF24L01_PORT_CSN  GPIOF
#define nRF24L01_PIN_CSN   GPIO_Pin_7

#define nRF24L01_PORT_IRQ  GPIOH
#define nRF24L01_PIN_IRQ   GPIO_Pin_7

/* SPI接口的SCK MOSI MISO端口定义 */
#define nRF24L01_PORT_SPI	GPIOB
#define nRF24L01_PIN_SCK	GPIO_Pin_3
#define nRF24L01_PIN_MISO	GPIO_Pin_4
#define nRF24L01_PIN_MOSI	GPIO_Pin_5

#define nRF24L01_PinSource_SCK	 GPIO_PinSource3
#define nRF24L01_PinSource_MISO	 GPIO_PinSource4
#define nRF24L01_PinSource_MOSI	 GPIO_PinSource5

#define nRF24L01_AF_SPI    GPIO_AF_SPI1

/*
	由于V5开发板本身的问题，下面的引脚一定要配置，防止TFT屏上的spi flash影响nRF24L01的通信
	配置TFT接口中的PWM脚为为推挽输出模式：
	PWM = 1  这个模式支持STM32读写RA8875外挂的串行Flash
	PWM = 0 这是正常工作模式，由RA8875 DMA读取外挂的串行Flash

	这里要选择将PWM设置为1。
*/
#define RA8875_PORT_PWM	    GPIOF
#define RA8875_PIN_PWM	    GPIO_Pin_6

/* 引脚操作，采用寄存器的形式，加快操作速度 */
#define nRF24L01_CSN_1()    nRF24L01_PORT_CSN->BSRRL = nRF24L01_PIN_CSN
#define nRF24L01_CSN_0()    nRF24L01_PORT_CSN->BSRRH = nRF24L01_PIN_CSN

#define nRF24L01_CE_1()     nRF24L01_PORT_CE->BSRRL = nRF24L01_PIN_CE
#define nRF24L01_CE_0()     nRF24L01_PORT_CE->BSRRH = nRF24L01_PIN_CE

#define nRF24L01_IRQ_Read()	  (nRF24L01_PORT_IRQ->IDR & nRF24L01_PIN_IRQ)

/* 发射端和接收端地址 */
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};  
uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};

/*
*********************************************************************************************************
*	函 数 名: nRF24L01_CfgSpiHard
*	功能说明: 配置STM32内部SPI硬件的工作模式、速度等参数，用于访问nRF24L01。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void nRF24L01_CfgSpiHard(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	
	/*
	STM32F4XX 时钟计算.
		HCLK = 168M
		PCLK1 = HCLK / 4 = 42M
		PCLK2 = HCLK / 2 = 84M

		SPI2、SPI3 在 PCLK1, 时钟42M
		SPI1       在 PCLK2, 时钟84M

		STM32F4 支持的最大SPI时钟为 37.5 Mbits/S, 因此需要分频。下面使用的是SPI1。
	*/
	/* 配置SPI硬件参数 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/* 数据方向：2线全双工 */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		/* STM32的SPI工作模式 ：主机模式 */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* 数据位长度 ： 8位 */
	/* SPI_CPOL和SPI_CPHA结合使用决定时钟和数据采样点的相位关系、
	   本例配置: 总线空闲是低电平,第1个边沿采样数据。
	*/
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			/* 片选控制方式：软件控制 */

	/* 设置波特率预分频系数 84MHz / 16 = 5.25MHz，24L01支持的最大SPI时钟为10MHz
	   这里选择SPI_BaudRatePrescaler_8容易出错。
	*/
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/* 数据位传输次序：高位先传 */
	SPI_InitStructure.SPI_CRCPolynomial = 7;			/* CRC多项式寄存器，复位后为7。本例程不用 */
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);				/* 使能SPI  */
}

/*
*********************************************************************************************************
*	函 数 名: nRF24L01_InitHard
*	功能说明: SPI配置
*	形    参：无
*	返 回 值: 无		                   			  
*********************************************************************************************************
*/
void nRF24L01_InitHard(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*使能GPIO时钟 */
	RCC_AHB1PeriphClockCmd(nRF24L01_RCC_ALL, ENABLE);
	
	/* 配置nRF24L01的CE引脚 */
	/* 配置为推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = nRF24L01_PIN_CE;
	GPIO_Init(nRF24L01_PORT_CE, &GPIO_InitStructure);
	
	/* 配置nRF24L01的 CSN 引脚 */
	/* 配置为推挽输出模式 */
	GPIO_InitStructure.GPIO_Pin = nRF24L01_PIN_CSN;
	GPIO_Init(nRF24L01_PORT_CSN, &GPIO_InitStructure);
	
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
	
	/* 配置nRF24L01的 IRQ 引脚 */
	/* 配置为输入带上拉 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 		
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 	
	GPIO_InitStructure.GPIO_Pin = nRF24L01_PIN_IRQ; 
	GPIO_Init(nRF24L01_PORT_IRQ, &GPIO_InitStructure);
	
	/* 配置 SCK, MISO , MOSI 为复用功能 */
	/* 使能SPI1时钟 */
	RCC_APB2PeriphClockCmd(nRF24L01_RCC_SPI, ENABLE);
	GPIO_PinAFConfig(nRF24L01_PORT_SPI, nRF24L01_PinSource_SCK,  nRF24L01_AF_SPI);
	GPIO_PinAFConfig(nRF24L01_PORT_SPI, nRF24L01_PinSource_MISO, nRF24L01_AF_SPI);
	GPIO_PinAFConfig(nRF24L01_PORT_SPI, nRF24L01_PinSource_MOSI, nRF24L01_AF_SPI);

	/* 注意GPIO速度最好不好超过25MHz，要不容易出错 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = nRF24L01_PIN_SCK | nRF24L01_PIN_MISO | nRF24L01_PIN_MOSI;
	GPIO_Init(nRF24L01_PORT_SPI, &GPIO_InitStructure);
	
	/* 配置 SPI1工作模式 */
	nRF24L01_CfgSpiHard();
	
	/* 用于拉高csn引脚，NRF进入空闲状态 */
	nRF24L01_CSN_1();  
} 

/*
*********************************************************************************************************
*	函 数 名: nRF24L01_WriteReadByte
*	功能说明: 用于向NRF读/写一字节数据
*	形    参: 写入的数据
*	返 回 值: 读取得的数据		        
*********************************************************************************************************
*/
static uint8_t nRF24L01_WriteReadByte(uint8_t dat)
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
*	函 数 名: nRF24L01_WriteReg
*	功能说明: 用于向nRF24L01特定的寄存器写入数据
*	形    参: _ucRegAddr: nRF24L01的命令+寄存器地址。
 		      _usValue:   将要向寄存器写入的数据
*	返 回 值: nRF24L01的status寄存器的状态		        
*********************************************************************************************************
*/
static uint8_t nRF24L01_WriteReg(uint8_t _ucRegAddr, uint8_t _usValue)
{
	uint8_t ucStatus;
	
	nRF24L01_CE_0();
	
	/* 置低CSN，使能SPI传输 */
	nRF24L01_CSN_0();
				
	/* 发送命令及寄存器号 */
	ucStatus = nRF24L01_WriteReadByte(_ucRegAddr);
		 
	 /* 向寄存器写入数据 */
	nRF24L01_WriteReadByte(_usValue); 
	          
	/* CSN拉高，完成 */	   
	nRF24L01_CSN_1();	
		
	/* 返回状态寄存器的值 */
	return(ucStatus);
}

/*
*********************************************************************************************************
*	函 数 名: nRF24L01_ReadReg
*	功能说明: 用于从nRF24L01特定的寄存器读出数据
*	形    参: reg: nRF24L01的命令+寄存器地址
*	返 回 值: 寄存器中的数据		        
*********************************************************************************************************
*/
static uint8_t nRF24L01_ReadReg(uint8_t _ucRegAddr)
{
 	uint8_t usValue;

	nRF24L01_CE_0();
	
	/* 置低CSN，使能SPI传输 */
 	nRF24L01_CSN_0();
				
  	 /* 发送寄存器号 */
	nRF24L01_WriteReadByte(_ucRegAddr); 

	 /* 读取寄存器的值 */
	usValue = nRF24L01_WriteReadByte(NOP);
	            
   	/*CSN拉高，完成*/
	nRF24L01_CSN_1();		
   	
	return usValue;
}	

/*
*********************************************************************************************************
*	函 数 名: nRF24L01_ReadBuf
*	功能说明: 用于从nRF24L01的寄存器中读出一串数据
*	形    参: _ucRegAddr: nRF24L01的命令+寄存器地址。
 		      _pBuf：     数据缓冲地址。
		      _ucLen:     _pBuf据长度
*	返 回 值: nRF24L01的status寄存器的状态		        
*********************************************************************************************************
*/
static uint8_t nRF24L01_ReadBuf(uint8_t _ucRegAddr, uint8_t *_pBuf, uint8_t _ucLen)
{
 	uint8_t ucStatus, i;

	nRF24L01_CE_0();
	
	/* 置低CSN，使能SPI传输 */
	nRF24L01_CSN_0();
		
	/*发送寄存器号*/		
	ucStatus = nRF24L01_WriteReadByte(_ucRegAddr); 

 	/*读取缓冲区数据*/
	for(i = 0; i < _ucLen; i++)
	{
		_pBuf[i] = nRF24L01_WriteReadByte(NOP);
	}
	    
	/* CSN拉高，完成 */
	nRF24L01_CSN_1();	
		
 	return ucStatus;	
}

/*
*********************************************************************************************************
*	函 数 名: nRF24L01_WriteBuf
*	功能说明: 用于向nRF24L01的寄存器中写入一串数据
*	形    参：_ucRegAddr: nRF24L01的命令+寄存器地址。
 		      _pBuf：     数据缓冲地址。
		      _ucLen:     _pBuf的数据长度。
*	返 回 值: NRF的status寄存器的状态。	        
*********************************************************************************************************
*/
static uint8_t nRF24L01_WriteBuf(uint8_t _ucRegAddr, uint8_t *_pBuf, uint8_t _ucLen)
{
	 uint8_t ucStatus,i;
	
	 nRF24L01_CE_0();
	
   	 /*置低CSN，使能SPI传输*/
	 nRF24L01_CSN_0();			

	 /*发送寄存器号*/	
  	 ucStatus = nRF24L01_WriteReadByte(_ucRegAddr); 
 	
  	  /*向缓冲区写入数据*/
	 for(i = 0; i < _ucLen; i++)
	{
		nRF24L01_WriteReadByte(*_pBuf++);
	}
	  	   
	/*CSN拉高，完成*/
	nRF24L01_CSN_1();			
  
  	return (ucStatus);	
}

/*
*********************************************************************************************************
*	函 数 名: nRF24L01_RxMode
*	功能说明: 设置nRF24L01工作在接收模式
*	形    参: 无
*	返 回 值: 无		        
*********************************************************************************************************
*/
void nRF24L01_RxMode(void)
{
	nRF24L01_CE_0();
	nRF24L01_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH);  /* 写RX节点地址 */
	nRF24L01_WriteReg(NRF_WRITE_REG+EN_AA, 0x01);               			/* 使能通道0的自动应答 */    
	nRF24L01_WriteReg(NRF_WRITE_REG+EN_RXADDR, 0x01);           			/* 使能通道0的接收地址 */    
	nRF24L01_WriteReg(NRF_WRITE_REG+RF_CH, CHANAL);             			/* 设置RF通信频率 */    
	nRF24L01_WriteReg(NRF_WRITE_REG+RX_PW_P0, RX_PLOAD_WIDTH);  			/* 选择通道0的有效数据宽度 */      
	nRF24L01_WriteReg(NRF_WRITE_REG+RF_SETUP, 0x0f); /* 设置TX发射参数,0db增益,2Mbps,低噪声增益开启 */   
	nRF24L01_WriteReg(NRF_WRITE_REG+CONFIG, 0x0f);   /* 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 */ 
	
	/*CE拉高，进入接收模式*/	
	nRF24L01_CE_1(); 
}    

/*
*********************************************************************************************************
*	函 数 名: nRF24L01_TxMode
*	功能说明: 设置nRF24L01工作在发送模式
*	形    参: 无
*	返 回 值: 无		        
*********************************************************************************************************
*/
void nRF24L01_TxMode(void)
{  
	nRF24L01_CE_0();
	nRF24L01_WriteBuf(NRF_WRITE_REG+TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    /* 写TX节点地址 */ 
	nRF24L01_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); /* 设置TX节点地址,主要为了使能ACK */   
	nRF24L01_WriteReg(NRF_WRITE_REG+EN_AA, 0x01);     /* 使能通道0的自动应答 */    	
	nRF24L01_WriteReg(NRF_WRITE_REG+EN_RXADDR, 0x01); /* 使能通道0的接收地址 */  	
	nRF24L01_WriteReg(NRF_WRITE_REG+SETUP_RETR, 0x0f);/* 设置自动重发间隔时间:250us + 86us;最大自动重发次数:10次 */	
	nRF24L01_WriteReg(NRF_WRITE_REG+RF_CH, CHANAL);   /* 设置RF通道为CHANAL */	
	nRF24L01_WriteReg(NRF_WRITE_REG+RF_SETUP, 0x0f);  /* 设置TX发射参数,0db增益,2Mbps,低噪声增益开启 */   	
	nRF24L01_WriteReg(NRF_WRITE_REG+CONFIG, 0x0e);    /* 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,发射模式,开启所有中断 */	
	
	/*CE拉高，进入发送模式*/	
	nRF24L01_CE_1();
	bsp_DelayUS(100);  /* CE要拉高一段时间才进入发送模式，时间大于10us */
}

/*
*********************************************************************************************************
*	函 数 名: nRF24L01_ConnectCheck
*	功能说明: 主要用于nRF24L01_与MCU是否正常连接
*	形    参: 无
*	返 回 值: 1 表示连接正常，0 表示连接不正常	        
*********************************************************************************************************
*/
uint8_t nRF24L01_ConnectCheck(void)
{
	uint8_t ucBuf[TX_ADR_WIDTH];

	/*写入TX_ADR_WIDTH个字节的地址.  */  
	nRF24L01_WriteBuf(NRF_WRITE_REG+TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);

	/*读出写入的地址 */
	nRF24L01_ReadBuf(TX_ADDR, ucBuf, TX_ADR_WIDTH); 
	 
	/* 用于调试目的 */
//	printf("ucBuf[0] = %d, TX_ADDRESS[0] = %d\r\n", ucBuf[0], TX_ADDRESS[0]);
//	printf("ucBuf[1] = %d, TX_ADDRESS[1] = %d\r\n", ucBuf[1], TX_ADDRESS[1]);
//	printf("ucBuf[2] = %d, TX_ADDRESS[2] = %d\r\n", ucBuf[2], TX_ADDRESS[2]);
//	printf("ucBuf[3] = %d, TX_ADDRESS[3] = %d\r\n", ucBuf[3], TX_ADDRESS[3]);
//	printf("ucBuf[4] = %d, TX_ADDRESS[4] = %d\r\n", ucBuf[4], TX_ADDRESS[4]);
	
	/* 比较写入的数据和读出的是否相同 */
	if(strncmp((char *)TX_ADDRESS, (char *)ucBuf, TX_ADR_WIDTH) == 0)
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
*	函 数 名: nRF24L01_TxData
*	功能说明: 用于向nRF24L01的发送缓冲区中写入数据
*	形    参: _pTxBuf 数据缓冲地址
*	返 回 值: 发送结果，成功返回TX_DS, 失败返回MAX_RT或0		        
*********************************************************************************************************
*/
uint8_t nRF24L01_TxData(uint8_t *_pTxBuf)
{
	uint8_t ucState;
	uint32_t ulCount = 0;   

	/*CE为低，进入待机模式1*/
	nRF24L01_CE_0();

	/*写数据到_pTxBuf最大32个字节*/						
	nRF24L01_WriteBuf(WR_TX_PLOAD, _pTxBuf, TX_PLOAD_WIDTH);

	/*CE为高，_pTxBuf非空，发送数据包 */   
	nRF24L01_CE_1();
	  	
	/*
	  等待发送完成中断                            
   	  while(nRF24L01_IRQ_Read()!=0); 
      选择下面这种方式可以有效防止发送数据阻塞时，死在这个状态 
	*/
	while((nRF24L01_IRQ_Read() != 0) && (ulCount < 65535))
	{
		ulCount++;
	}
	
	if(ulCount >= 65535) 
	{
		return 0;		
	}
	
	/* 读取状态寄存器的值 */                              
	ucState = nRF24L01_ReadReg(STATUS);

	/*清除TX_DS或MAX_RT中断标志*/                  
	nRF24L01_WriteReg(NRF_WRITE_REG+STATUS, ucState); 	

	nRF24L01_WriteReg(FLUSH_TX,NOP);      /* 清除TX FIFO寄存器 */ 

	 /*判断中断类型*/ 
	/* 达到最大重发次数 */
	if(ucState & MAX_RT)                     
	{
		return MAX_RT;
	}
	/* 发送完成 */
	else if(ucState & TX_DS)
	{
		return TX_DS;
	} 
	/* 其他原因发送失败 */	
	else
	{
		return 0;               
	}
} 

/*
*********************************************************************************************************
*	函 数 名: nRF24L01_RxData
*	功能说明: 用于从nRF24L01的接收缓冲区中读出数据
*	形    参: _pRxBuf 数据缓冲地址
*	返 回 值: 接收结果，成功返回RX_DS, 失败返回0		        
*********************************************************************************************************
*/
uint8_t nRF24L01_RxData(uint8_t *_pRxBuf)
{
	uint8_t ucState;
	uint32_t ulCount = 0;  

	nRF24L01_CE_1();	  /* 进入接收状态 */
	
	/*
	  等待接收中断
	  while(nRF24L01_IRQ_Read()!=0);
	  选择下面这种方式可以有效防止发送数据阻塞时，死在这个状态 
	*/
	while((nRF24L01_IRQ_Read() != 0) && (ulCount < 65535))
	{
		ulCount++;
	}
	
	if(ulCount >= 65535) 
	{
		return 0;		
	}
	
	nRF24L01_CE_0();  	 /* 进入待机状态 */

	/*读取status寄存器的值  */               
	ucState=nRF24L01_ReadReg(STATUS);
	 
	/* 清除中断标志*/      
	nRF24L01_WriteReg(NRF_WRITE_REG+STATUS, ucState);

	/*判断是否接收到数据*/
	if(ucState & RX_DR)    /* 接收到数据 */
	{
		nRF24L01_ReadBuf(RD_RX_PLOAD, _pRxBuf, RX_PLOAD_WIDTH); /* 读取数据 */
		nRF24L01_WriteReg(FLUSH_RX, NOP);                       /* 清除RX FIFO寄存器 */
		return RX_DR; 
	}
	else 
    {
		return 0;   /* 没收到任何数据 */
	}			
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
