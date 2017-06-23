#include "bsp_iic.h"

#include "bsp_systick.h"

/*	24c512    512page * 128byte * 8 = 512k   */
unsigned int I2C_PageSize = 128;
unsigned int I2C_PageNum = 512;
/***********************************************************************
**
*	@Function	Ӳ��IIC�����ʼ��
*	@Param		None
*	@Return 	None
*	@Modify		None
**
************************************************************************/
void I2C_HardwareInit(void)
{
	//I2C_HardWare_GPIO_Config();
	I2C_InitTypeDef  I2C_InitStructure; 
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);  
	/* PB6-I2C1_SCL��PB7-I2C1_SDA*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 =0x0A; 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_Cmd(I2Cxx, ENABLE);
	I2C_Init(I2Cxx, &I2C_InitStructure);
}
/***********************************************************************
**
*	@Function	Ӳ��IIC�����ʼ��
*	@Param		
*		@arg	WriteAddr��	дe2�ĵ�ַ
*		@arg	wData��		����
*		@arg	WriteAddr��	E2�豸��ַ
*	@Return 	None
*	@Modify		None
**
************************************************************************/
void I2C_HardwareWriteByte(unsigned int WriteAddr,unsigned char wData,unsigned int DeviceAddr)
{
	I2C_GenerateSTART(I2Cxx, ENABLE);
	while(!I2C_CheckEvent(I2Cxx, I2C_EVENT_MASTER_MODE_SELECT));  //Test on EV5 and clear it
	I2C_Send7bitAddress(I2Cxx, DeviceAddr, I2C_Direction_Transmitter);	
	while(!I2C_CheckEvent(I2Cxx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));	//Test on EV6 and clear it 
	I2C_SendData(I2Cxx, WriteAddr);
	while(!I2C_CheckEvent(I2Cxx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2Cxx, wData); 
	while(!I2C_CheckEvent(I2Cxx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));// Test on EV8 and clear it 
	I2C_GenerateSTOP(I2Cxx, ENABLE); 
}
/***********************************************************************
**
*	@Function	IIC ���ģ�� IO�ڳ�ʼ��
*	@Param		None
*	@Return 	None
*	@Modify		None
**
************************************************************************/
void I2C_SoftwareInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin =  IIC_SDA | IIC_SCL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // PP or OD   
    
	GPIO_Init(IIC_PORT, &GPIO_InitStructure);
	SCL_HIGHT;
	SDA_HIGHT;
}
/***********************************************************************
**
*	@Function	IIC ���ģ�� ��ʼ�ź�
*	@Param		None
*	@Return 	None
*	@Modify		None
**
************************************************************************/
void I2C_SoftwareStart(void)
{
	SDA_HIGHT;
	SCL_HIGHT;
	Delay_us(4);
	SDA_LOW;
	Delay_us(4);
	SCL_LOW;
}
/***********************************************************************
**
*	@Function	IIC ���ģ�� ֹͣ�ź�
*	@Param		None
*	@Return 	None
*	@Modify		None
**
************************************************************************/
void I2C_SoftwareStop(void)
{
    SDA_LOW;
	SCL_HIGHT;
	Delay_us(4);
	SDA_HIGHT;
	SCL_HIGHT;
	Delay_us(4);
}
/***********************************************************************
**
*	@Function	IIC ���ģ�� дһ���ֽ�
*	@Param		
*		@arg	wbyte:	д���ֽ�ֵ
*	@Return 	None
*	@Modify		None
**
************************************************************************/
void I2C_SoftwareWriteByte(unsigned char wbyte)
{
	unsigned char i = 0;
	SCL_LOW;
    for(i=0; i<8; i++)
	{
        if(wbyte & 0x80)
			SDA_HIGHT;
        else
			SDA_LOW;
		wbyte <<= 1;
		Delay_us(2);	// 100k hz
		SCL_HIGHT;
		Delay_us(2);
		SCL_LOW;
		Delay_us(2);
	}
}
/***********************************************************************
**
*	@Function	IIC ���ģ�� �ȴ�Ӧ��
*	@Param		None
*	@Return 	Ack: 1   No Ack: 0
*	@Modify		None
**
************************************************************************/
unsigned char I2C_SoftwareWaitAck(void)
{
    unsigned char Count =0;
	SCL_HIGHT;	
	Delay_us(1);
	SDA_HIGHT;
	Delay_us(1);
    while(READ_SDA)
	{
		Count ++;
		if(Count >= 250)
			return 0;
	}
	SCL_LOW;
	return 1;
}
/***********************************************************************
**
*	@Function	IIC ���ģ�� ����Ӧ���ź�
*	@Param		None
*	@Return 	None
*	@Modify		None
**
************************************************************************/
void I2C_SoftwareHostAck(void)
{
	SCL_LOW;
	SDA_LOW;
	Delay_us(2);
	SCL_HIGHT;
	Delay_us(2);
	SCL_LOW;
}
/***********************************************************************
**
*	@Function	IIC ���ģ�� ��Ӧ���ź�
*	@Param		None
*	@Return 	None
*	@Modify		None
**
************************************************************************/
void I2C_SoftwareHostNoAck(void)
{
	SCL_LOW;
	SDA_HIGHT;
	Delay_us(2);
	SCL_HIGHT;
	Delay_us(2);
	SCL_LOW;
}
/***********************************************************************
**
*	@Function	IIC ���ģ�� ��һ���ֽ�
*	@Param		
*		@arg	ack���Ƿ����Ӧ���ź�    1 Ӧ��0 ��Ӧ��
*	@Return 	None
*	@Modify		None
**
************************************************************************/
unsigned char I2C_SoftwareReadByte(unsigned char ack)
{
	unsigned char i=0;
    unsigned char rbyte=0;
	
    for(i = 0; i<8; i++)
    {
		SCL_LOW;
		Delay_us(2);
		SCL_HIGHT;
		rbyte <<= 1;
		if(READ_SDA)
			rbyte |= 1;;
		Delay_us(1);	
    }
	if(!ack)
		I2C_SoftwareHostNoAck();
	else
		I2C_SoftwareHostAck();
    return rbyte;
}


