/**
  ******************************************************************************
  * @file    sdhc.h
  * @author  YANDLD
  * @version V3.0
  * @date    2016.2.13
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#ifndef __CH_LIB_SD_H__
#define __CH_LIB_SD_H__

#include <stdint.h>


/* SD card type */
#define CARD_TYPE_SD        (2)
#define CARD_TYPE_SDHC      (3)

typedef struct 
{
  uint32_t cmd;
  uint32_t arg;
  uint32_t blkSize;
  uint32_t blkCount;
  uint32_t resp[4];
}SDHC_Cmd_t;

/* SDHC pinmux */
#define SDHC_PE00_TO_PE05          (0x00018120)

/* API */
uint32_t SDHC_Init(uint32_t MAP, uint32_t sdClk);
uint32_t SDHC_SendCmd(SDHC_Cmd_t *cmd);
void SDHC_SetSDClk(uint32_t instance, uint32_t sdClk);

/* SDCard API */
uint32_t SD_CardInit(uint32_t instance);
uint32_t SD_GetSizeInMB(void);
uint32_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buf);
uint32_t SD_WriteSingleBlock(uint32_t sector,  uint8_t *buf);
uint32_t SD_ReadMultiBlock(uint32_t sector, uint8_t *buf, uint32_t cnt);
uint32_t SD_WriteMultiBlock(uint32_t sector, uint8_t *buf, uint32_t cnt);

#endif



