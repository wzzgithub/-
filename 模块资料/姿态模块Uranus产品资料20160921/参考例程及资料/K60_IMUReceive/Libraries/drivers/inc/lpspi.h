/**
  ******************************************************************************
  * @file    lpspi.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.03.28
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_LPSPI_H__
#define __CH_LIB_LPSPI_H__

#include <stdint.h>
#include "common.h"

/*!< LPSPI instance */
#define HW_LPSPI0  (0x00U)
#define HW_LPSPI1  (0x01U)

uint32_t LPSPI_Init(uint32_t MAP, uint32_t baudrate);
uint32_t LPSPI_ReadWrite(uint32_t instance, uint32_t data);
void LPSPI_SelectCS(uint32_t instance, uint32_t cs);
void LPSPI_SetCS(uint32_t instance, uint32_t state);


#endif
