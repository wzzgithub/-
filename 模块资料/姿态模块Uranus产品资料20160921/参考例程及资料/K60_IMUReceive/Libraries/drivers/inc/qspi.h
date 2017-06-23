/**
  ******************************************************************************
  * @file    qspi.h
  * @author  YANDLD
  * @version V3.0
  * @date    2016.7.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_QSPI_H__
#define __CH_LIB_QSPI_H__

#include <stdint.h>
#include <stdbool.h>

#define FSL_FEATURE_QSPI_AMBA_BASE (0x68000000U)
#define FSL_FEATURE_QSPI_ARDB_BASE (0x67000000U)

typedef struct
{
    uint32_t flashA1Size;                             /*!< Flash A1 size */
    uint32_t flashA2Size;                             /*!< Flash A2 size */
    uint32_t flashB1Size;                             /*!< Flash B1 size */
    uint32_t flashB2Size;                             /*!< Flash B2 size */
    uint32_t lookuptable[64];                         /*!< Flash command in LUT */
    uint32_t dataHoldTime;                            /*!< Data line hold time. */
    uint32_t CSHoldTime;                              /*!< CS line hold time */
    uint32_t CSSetupTime;                             /*!< CS line setup time*/
    uint32_t cloumnspace;                             /*!< Column space size */
    uint32_t dataLearnValue;                          /*!< Data Learn value if enable data learn */
    bool enableWordAddress;                           /*!< If enable word address.*/
} QSPI_FlashConfig_t;

uint32_t QSPI_Init(uint32_t MAP, uint32_t baud);
void QSPI_ConfigFlash(QSPI_FlashConfig_t *config);

/* QSPI spi flash API */
uint32_t QSPI_CmdEraseSector(uint32_t addr);
uint32_t QSPI_CmdProgramPage(uint32_t addr, uint32_t *buf);
uint32_t QSPI_CmdWriteSector(uint32_t addr, uint32_t *buf);
uint32_t QSPI_EnableQuadMode(void);
uint32_t QSPI_Test(uint32_t addr, uint32_t size);
uint32_t QSPI_LoadImage(uint32_t addr, uint32_t* image, uint32_t len);
#endif
