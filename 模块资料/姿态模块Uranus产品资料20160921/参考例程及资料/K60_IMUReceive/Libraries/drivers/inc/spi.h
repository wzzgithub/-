/**
  ******************************************************************************
  * @file    spi.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.06.12
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_SPI_H__
#define __CH_LIB_SPI_H__

#include <stdint.h>
#include <stdbool.h>
  
//!< SPI instance
#define HW_SPI0     (0x00)
#define HW_SPI1     (0x01)
#define HW_SPI2     (0x02)


//!< SPI 数据格式
typedef enum
{
    kSPI_CPOL0_CPHA0,
    kSPI_CPOL0_CPHA1,
    kSPI_CPOL1_CPHA1,
    kSPI_CPOL1_CPHA0,
}SPI_Format_t;


//!< 中断
typedef enum
{
    kSPI_TCF,       /* transfer complete */
}SPI_Int_t;


//!< 快速初始化结构，引脚位图
#define SPI1_SCK_PE02_MOSI_PE01_MISO_PE03  (0x0000C2A1U)
#define SPI0_SCK_PE17_MOSI_PE18_MISO_PE19  (0X0000E2A0U)
#define SPI0_SCK_PA15_MOSI_PA16_MISO_PA17  (0X0000DE80U)
#define SPI0_SCK_PC05_MOSI_PC06_MISO_PC07  (0X0000CA90U)
#define SPI0_SCK_PD01_MOSI_PD02_MISO_PD03  (0X0000C298U)
#define SPI1_SCK_PD05_MOSI_PD06_MISO_PD07  (0x01F00CBD9)
#define SPI2_SCK_PB21_MOSI_PB22_MISO_PB23  (0x0000EA8AU)
#define SPI2_SCK_PD12_MOSI_PD13_MISO_PD14  (0x0000D89AU)


/* API 功能接口 */
uint32_t SPI_Init(uint32_t MAP, uint32_t baudrate);
void SPI_SetFormat(uint32_t instance, SPI_Format_t format);
uint32_t SPI_ReadWrite(uint32_t instance, uint32_t data);
uint32_t SPI_SetIntMode(uint32_t instance, SPI_Int_t mode, bool val);
void SPI_SetTxFIFO(uint32_t instance, bool val);
void SPI_SetRxFIFO(uint32_t instance, bool val);

#endif

