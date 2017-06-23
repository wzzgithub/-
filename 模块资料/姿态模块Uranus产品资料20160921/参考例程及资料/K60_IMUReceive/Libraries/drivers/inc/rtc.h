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
 * \brief RTC��ʼ���ṹ
 */
typedef struct 
{
   uint16_t year;    ///< �� �� 200��2099.
   uint16_t month;   ///< �� �� 1��12.
   uint16_t day;     ///< �� �� 1��31 (�����·�).
   uint16_t hour;    ///< ʱ �� 0��23.
   uint16_t minute;  ///< �� �� 0��59.
   uint8_t second;   ///< �� �� 0��59.
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
