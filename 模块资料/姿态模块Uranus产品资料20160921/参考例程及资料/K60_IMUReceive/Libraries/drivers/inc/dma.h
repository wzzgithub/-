/**
  ******************************************************************************
  * @file    dma.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.06.05
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#ifndef __CH_LIB_DMA_H__
#define __CH_LIB_DMA_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    kDmaRequestMux0Disable = 0|0x100U, /*!< DMAMUX TriggerDisabled. */
    kDmaRequestMux0Reserved1 = 1|0x100U, /*!< Reserved1 */
    kDmaRequestMux0UART0Rx = 2|0x100U, /*!< UART0 Receive. */
    kDmaRequestMux0UART0Tx = 3|0x100U, /*!< UART0 Transmit. */
    kDmaRequestMux0UART1Rx = 4|0x100U, /*!< UART1 Receive. */
    kDmaRequestMux0UART1Tx = 5|0x100U, /*!< UART1 Transmit. */
    kDmaRequestMux0UART2Rx = 6|0x100U, /*!< UART2 Receive. */
    kDmaRequestMux0UART2Tx = 7|0x100U, /*!< UART2 Transmit. */
    kDmaRequestMux0UART3Rx = 8|0x100U, /*!< UART3 Receive. */
    kDmaRequestMux0UART3Tx = 9|0x100U, /*!< UART3 Transmit. */
    kDmaRequestMux0UART4 = 10|0x100U, /*!< UART4 Transmit or Receive. */
    kDmaRequestMux0UART5 = 11|0x100U, /*!< UART5 Transmit or Receive. */
    kDmaRequestMux0I2S0Rx = 12|0x100U, /*!< I2S0 Receive. */
    kDmaRequestMux0I2S0Tx = 13|0x100U, /*!< I2S0 Transmit. */
    kDmaRequestMux0SPI0Rx = 14|0x100U, /*!< SPI0 Receive. */
    kDmaRequestMux0SPI0Tx = 15|0x100U, /*!< SPI0 Transmit. */
    kDmaRequestMux0SPI1 = 16|0x100U, /*!< SPI1 Transmit or Receive. */
    kDmaRequestMux0SPI2 = 17|0x100U, /*!< SPI2 Transmit or Receive. */
    kDmaRequestMux0I2C0 = 18|0x100U, /*!< I2C0. */
    kDmaRequestMux0I2C1I2C2 = 19|0x100U, /*!< I2C1 and I2C2. */
    kDmaRequestMux0FTM0Channel0 = 20|0x100U, /*!< FTM0 C0V. */
    kDmaRequestMux0FTM0Channel1 = 21|0x100U, /*!< FTM0 C1V. */
    kDmaRequestMux0FTM0Channel2 = 22|0x100U, /*!< FTM0 C2V. */
    kDmaRequestMux0FTM0Channel3 = 23|0x100U, /*!< FTM0 C3V. */
    kDmaRequestMux0FTM0Channel4 = 24|0x100U, /*!< FTM0 C4V. */
    kDmaRequestMux0FTM0Channel5 = 25|0x100U, /*!< FTM0 C5V. */
    kDmaRequestMux0FTM0Channel6 = 26|0x100U, /*!< FTM0 C6V. */
    kDmaRequestMux0FTM0Channel7 = 27|0x100U, /*!< FTM0 C7V. */
    kDmaRequestMux0FTM1Channel0 = 28|0x100U, /*!< FTM1 C0V. */
    kDmaRequestMux0FTM1Channel1 = 29|0x100U, /*!< FTM1 C1V. */
    kDmaRequestMux0FTM2Channel0 = 30|0x100U, /*!< FTM2 C0V. */
    kDmaRequestMux0FTM2Channel1 = 31|0x100U, /*!< FTM2 C1V. */
    kDmaRequestMux0FTM3Channel0 = 32|0x100U, /*!< FTM3 C0V. */
    kDmaRequestMux0FTM3Channel1 = 33|0x100U, /*!< FTM3 C1V. */
    kDmaRequestMux0FTM3Channel2 = 34|0x100U, /*!< FTM3 C2V. */
    kDmaRequestMux0FTM3Channel3 = 35|0x100U, /*!< FTM3 C3V. */
    kDmaRequestMux0FTM3Channel4 = 36|0x100U, /*!< FTM3 C4V. */
    kDmaRequestMux0FTM3Channel5 = 37|0x100U, /*!< FTM3 C5V. */
    kDmaRequestMux0FTM3Channel6 = 38|0x100U, /*!< FTM3 C6V. */
    kDmaRequestMux0FTM3Channel7 = 39|0x100U, /*!< FTM3 C7V. */
    kDmaRequestMux0ADC0 = 40|0x100U, /*!< ADC0. */
    kDmaRequestMux0ADC1 = 41|0x100U, /*!< ADC1. */
    kDmaRequestMux0CMP0 = 42|0x100U, /*!< CMP0. */
    kDmaRequestMux0CMP1 = 43|0x100U, /*!< CMP1. */
    kDmaRequestMux0CMP2 = 44|0x100U, /*!< CMP2. */
    kDmaRequestMux0DAC0 = 45|0x100U, /*!< DAC0. */
    kDmaRequestMux0DAC1 = 46|0x100U, /*!< DAC1. */
    kDmaRequestMux0CMT = 47|0x100U, /*!< CMT. */
    kDmaRequestMux0PDB = 48|0x100U, /*!< PDB0. */
    kDmaRequestMux0PortA = 49|0x100U, /*!< PTA. */
    kDmaRequestMux0PortB = 50|0x100U, /*!< PTB. */
    kDmaRequestMux0PortC = 51|0x100U, /*!< PTC. */
    kDmaRequestMux0PortD = 52|0x100U, /*!< PTD. */
    kDmaRequestMux0PortE = 53|0x100U, /*!< PTE. */
    kDmaRequestMux0IEEE1588Timer0 = 54|0x100U, /*!< ENET IEEE 1588_timer_0. */
    kDmaRequestMux0IEEE1588Timer1 = 55|0x100U, /*!< ENET IEEE 1588_timer_1. */
    kDmaRequestMux0IEEE1588Timer2 = 56|0x100U, /*!< ENET IEEE 1588_timer_2. */
    kDmaRequestMux0IEEE1588Timer3 = 57|0x100U, /*!< ENET IEEE 1588_timer_3. */
    kDmaRequestMux0AlwaysOn58 = 58|0x100U, /*!< DMAMUX Always Enabled_slot. */
    kDmaRequestMux0AlwaysOn59 = 59|0x100U, /*!< DMAMUX Always Enabled_slot. */
    kDmaRequestMux0AlwaysOn60 = 60|0x100U, /*!< DMAMUX Always Enabled_slot. */
    kDmaRequestMux0AlwaysOn61 = 61|0x100U, /*!< DMAMUX Always Enabled_slot. */
    kDmaRequestMux0AlwaysOn62 = 62|0x100U, /*!< DMAMUX Always Enabled_slot. */
    kDmaRequestMux0AlwaysOn63 = 63|0x100U, /*!< DMAMUX Always Enabled_slot. */
}DMAReq_t;

//!< DMA通道
#define HW_DMA_CH0    (0x00)    /*!< Hardware DMA CH0 */
#define HW_DMA_CH1    (0x01)
#define HW_DMA_CH2    (0x02)
#define HW_DMA_CH3    (0x03)

//!< 触发模式选择
typedef enum
{
    kDMA_TrigSrc_Normal,        /* Normal Mode */
    kDMA_TrigSrc_Periodic,      /* Period Trigger Mode */
}DMA_TrigSrcMod_t;


//!< DMA Moduluo
typedef enum 
{
    kDMA_ModuloDisable = 0x0U,
    kDMA_Modulo16bytes = 0x1U,
    kDMA_Modulo32bytes = 0x2U,
    kDMA_Modulo64bytes = 0x3U,
    kDMA_Modulo128bytes = 0x4U,
    kDMA_Modulo256bytes = 0x5U,
    kDMA_Modulo512bytes = 0x6U,
    kDMA_Modulo1Kbytes = 0x7U,
    kDMA_Modulo2Kbytes = 0x8U,
    kDMA_Modulo4Kbytes = 0x9U,
    kDMA_Modulo8Kbytes = 0xAU,
    kDMA_Modulo16Kbytes = 0xBU,
    kDMA_Modulo32Kbytes = 0xCU,
    kDMA_Modulo64Kbytes = 0xDU,
    kDMA_Modulo128Kbytes = 0xEU,
    kDMA_Modulo256Kbytes = 0xFU,
} DMA_Modulo_t;

//!< 初始化结构体
typedef struct 
{
    uint8_t                     chl;
    uint32_t                    trigSrc;
    uint16_t                    totalByte;       /* int byte */
    DMA_TrigSrcMod_t            trigSrcMod;

    bool                        sAddrIsInc;
    uint32_t                    sAddr;
    uint32_t                    sDataWidth;     /* size in byte:1,2,4*/

    bool                        dAddrIsInc;                
    uint32_t                    dAddr;                      
    uint32_t                    dDataWidth;                                               
}DMA_Init_t;


//!< interrupt select
typedef enum
{
    kDMAInt_All,
}DMA_Int_t;  

//typedef struct
//{
//    void *      sAddr;
//    void *      dAddr;
//    DMAReq_t    req;
//    uint32_t    chl;
//    bool        isActive;
//}DMA_PD_t;


void DMA_Init(DMA_Init_t *Init);
void DMA_Start(uint8_t chl);
void DMA_Stop(uint8_t chl);
void DMA_SetIntMode(uint8_t chl, DMA_Int_t mode, bool val);
uint32_t DMA_GetDestAddr(uint8_t chl);
uint32_t DMA_GetSrcAddr(uint8_t chl);
void DMA_SetDestAddr(uint8_t chl, uint32_t addr);
void DMA_SetSrcAddr(uint8_t chl, uint32_t addr);
void DMA_ClearIntFlag(uint32_t chl);
void DMA_SetSrcMod(uint8_t chl, DMA_Modulo_t sMod);
void DMA_SetDestMod(uint8_t chl, DMA_Modulo_t dMod);
uint32_t DMA_GetTransCnt(uint8_t chl);
void DMA_SetTransCnt(uint8_t chl, uint32_t val);
uint32_t DMA_IsTransDone(uint8_t chl);



#endif


