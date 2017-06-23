/**
  ******************************************************************************
  * @file    lptmr.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.06.08
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
	
#ifndef __CH_LPTMR_H__
#define __CH_LPTMR_H__

#include <stdint.h>
#include <stdbool.h>

#define HW_LPTMR0       (0x00)
#define HW_LPTMR1       (0x01)


#define LPTMR_ALT1_PA19   (0X00086780U)
#define LPTMR_ALT2_PC05   (0X00104AD0U)

//!< API functions
void LPTMR_TC_Init(uint32_t instance, uint32_t ms);
void LPTMR_PC_Init(uint32_t MAP, uint32_t polarity, uint32_t overFlowValue);
void LPTMR_SetIntMode(uint32_t instance, bool status);
uint32_t LPTMR_ReadCounter(uint32_t instance);
void LPTMR_ResetCounter(uint32_t instance);
void LPTMR_SetTime(uint32_t instance, uint32_t ms);

#endif

