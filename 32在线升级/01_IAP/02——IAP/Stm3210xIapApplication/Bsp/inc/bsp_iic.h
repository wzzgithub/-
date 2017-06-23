#ifndef	_BSP_IIC_H__
#define	_BSP_IIC_H__

#include "stm32f10x.h"

//#define	USE_I2C_1


#ifdef USE_I2C_1
	#define	I2Cxx	I2C1
#else	// use i2c 2
	#define	I2Cxx			I2C2
#endif

#define	IIC_SDA				GPIO_Pin_11
#define	IIC_SCL				GPIO_Pin_10
#define	IIC_GPIO_RCC		RCC_APB2Periph_GPIOB
#define	IIC_PORT			GPIOB




#define	SDA_HIGHT			GPIO_WriteBit(IIC_PORT,IIC_SDA,(BitAction)1)
#define	SDA_LOW				GPIO_WriteBit(IIC_PORT,IIC_SDA,(BitAction)0)

#define	SCL_HIGHT			GPIO_WriteBit(IIC_PORT,IIC_SCL,(BitAction)1)
#define	SCL_LOW				GPIO_WriteBit(IIC_PORT,IIC_SCL,(BitAction)0)

#define	READ_SDA			(GPIO_ReadInputDataBit(IIC_PORT,IIC_SDA) ? 1 : 0)














void I2C_HardwareInit(void);
void I2C_SoftwareInit(void);

void I2C_SoftwareWriteByte(unsigned char wbyte);
void I2C_SoftwareHostNoAck(void);
void I2C_SoftwareHostAck(void);
unsigned char I2C_SoftwareWaitAck(void);
void I2C_SoftwareStop(void);
void I2C_SoftwareStart(void);
unsigned char I2C_SoftwareReadByte(unsigned char ack);


void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);
void I2C_EE_WaitEepromStandbyState(void);


void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr);
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);
void I2C_HardwareInit(void);


void I2C_HardwareWriteByte(unsigned int WriteAddr,unsigned char wData,unsigned int DeviceAddr);

#endif


