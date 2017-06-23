/**
  ******************************************************************************
  * @file    lpit.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2015.11.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LPIT_H__
#define __CH_LPIT_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif


//!< hardware instances
#define HW_LPIT_CH0   (0x00U)
#define HW_LPIT_CH1   (0x01U)

typedef struct
{
    bool reloadOnTrigger;          /*!< true: Timer reloads when a trigger is detected;
                                              false: No effect */
    bool stopOnTimeout;            /*!< true: Timer will stop after timeout;
                                              false: does not stop after timeout */
    bool startOnTrigger;           /*!< true: Timer starts when a trigger is detected;
                                              false: decrement immediately */
    uint32_t triggerSource;         
}LPIT_AdvConfig_t;

//!< API functions
void LPIT_SetIntMode(uint32_t chl, bool val);
void LPIT_Init(uint32_t chl, uint32_t us);
void LPIT_SetValue(uint32_t chl, uint32_t val);
void LPIT_SetTime(uint32_t chl, uint32_t us);
uint32_t LPIT_GetValue(uint32_t chl);     
uint32_t LPIT_GetTime(uint32_t chl);
void LPIT_AdvConfig(uint32_t chl, LPIT_AdvConfig_t *config);

#ifdef __cplusplus
}
#endif

#endif

