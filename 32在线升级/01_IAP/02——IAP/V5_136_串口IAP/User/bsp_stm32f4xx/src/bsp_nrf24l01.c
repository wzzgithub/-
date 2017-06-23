/*
*********************************************************************************************************
*	                                  
*	ģ������ : nRF24L01����ģ��
*	�ļ����� : bsp_nrf24l01.c
*	��    �� : V1.0
*	˵    �� : nRF24L01����
*	�޸ļ�¼ :
*		�汾��    ����          ����          ˵��
*		v1.0    2015-02-12     Eric2013	      �װ�
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"

/*
	nRF24L01ģ�����ֱ�Ӳ嵽STM32-V5������nRF24L01ģ�����ĸ�ӿ��ϡ�

    nRF24L01ģ��   STM32F407������
	  VCC   ------  3.3V
	  GND   ------  GND
      SCK   ------  PB3/SPI3_SCK
      MISO  ------  PB4/SPI3_MISO
      MOSI  ------  PB5/SPI3_MOSI
      CSN   ------  PF7/NRF24L01_CSN
      IRQ   ------  PH7/NRF24L01_IRQ
      CE    ------  PA4/NRF905_TX_EN/NRF24L01_CE/DAC1_OUT
*/

/* ����GPIO�˿� */
#define nRF24L01_RCC_SPI  RCC_APB2Periph_SPI1
#define nRF24L01_RCC_ALL  (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOH)

#define nRF24L01_PORT_CE   GPIOA
#define nRF24L01_PIN_CE    GPIO_Pin_4

#define nRF24L01_PORT_CSN  GPIOF
#define nRF24L01_PIN_CSN   GPIO_Pin_7

#define nRF24L01_PORT_IRQ  GPIOH
#define nRF24L01_PIN_IRQ   GPIO_Pin_7

/* SPI�ӿڵ�SCK MOSI MISO�˿ڶ��� */
#define nRF24L01_PORT_SPI	GPIOB
#define nRF24L01_PIN_SCK	GPIO_Pin_3
#define nRF24L01_PIN_MISO	GPIO_Pin_4
#define nRF24L01_PIN_MOSI	GPIO_Pin_5

#define nRF24L01_PinSource_SCK	 GPIO_PinSource3
#define nRF24L01_PinSource_MISO	 GPIO_PinSource4
#define nRF24L01_PinSource_MOSI	 GPIO_PinSource5

#define nRF24L01_AF_SPI    GPIO_AF_SPI1

/*
	����V5�����屾������⣬���������һ��Ҫ���ã���ֹTFT���ϵ�spi flashӰ��nRF24L01��ͨ��
	����TFT�ӿ��е�PWM��ΪΪ�������ģʽ��
	PWM = 1  ���ģʽ֧��STM32��дRA8875��ҵĴ���Flash
	PWM = 0 ������������ģʽ����RA8875 DMA��ȡ��ҵĴ���Flash

	����Ҫѡ��PWM����Ϊ1��
*/
#define RA8875_PORT_PWM	    GPIOF
#define RA8875_PIN_PWM	    GPIO_Pin_6

/* ���Ų��������üĴ�������ʽ���ӿ�����ٶ� */
#define nRF24L01_CSN_1()    nRF24L01_PORT_CSN->BSRRL = nRF24L01_PIN_CSN
#define nRF24L01_CSN_0()    nRF24L01_PORT_CSN->BSRRH = nRF24L01_PIN_CSN

#define nRF24L01_CE_1()     nRF24L01_PORT_CE->BSRRL = nRF24L01_PIN_CE
#define nRF24L01_CE_0()     nRF24L01_PORT_CE->BSRRH = nRF24L01_PIN_CE

#define nRF24L01_IRQ_Read()	  (nRF24L01_PORT_IRQ->IDR & nRF24L01_PIN_IRQ)

/* ����˺ͽ��ն˵�ַ */
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};  
uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_CfgSpiHard
*	����˵��: ����STM32�ڲ�SPIӲ���Ĺ���ģʽ���ٶȵȲ��������ڷ���nRF24L01��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void nRF24L01_CfgSpiHard(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	
	/*
	STM32F4XX ʱ�Ӽ���.
		HCLK = 168M
		PCLK1 = HCLK / 4 = 42M
		PCLK2 = HCLK / 2 = 84M

		SPI2��SPI3 �� PCLK1, ʱ��42M
		SPI1       �� PCLK2, ʱ��84M

		STM32F4 ֧�ֵ����SPIʱ��Ϊ 37.5 Mbits/S, �����Ҫ��Ƶ������ʹ�õ���SPI1��
	*/
	/* ����SPIӲ������ */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/* ���ݷ���2��ȫ˫�� */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		/* STM32��SPI����ģʽ ������ģʽ */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* ����λ���� �� 8λ */
	/* SPI_CPOL��SPI_CPHA���ʹ�þ���ʱ�Ӻ����ݲ��������λ��ϵ��
	   ��������: ���߿����ǵ͵�ƽ,��1�����ز������ݡ�
	*/
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			/* Ƭѡ���Ʒ�ʽ��������� */

	/* ���ò�����Ԥ��Ƶϵ�� 84MHz / 16 = 5.25MHz��24L01֧�ֵ����SPIʱ��Ϊ10MHz
	   ����ѡ��SPI_BaudRatePrescaler_8���׳���
	*/
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/* ����λ������򣺸�λ�ȴ� */
	SPI_InitStructure.SPI_CRCPolynomial = 7;			/* CRC����ʽ�Ĵ�������λ��Ϊ7�������̲��� */
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);				/* ʹ��SPI  */
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_InitHard
*	����˵��: SPI����
*	��    �Σ���
*	�� �� ֵ: ��		                   			  
*********************************************************************************************************
*/
void nRF24L01_InitHard(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*ʹ��GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(nRF24L01_RCC_ALL, ENABLE);
	
	/* ����nRF24L01��CE���� */
	/* ����Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = nRF24L01_PIN_CE;
	GPIO_Init(nRF24L01_PORT_CE, &GPIO_InitStructure);
	
	/* ����nRF24L01�� CSN ���� */
	/* ����Ϊ�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = nRF24L01_PIN_CSN;
	GPIO_Init(nRF24L01_PORT_CSN, &GPIO_InitStructure);
	
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
	
	/* ����nRF24L01�� IRQ ���� */
	/* ����Ϊ��������� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 		
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 	
	GPIO_InitStructure.GPIO_Pin = nRF24L01_PIN_IRQ; 
	GPIO_Init(nRF24L01_PORT_IRQ, &GPIO_InitStructure);
	
	/* ���� SCK, MISO , MOSI Ϊ���ù��� */
	/* ʹ��SPI1ʱ�� */
	RCC_APB2PeriphClockCmd(nRF24L01_RCC_SPI, ENABLE);
	GPIO_PinAFConfig(nRF24L01_PORT_SPI, nRF24L01_PinSource_SCK,  nRF24L01_AF_SPI);
	GPIO_PinAFConfig(nRF24L01_PORT_SPI, nRF24L01_PinSource_MISO, nRF24L01_AF_SPI);
	GPIO_PinAFConfig(nRF24L01_PORT_SPI, nRF24L01_PinSource_MOSI, nRF24L01_AF_SPI);

	/* ע��GPIO�ٶ���ò��ó���25MHz��Ҫ�����׳��� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = nRF24L01_PIN_SCK | nRF24L01_PIN_MISO | nRF24L01_PIN_MOSI;
	GPIO_Init(nRF24L01_PORT_SPI, &GPIO_InitStructure);
	
	/* ���� SPI1����ģʽ */
	nRF24L01_CfgSpiHard();
	
	/* ��������csn���ţ�NRF�������״̬ */
	nRF24L01_CSN_1();  
} 

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_WriteReadByte
*	����˵��: ������NRF��/дһ�ֽ�����
*	��    ��: д�������
*	�� �� ֵ: ��ȡ�õ�����		        
*********************************************************************************************************
*/
static uint8_t nRF24L01_WriteReadByte(uint8_t dat)
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
*	�� �� ��: nRF24L01_WriteReg
*	����˵��: ������nRF24L01�ض��ļĴ���д������
*	��    ��: _ucRegAddr: nRF24L01������+�Ĵ�����ַ��
 		      _usValue:   ��Ҫ��Ĵ���д�������
*	�� �� ֵ: nRF24L01��status�Ĵ�����״̬		        
*********************************************************************************************************
*/
static uint8_t nRF24L01_WriteReg(uint8_t _ucRegAddr, uint8_t _usValue)
{
	uint8_t ucStatus;
	
	nRF24L01_CE_0();
	
	/* �õ�CSN��ʹ��SPI���� */
	nRF24L01_CSN_0();
				
	/* ��������Ĵ����� */
	ucStatus = nRF24L01_WriteReadByte(_ucRegAddr);
		 
	 /* ��Ĵ���д������ */
	nRF24L01_WriteReadByte(_usValue); 
	          
	/* CSN���ߣ���� */	   
	nRF24L01_CSN_1();	
		
	/* ����״̬�Ĵ�����ֵ */
	return(ucStatus);
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_ReadReg
*	����˵��: ���ڴ�nRF24L01�ض��ļĴ�����������
*	��    ��: reg: nRF24L01������+�Ĵ�����ַ
*	�� �� ֵ: �Ĵ����е�����		        
*********************************************************************************************************
*/
static uint8_t nRF24L01_ReadReg(uint8_t _ucRegAddr)
{
 	uint8_t usValue;

	nRF24L01_CE_0();
	
	/* �õ�CSN��ʹ��SPI���� */
 	nRF24L01_CSN_0();
				
  	 /* ���ͼĴ����� */
	nRF24L01_WriteReadByte(_ucRegAddr); 

	 /* ��ȡ�Ĵ�����ֵ */
	usValue = nRF24L01_WriteReadByte(NOP);
	            
   	/*CSN���ߣ����*/
	nRF24L01_CSN_1();		
   	
	return usValue;
}	

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_ReadBuf
*	����˵��: ���ڴ�nRF24L01�ļĴ����ж���һ������
*	��    ��: _ucRegAddr: nRF24L01������+�Ĵ�����ַ��
 		      _pBuf��     ���ݻ����ַ��
		      _ucLen:     _pBuf�ݳ���
*	�� �� ֵ: nRF24L01��status�Ĵ�����״̬		        
*********************************************************************************************************
*/
static uint8_t nRF24L01_ReadBuf(uint8_t _ucRegAddr, uint8_t *_pBuf, uint8_t _ucLen)
{
 	uint8_t ucStatus, i;

	nRF24L01_CE_0();
	
	/* �õ�CSN��ʹ��SPI���� */
	nRF24L01_CSN_0();
		
	/*���ͼĴ�����*/		
	ucStatus = nRF24L01_WriteReadByte(_ucRegAddr); 

 	/*��ȡ����������*/
	for(i = 0; i < _ucLen; i++)
	{
		_pBuf[i] = nRF24L01_WriteReadByte(NOP);
	}
	    
	/* CSN���ߣ���� */
	nRF24L01_CSN_1();	
		
 	return ucStatus;	
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_WriteBuf
*	����˵��: ������nRF24L01�ļĴ�����д��һ������
*	��    �Σ�_ucRegAddr: nRF24L01������+�Ĵ�����ַ��
 		      _pBuf��     ���ݻ����ַ��
		      _ucLen:     _pBuf�����ݳ��ȡ�
*	�� �� ֵ: NRF��status�Ĵ�����״̬��	        
*********************************************************************************************************
*/
static uint8_t nRF24L01_WriteBuf(uint8_t _ucRegAddr, uint8_t *_pBuf, uint8_t _ucLen)
{
	 uint8_t ucStatus,i;
	
	 nRF24L01_CE_0();
	
   	 /*�õ�CSN��ʹ��SPI����*/
	 nRF24L01_CSN_0();			

	 /*���ͼĴ�����*/	
  	 ucStatus = nRF24L01_WriteReadByte(_ucRegAddr); 
 	
  	  /*�򻺳���д������*/
	 for(i = 0; i < _ucLen; i++)
	{
		nRF24L01_WriteReadByte(*_pBuf++);
	}
	  	   
	/*CSN���ߣ����*/
	nRF24L01_CSN_1();			
  
  	return (ucStatus);	
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_RxMode
*	����˵��: ����nRF24L01�����ڽ���ģʽ
*	��    ��: ��
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void nRF24L01_RxMode(void)
{
	nRF24L01_CE_0();
	nRF24L01_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH);  /* дRX�ڵ��ַ */
	nRF24L01_WriteReg(NRF_WRITE_REG+EN_AA, 0x01);               			/* ʹ��ͨ��0���Զ�Ӧ�� */    
	nRF24L01_WriteReg(NRF_WRITE_REG+EN_RXADDR, 0x01);           			/* ʹ��ͨ��0�Ľ��յ�ַ */    
	nRF24L01_WriteReg(NRF_WRITE_REG+RF_CH, CHANAL);             			/* ����RFͨ��Ƶ�� */    
	nRF24L01_WriteReg(NRF_WRITE_REG+RX_PW_P0, RX_PLOAD_WIDTH);  			/* ѡ��ͨ��0����Ч���ݿ�� */      
	nRF24L01_WriteReg(NRF_WRITE_REG+RF_SETUP, 0x0f); /* ����TX�������,0db����,2Mbps,���������濪�� */   
	nRF24L01_WriteReg(NRF_WRITE_REG+CONFIG, 0x0f);   /* ���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ */ 
	
	/*CE���ߣ��������ģʽ*/	
	nRF24L01_CE_1(); 
}    

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_TxMode
*	����˵��: ����nRF24L01�����ڷ���ģʽ
*	��    ��: ��
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void nRF24L01_TxMode(void)
{  
	nRF24L01_CE_0();
	nRF24L01_WriteBuf(NRF_WRITE_REG+TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    /* дTX�ڵ��ַ */ 
	nRF24L01_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); /* ����TX�ڵ��ַ,��ҪΪ��ʹ��ACK */   
	nRF24L01_WriteReg(NRF_WRITE_REG+EN_AA, 0x01);     /* ʹ��ͨ��0���Զ�Ӧ�� */    	
	nRF24L01_WriteReg(NRF_WRITE_REG+EN_RXADDR, 0x01); /* ʹ��ͨ��0�Ľ��յ�ַ */  	
	nRF24L01_WriteReg(NRF_WRITE_REG+SETUP_RETR, 0x0f);/* �����Զ��ط����ʱ��:250us + 86us;����Զ��ط�����:10�� */	
	nRF24L01_WriteReg(NRF_WRITE_REG+RF_CH, CHANAL);   /* ����RFͨ��ΪCHANAL */	
	nRF24L01_WriteReg(NRF_WRITE_REG+RF_SETUP, 0x0f);  /* ����TX�������,0db����,2Mbps,���������濪�� */   	
	nRF24L01_WriteReg(NRF_WRITE_REG+CONFIG, 0x0e);    /* ���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж� */	
	
	/*CE���ߣ����뷢��ģʽ*/	
	nRF24L01_CE_1();
	bsp_DelayUS(100);  /* CEҪ����һ��ʱ��Ž��뷢��ģʽ��ʱ�����10us */
}

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_ConnectCheck
*	����˵��: ��Ҫ����nRF24L01_��MCU�Ƿ���������
*	��    ��: ��
*	�� �� ֵ: 1 ��ʾ����������0 ��ʾ���Ӳ�����	        
*********************************************************************************************************
*/
uint8_t nRF24L01_ConnectCheck(void)
{
	uint8_t ucBuf[TX_ADR_WIDTH];

	/*д��TX_ADR_WIDTH���ֽڵĵ�ַ.  */  
	nRF24L01_WriteBuf(NRF_WRITE_REG+TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);

	/*����д��ĵ�ַ */
	nRF24L01_ReadBuf(TX_ADDR, ucBuf, TX_ADR_WIDTH); 
	 
	/* ���ڵ���Ŀ�� */
//	printf("ucBuf[0] = %d, TX_ADDRESS[0] = %d\r\n", ucBuf[0], TX_ADDRESS[0]);
//	printf("ucBuf[1] = %d, TX_ADDRESS[1] = %d\r\n", ucBuf[1], TX_ADDRESS[1]);
//	printf("ucBuf[2] = %d, TX_ADDRESS[2] = %d\r\n", ucBuf[2], TX_ADDRESS[2]);
//	printf("ucBuf[3] = %d, TX_ADDRESS[3] = %d\r\n", ucBuf[3], TX_ADDRESS[3]);
//	printf("ucBuf[4] = %d, TX_ADDRESS[4] = %d\r\n", ucBuf[4], TX_ADDRESS[4]);
	
	/* �Ƚ�д������ݺͶ������Ƿ���ͬ */
	if(strncmp((char *)TX_ADDRESS, (char *)ucBuf, TX_ADR_WIDTH) == 0)
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
*	�� �� ��: nRF24L01_TxData
*	����˵��: ������nRF24L01�ķ��ͻ�������д������
*	��    ��: _pTxBuf ���ݻ����ַ
*	�� �� ֵ: ���ͽ�����ɹ�����TX_DS, ʧ�ܷ���MAX_RT��0		        
*********************************************************************************************************
*/
uint8_t nRF24L01_TxData(uint8_t *_pTxBuf)
{
	uint8_t ucState;
	uint32_t ulCount = 0;   

	/*CEΪ�ͣ��������ģʽ1*/
	nRF24L01_CE_0();

	/*д���ݵ�_pTxBuf���32���ֽ�*/						
	nRF24L01_WriteBuf(WR_TX_PLOAD, _pTxBuf, TX_PLOAD_WIDTH);

	/*CEΪ�ߣ�_pTxBuf�ǿգ��������ݰ� */   
	nRF24L01_CE_1();
	  	
	/*
	  �ȴ���������ж�                            
   	  while(nRF24L01_IRQ_Read()!=0); 
      ѡ���������ַ�ʽ������Ч��ֹ������������ʱ���������״̬ 
	*/
	while((nRF24L01_IRQ_Read() != 0) && (ulCount < 65535))
	{
		ulCount++;
	}
	
	if(ulCount >= 65535) 
	{
		return 0;		
	}
	
	/* ��ȡ״̬�Ĵ�����ֵ */                              
	ucState = nRF24L01_ReadReg(STATUS);

	/*���TX_DS��MAX_RT�жϱ�־*/                  
	nRF24L01_WriteReg(NRF_WRITE_REG+STATUS, ucState); 	

	nRF24L01_WriteReg(FLUSH_TX,NOP);      /* ���TX FIFO�Ĵ��� */ 

	 /*�ж��ж�����*/ 
	/* �ﵽ����ط����� */
	if(ucState & MAX_RT)                     
	{
		return MAX_RT;
	}
	/* ������� */
	else if(ucState & TX_DS)
	{
		return TX_DS;
	} 
	/* ����ԭ����ʧ�� */	
	else
	{
		return 0;               
	}
} 

/*
*********************************************************************************************************
*	�� �� ��: nRF24L01_RxData
*	����˵��: ���ڴ�nRF24L01�Ľ��ջ������ж�������
*	��    ��: _pRxBuf ���ݻ����ַ
*	�� �� ֵ: ���ս�����ɹ�����RX_DS, ʧ�ܷ���0		        
*********************************************************************************************************
*/
uint8_t nRF24L01_RxData(uint8_t *_pRxBuf)
{
	uint8_t ucState;
	uint32_t ulCount = 0;  

	nRF24L01_CE_1();	  /* �������״̬ */
	
	/*
	  �ȴ������ж�
	  while(nRF24L01_IRQ_Read()!=0);
	  ѡ���������ַ�ʽ������Ч��ֹ������������ʱ���������״̬ 
	*/
	while((nRF24L01_IRQ_Read() != 0) && (ulCount < 65535))
	{
		ulCount++;
	}
	
	if(ulCount >= 65535) 
	{
		return 0;		
	}
	
	nRF24L01_CE_0();  	 /* �������״̬ */

	/*��ȡstatus�Ĵ�����ֵ  */               
	ucState=nRF24L01_ReadReg(STATUS);
	 
	/* ����жϱ�־*/      
	nRF24L01_WriteReg(NRF_WRITE_REG+STATUS, ucState);

	/*�ж��Ƿ���յ�����*/
	if(ucState & RX_DR)    /* ���յ����� */
	{
		nRF24L01_ReadBuf(RD_RX_PLOAD, _pRxBuf, RX_PLOAD_WIDTH); /* ��ȡ���� */
		nRF24L01_WriteReg(FLUSH_RX, NOP);                       /* ���RX FIFO�Ĵ��� */
		return RX_DR; 
	}
	else 
    {
		return 0;   /* û�յ��κ����� */
	}			
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
