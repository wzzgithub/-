/**
  ******************************************************************************
  * @file    rtc.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.06.07
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_RTC_H__
#define __CH_LIB_RTC_H__

#include <stdint.h>
#include "common.h"

/**
 * \struct RTC_DateTime_t
 * \brief RTC初始化结构
 */
typedef struct 
{
   uint16_t year;    ///< 年 从 200到2099.
   uint16_t month;   ///< 月 从 1到12.
   uint16_t day;     ///< 日 从 1到31 (依据月份).
   uint16_t hour;    ///< 时 从 0到23.
   uint16_t minute;  ///< 分 从 0到59.
   uint8_t second;   ///< 秒 从 0到59.
}RTC_DateTime_t;


/**
 * \enum RTC_Int_t
 * \brief RTC interrupt and DMA select
 */
typedef enum
{
    kRTC_IntAlarm,
    kRTC_IntSecond,
}RTC_Int_t;



/* API */
void RTC_Init(void);
void RTC_GetTime(RTC_DateTime_t * datetime);
void RTC_SetTime(RTC_DateTime_t * datetime);
void RTC_SetIntMode(RTC_Int_t config, bool status);
bool RTC_IsTimeValid(void);
void RTC_SetAlarm(RTC_DateTime_t * datetime);
void RTC_GetAlarm(RTC_DateTime_t * datetime);
int  RTC_GetWeek(int year, int month, int days);
uint32_t RTC_GetCounter(void);
uint32_t RTC_GetTSR(void);
void RTC_SetTSR(uint32_t val);

#endif
