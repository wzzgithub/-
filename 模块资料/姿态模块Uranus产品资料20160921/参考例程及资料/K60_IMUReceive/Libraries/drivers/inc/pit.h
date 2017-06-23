/**
  ******************************************************************************
  * @file    pit.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.05.31
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_PIT_H__
#define __CH_LIB_PIT_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif


//!< hardware instances
#define HW_PIT_CH0   (0x00U)
#define HW_PIT_CH1   (0x01U)

//!< API ¹¦ÄÜº¯Êý
void PIT_Init(uint32_t chl, uint32_t us);
uint32_t PIT_SetIntMode(uint32_t chl, bool val);
void PIT_SetValue(uint8_t chl, uint32_t val);
uint32_t PIT_GetValue(uint32_t chl);
void PIT_SetTime(uint32_t chl, uint32_t us);
uint32_t PIT_GetTime(uint32_t chl);
     
#ifdef __cplusplus
}
#endif

#endif

