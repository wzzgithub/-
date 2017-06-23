/**
  ******************************************************************************
  * @file    i2c.h
  * @author  YANDLD
  * @version V3.0
  * @date    2016.6.7
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_I2C_H__
#define __CH_LIB_I2C_H__


#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stdint.h>


#define HW_I2C0         (0x00U)
#define HW_I2C1         (0x01U)
#define HW_I2C2         (0x02U)

//!< I2C QuickInit macro
#define I2C1_SCL_PE01_SDA_PE00  (0X000081A1U)
#define I2C0_SCL_PE19_SDA_PE18  (0X0000A520U)
#define I2C0_SCL_PF22_SDA_PF23  (0X0000ACA8U)
#define I2C0_SCL_PB00_SDA_PB01  (0X00008088U)
#define I2C0_SCL_PB02_SDA_PB03  (0X00008488U)
#define I2C1_SCL_PC10_SDA_PC11  (0X00009491U)
#define I2C0_SCL_PD08_SDA_PD09  (0X00009098U)
#define I2C0_SCL_PE24_SDA_PE25  (0X0000B160U)
#define I2C1_SCL_PC01_SDA_PC02  (0X00008291U)
#define I2C0_SCL_PB03_SDA_PB04  (0X00008688U)
#define I2C1_SCL_PD07_SDA_PD06  (0x00008D19U)
#define I2C0_SCL_PC06_SDA_PC07  (0x00008DD0U)
#define I2C2_SCL_PA11_SDA_PA10  (0X38009482U)


uint32_t I2C_Init(uint32_t MAP, uint32_t baudrate);
uint32_t I2C_BurstRead(uint32_t instance, uint8_t addr, uint32_t regAddr, uint32_t regLen, uint8_t* buf, uint32_t len);
uint32_t I2C_BurstWrite(uint32_t instance ,uint8_t addr, uint32_t regAddr, uint32_t regLen, uint8_t *buf, uint32_t len);
uint32_t I2C_ReadReg(uint32_t instance, uint8_t addr, uint8_t regAddr, uint8_t* buf);
uint32_t I2C_WriteReg(uint32_t instance, uint8_t addr, uint8_t regAddr, uint8_t buf);
uint32_t SCCB_ReadReg(uint32_t instance, uint8_t addr, uint8_t regAddr, uint8_t* buf);
uint32_t SCCB_WriteReg(uint32_t instance, uint8_t addr, uint8_t regAddr, uint8_t buf);

/* for GPIO I2C only */
uint32_t I2C_InitEx(uint32_t port, uint32_t sda_pin, uint32_t scl_pin, uint32_t baudrate);


/* test function */
uint32_t I2C_Probe(uint32_t instance, uint8_t addr);
void I2C_Scan(uint32_t MAP);

#ifdef __cplusplus
}
#endif





#endif
