/*
*********************************************************************************************************
*	                                  
*	ģ������ : nRF905����ģ��
*	�ļ����� : bsp_nrf905.c
*	��    �� : V1.0
*	˵    �� : nRF905����
*	�޸ļ�¼ :
*		�汾��    ����          ����          ˵��
*		v1.0    2015-02-28     Eric2013	      �װ�
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"


/*
	nRF905ģ�����ֱ�Ӳ嵽STM32-V5������nRF905ģ�����ĸ�ӿ��ϡ�

    nRF905ģ��   STM32F407������
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

/* SPI�ӿڵ�SCK MOSI MISO�˿ڶ��� */
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

/* GPIO����������� */
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

/* �ж����� */
#define nRF905_DR_PORT			GPIOB
#define nRF905_DR_PIN			GPIO_Pin_8		/*** PB8 DR ***/
#define nRF905_EXIT_IRQn	    EXTI9_5_IRQn
#define nRF905_EXIT_Line	    EXTI_Line8	
#define nRF905_EXIT_PortSourceGPIO	    EXTI_PortSourceGPIOB
#define nRF905_EXTI_PinSource    	    EXTI_PinSource8	

/* PWR_UP ���� */
#define nRF905_POWER_ON()       GPIOB->BSRRL = nRF905_PWR_UP_PIN
#define nRF905_POWER_OFF()      GPIOB->BSRRH  = nRF905_PWR_UP_PIN

/* Ƭѡ���� */
#define nRF905SE_CSN_1() 	 	GPIOC->BSRRL = nRF905_CSN_PIN     
#define nRF905SE_CSN_0()   		GPIOC->BSRRH =  nRF905_CSN_PIN

/* CE���� */
#define nRF905_CE_1()     			GPIOA->BSRRL = nRF905_CE_PIN
#define nRF905_CE_0()    		    GPIOA->BSRRH =  nRF905_CE_PIN
#define nRF905_Standby()			nRF905_CE_0()
#define nRF905_SHOCKBURST_TRX()		nRF905_CE_1()

/* TX_EN���� */
#define nRF905_TX_EN_ON()           GPIOA->BSRRL = nRF905_TX_EN_PIN
#define nRF905_TX_EN_OFF()          GPIOA->BSRRH =  nRF905_TX_EN_PIN
#define nRF905_MODE_TX()	        nRF905_TX_EN_ON()
#define nRF905_MODE_RX()	        nRF905_TX_EN_OFF()

#define nRF905_IRQ_Read()           (GPIOB->IDR & nRF905_DR_PIN)

/*
	����V5�����屾������⣬���������һ��Ҫ���ã���ֹTFT���ϵ�spi flashӰ��nRF24L01��ͨ��
	����TFT�ӿ��е�PWM��ΪΪ�������ģʽ��
	PWM = 1  ���ģʽ֧��STM32��дRA8875��ҵĴ���Flash
	PWM = 0 ������������ģʽ����RA8875 DMA��ȡ��ҵĴ���Flash

	����Ҫѡ��PWM����Ϊ1��
*/
#define RA8875_PORT_PWM	    GPIOF
#define RA8875_PIN_PWM	    GPIO_Pin_6

/************************ nRF905��ؼĴ������� ***************/
typedef struct 
{
   /* ���üĴ������� */
   uint8_t nRF905_ConfigLength;
	
   /* ���üĴ��� */  
   uint8_t nRF905_ConfigBuf[10];

}nRF905_InitTypeDef;


uint8_t nRF905_ConfigBuf[11] =
{
  10,                               /* �������� ����ĳ��� */
  0x4c,                             /* CH_NO,����Ƶ����430MHZ */
  0x0c,                             /* �������Ϊ10db,���ط����ڵ�Ϊ����ģʽ */
  0x44,                             /* ��ַ�������Ϊ4�ֽ� */
  0x20,0x20,                        /* ���շ�����Ч���ݳ���Ϊ32�ֽ� */
  0xCC,0xCC,0xCC,0xCC,              /* ���յ�ַ */
  0x58,                             /* CRC����8λCRCУ�飬�ⲿʱ���źŲ�ʹ�ܣ�16M���� */
};

/* ���� **********************************************************/
uint8_t *nRF905_RxBuffer;
uint8_t  nRF905_RxBufferSize;

uint8_t nRF905_TxAddress[nRF905_AddressSize] = {0x80, 0x81, 0x82, 0x83}; /* ���͵�ַ */
uint8_t nRF905_RxAddress[nRF905_AddressSize] = {0x31, 0x32, 0x33, 0x30}; /* ������ַ */

__IO uint8_t nRF905_DRPinFlag = 0; /* ���ͺͽ����жϣ�û�н����ж�ʱֵΪ0�������ֵ��Ϊ1 */
uint8_t g_ucNRF905TxRxMode = 0;    /* nRF905�����ڽ���ģʽֵΪ0�������ڷ���ģʽֵΪ1 */

/* �ļ��ڲ����� **************************************************/
static void nRF905_SetTxMode(void);
static void nRF905_SetRxMode(void);
static void nRF905_WriteBuf(uint8_t _ucCmd, uint8_t *_pBuf, uint8_t _ucLen);
static void nRF905_ReadBuf(uint8_t _ucCmd, uint8_t *_pBuf, uint8_t _ucLen);

/*
*********************************************************************************************************
*	�� �� ��: nRF905_CfgSpiHard
*	����˵��: ����STM32�ڲ�SPIӲ���Ĺ���ģʽ���ٶȵȲ��������ڷ���nRF905��
*	��    ��: ��
*	�� �� ֵ: ��
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
		PC2           ==> CSN     (Ƭѡ)
   */

	/* �����ģ���ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/* ���� SCK, MISO , MOSI Ϊ���ù��� */
	/* ʹ��SPI1ʱ�� */
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
	
	/* SPI ����(��Ӧnrf905��spiЭ��) */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/* ѡ��2��ȫ˫��ģʽ */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						/* CPU��SPI��Ϊ���豸 */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					/* 8������ */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;							/* CLK���ſ���״̬��ƽ = 0 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						/* ���ݲ����ڵ�1������(������) */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  							/* �������Ƭѡ */
	
	/*
	STM32F4XX ʱ�Ӽ���.
		HCLK = 168M
		PCLK1 = HCLK / 4 = 42M
		PCLK2 = HCLK / 2 = 84M

		SPI2��SPI3 �� PCLK1, ʱ��42M
		SPI1       �� PCLK2, ʱ��84M

		STM32F4 ֧�ֵ����SPIʱ��Ϊ 37.5 Mbits/S, �����Ҫ��Ƶ������ʹ�õ���SPI1��
		���ò�����Ԥ��Ƶϵ�� 84MHz / 8 = 10.5MHz��NRF905֧�ֵ����SPIʱ��Ϊ10MHz
	*/
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					/* ���λ�ȴ��� */
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(nRF905_SPI_USING,&SPI_InitStructure);

	/* SPI ʹ�� */
	SPI_Cmd(nRF905_SPI_USING,ENABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_InitHard
*	����˵��: SPI����
*	��    �Σ���
*	�� �� ֵ: ��		                   			  
*********************************************************************************************************
*/
void nRF905_InitHard(void)
{
	nRF905_InitTypeDef *nRF905_InitStructurePtr;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	
	/**********************��1��������GPIO ********************************/
	/* ʱ��ʹ�� **/
	RCC_AHB1PeriphClockCmd(nRF905_RCC_ALL, ENABLE);

	/*  nRF905 CE, Tx_EN, PWR_UP��������  */
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

	/*  nRF905 CSN ��������  */
	GPIO_InitStructure.GPIO_Pin = nRF905_CSN_PIN;
	GPIO_Init(nRF905_CSN_PORT, &GPIO_InitStructure);
	
	/*
	  ����V5�����屾������⣬���������һ��Ҫ���ã���ֹTFT���ϵ�spi flashӰ��nRF24L01��ͨ��
	  ����TFT�ӿ��е�PWM��ΪΪ�������ģʽ��
	  PWM = 1  ���ģʽ֧��STM32��дRA8875��ҵĴ���Flash
	  PWM = 0 ������������ģʽ����RA8875 DMA��ȡ��ҵĴ���Flash
	  
	  ����Ҫѡ��PWM����Ϊ1��
	*/
	GPIO_InitStructure.GPIO_Pin = RA8875_PIN_PWM;
	GPIO_Init(RA8875_PORT_PWM, &GPIO_InitStructure);
	GPIO_SetBits(RA8875_PORT_PWM, RA8875_PIN_PWM);

	/*  nRF905 CD, AM ��������  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 		
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_InitStructure.GPIO_Pin = nRF905_AM_PIN ;	
	GPIO_Init(nRF905_AM_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = nRF905_CD_PIN;	
	GPIO_Init(nRF905_CD_PORT, &GPIO_InitStructure);

	/*  nRF905 DR��������  */
	GPIO_InitStructure.GPIO_Pin = nRF905_DR_PIN;
	GPIO_Init(nRF905_DR_PORT, &GPIO_InitStructure);	
	
	
	/**********************��2��������SPI�ӿ� ********************************/
	nRF905_CfgSpiHard();
	

	/**********************��3���������ⲿ�ж�NVIC ****************************/
	/* ʹ��ͨ���ж� */
	NVIC_InitStructure.NVIC_IRQChannel = nRF905_EXIT_IRQn;    
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/**********************��4���������ⲿ�ж� *******************************/
	/* ʹ��SYSCFGʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* ���� EXTI Line8 �� PB8 ���� */
	SYSCFG_EXTILineConfig(nRF905_EXIT_PortSourceGPIO, nRF905_EXTI_PinSource);

	/* ���� EXTI LineXXX */
	EXTI_InitStructure.EXTI_Line = nRF905_EXIT_Line;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	/**********************��5������ʼ��nRF905 *******************************/
	nRF905_POWER_ON();
	
	nRF905_InitStructurePtr = (nRF905_InitTypeDef *) nRF905_ConfigBuf;
	
	/* ��ʼ��nFR905����ؼĴ��� */
	nRF905_WriteBuf(nRF905_WC, nRF905_InitStructurePtr->nRF905_ConfigBuf,
							   nRF905_InitStructurePtr->nRF905_ConfigLength);
	
	/**********************��6��������nRF905����ģʽ ***************************/
	/* ����Ĭ�Ϲ����ڽ���ģʽ */
	nRF905_SetTxAddress(nRF905_RxAddress, nRF905_AddressSize);//���÷��͵�ַ
	nRF905_SetRxAddress(nRF905_TxAddress, nRF905_AddressSize);//���ñ�����ַ
	
	/**********************��7��������Ĭ�Ϲ����ڽ���ģʽ ***********************/
	g_ucNRF905TxRxMode = 0;
	nRF905_SetRxMode();

}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_SetTxMode
*	����˵��: ��NRF905ģ������Ϊ����ģʽ.
*             �ڿ���ģʽʱ�ѷ������ݺͷ��͵�ַд����Ӧ�Ĵ�����Ȼ�����÷���ģʽ
*	��    �Σ���
*	�� �� ֵ: ��		                   			  
*********************************************************************************************************
*/
static void nRF905_SetTxMode(void)
{
	nRF905_MODE_TX();	
	nRF905_SHOCKBURST_TRX();
}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_SetRxMode
*	����˵��: ��NRF905ģ������Ϊ����ģʽ
*	��    �Σ���
*	�� �� ֵ: ��		                   			  
*********************************************************************************************************
*/
static void nRF905_SetRxMode(void)
{
	nRF905_MODE_RX();	
	nRF905_SHOCKBURST_TRX();
	/* CE_ON���ٳ���650us��650us��nRF905��ʼ��������Ϣ */
}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_WriteReadByte
*	����˵��: ������NRF��/дһ�ֽ�����
*	��    ��: д�������
*	�� �� ֵ: ��ȡ�õ�����		        
*********************************************************************************************************
*/
static uint8_t nRF905_WriteReadByte(uint8_t dat)
{  	
	/* ��SPI���ͻ������ǿ�ʱ�ȴ� */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	
	/* ͨ��SPI����һ�ֽ����� */
	SPI_I2S_SendData(SPI1, dat);		
	
	/* ��SPI���ջ�����Ϊ��ʱ�ȴ� */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* ͨ��SPI����һ�ֽ����� */
	return SPI_I2S_ReceiveData(SPI1);
}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_ReadBuf
*	����˵��: ����nRF905�ļĴ����ж���һ������
*	��    ��: _ucRegAddr: nRF905������+�Ĵ�����ַ��
 		      _pBuf��     ���ݻ����ַ��
		      _ucLen:     _pBuf�ݳ���
*	�� �� ֵ: ��	        
*********************************************************************************************************
*/
static void nRF905_ReadBuf(uint8_t _ucCmd, uint8_t *_pBuf, uint8_t _ucLen)
{
	uint8_t i;
	
	/* �������ģʽ����SPI�� ����ģʽ���ߵ���ģʽ���ܲ���SPI */
	nRF905_Standby();
	
	nRF905SE_CSN_0();
	
	/* �������� */
	nRF905_WriteReadByte(_ucCmd);

	/* ��ȡ���� */
	for(i = 0; i < _ucLen; i++) 
	{
		_pBuf[i] = nRF905_WriteReadByte(NOP);
	}
	
	nRF905SE_CSN_1();
}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_WriteBuf
*	����˵��: ������nRF905�ļĴ�����д��һ������
*	��    �Σ�_ucRegAddr: nRF905������+�Ĵ�����ַ��
 		      _pBuf��     ���ݻ����ַ��
		      _ucLen:     _pBuf�����ݳ��ȡ�
*	�� �� ֵ: ��	        
*********************************************************************************************************
*/
static void nRF905_WriteBuf(uint8_t _ucCmd, uint8_t *_pBuf, uint8_t _ucLen)
{
	uint8_t i;
	
	/* �������ģʽ����SPI�� ����ģʽ���ߵ���ģʽ���ܲ���SPI */
	nRF905_Standby();
	
	nRF905SE_CSN_0();
	
	/* �������� */
	nRF905_WriteReadByte(_ucCmd);

	/* д���� */
	for(i=0; i<_ucLen; i++) 
	{
		nRF905_WriteReadByte(*_pBuf++);
	}

	nRF905SE_CSN_1();	
}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_SetTxAddress
*	����˵��: д���͵�ַ
*	��    ��: _pBuf��     ���ݻ����ַ��
*		      _ucLen:     _pBuf�����ݳ��ȡ�
*	�� �� ֵ: ��	        
*********************************************************************************************************
*/
void nRF905_SetTxAddress(uint8_t *_pBuf, uint8_t _ucLen)
{
    nRF905_WriteBuf(nRF905_WTA, _pBuf, _ucLen);	
}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_SetTxAddress
*	����˵��: д������ַ
*	��    ��: _pBuf��     ���ݻ����ַ��
*		      _ucLen:     _pBuf�����ݳ��ȡ�
*	�� �� ֵ: ��	        
*********************************************************************************************************
*/
void nRF905_SetRxAddress(uint8_t *_pBuf, uint8_t _ucLen)
{
    nRF905_WriteBuf(nRF905_WRA, _pBuf, _ucLen);		
}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_SendPacket
*	����˵��: ����ָ�����ݣ���������Ҫ���͵����ݣ�Ȼ��������
*	��    ��: _pBuf��     ���ݻ����ַ��
*		      _ucLen:     _pBuf�����ݳ��ȡ�
*	�� �� ֵ: ��	        
*********************************************************************************************************
*/
void nRF905_SendPacket(uint8_t *_pBuf, uint8_t _ucLen)
{
	/* ����Ҫ���͵����� */
	nRF905_WriteBuf(nRF905_WTP, _pBuf, _ucLen);	
	
	/*
	   ���Ƚ�NRF905ģ�����÷���ģʽ���ȴ�ģ�齫�������ݼĴ��������ݷ�����ɣ�
       Ȼ�����������ɱ�־��������óɿ���ģʽ(���ж��������óɿ���ģʽ)
	*/
	nRF905_SetTxMode();
	
	/* �������ó�������ʽ�������ó�ʱ�ȴ�����ֹ��û�з�����ɣ��ٴ���ӷ������� */
	while(nRF905_DRPinFlag == 0);

	/* ���ݷ�����ɣ�����־������Ϊ0 */
	DISABLE_INT();  			/* ���ж� */
	nRF905_DRPinFlag = 0;
	ENABLE_INT();  				/* ���ж� */	
}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_ReceivePacket
*	����˵��: �������ݣ����ȵȴ�ģ�����������ɣ�Ȼ���ģ��������ݼĴ����ж�ȡ���յ�������
*	��    ��: _pBuf��     ���ݻ����ַ��
*		      _ucLen:     _pBuf�����ݳ��ȡ�
*	�� �� ֵ: ��	        
*********************************************************************************************************
*/
void nRF905_ReceivePacket(uint8_t *_pBuf, uint8_t _ucLen)
{	
	/*
		���Ƚ�NRF905ģ�����ý���ģʽ���ȴ�������ɱ�־��
        Ȼ�����������ɱ�־��������óɿ���ģʽ(���ж������ȡ���ݲ����óɿ���ģʽ)
	*/
	nRF905_SetRxMode();
	nRF905_RxBuffer= _pBuf;
	nRF905_RxBufferSize = _ucLen;
}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_ConnectCheck
*	����˵��: ��Ҫ����nRF905_��MCU�Ƿ���������
*	��    ��: ��
*	�� �� ֵ: 1 ��ʾ����������0 ��ʾ���Ӳ�����	        
*********************************************************************************************************
*/
uint8_t nRF905_ConnectCheck(void)
{
	uint8_t ucBuf[nRF905_AddressSize];

	/*����д��ı�����ַ */
	nRF905_ReadBuf(nRF905_RRA, ucBuf, nRF905_AddressSize); 
	 
	/* ���ڵ���Ŀ�� */
//	printf("ucBuf[0] = %d, RX_ADDRESS[0] = %d\r\n", ucBuf[0], RX_ADDRESS[0]);
//	printf("ucBuf[1] = %d, RX_ADDRESS[1] = %d\r\n", ucBuf[1], RX_ADDRESS[1]);
//	printf("ucBuf[2] = %d, RX_ADDRESS[2] = %d\r\n", ucBuf[2], RX_ADDRESS[2]);
//	printf("ucBuf[3] = %d, RX_ADDRESS[3] = %d\r\n", ucBuf[3], RX_ADDRESS[3]);
//	printf("ucBuf[4] = %d, RX_ADDRESS[4] = %d\r\n", ucBuf[4], RX_ADDRESS[4]);
	
	/* �Ƚ�д������ݺͶ������Ƿ���ͬ */
	if(strncmp((char *)nRF905_TxAddress, (char *)ucBuf, nRF905_AddressSize) == 0)
	{
		return 1;  /* ��ͬ */
	}
	else
	{
		return 0;  /* ����ͬ */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: nRF905_EXTI_ISR
*	����˵��: ���ͺͽ����ж�
*	��    ��: ��
*	�� �� ֵ: ��	        
*********************************************************************************************************
*/
void nRF905_EXTI_ISR(void) 
{
	/* ���ý����жϱ�־ */
	nRF905_DRPinFlag = 1;
	
	/* �������ģʽ */
	nRF905_Standby();
	
	/* ����ǹ����ڽ���ģʽ�����жϣ������յ����ݶ�ȡ���� */
	if(g_ucNRF905TxRxMode == 0)
	{
		nRF905_ReadBuf(nRF905_RRP, nRF905_RxBuffer, nRF905_RxBufferSize);
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
