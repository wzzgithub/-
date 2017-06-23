/**
  ******************************************************************************
  * @file    flexio.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.6.13
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#ifndef __CH_LIB_FLEXIO_H__
#define __CH_LIB_FLEXIO_H__

#include <stdint.h>
#include <stdbool.h>


#define HW_FLEXIO_UART0         (0)
#define HW_FLEXIO_UART1         (2)
#define HW_FLEXIO_PWM0          (0)
#define HW_FLEXIO_PWM1          (1)

#define HW_FLEXIO_D0          (0)
#define HW_FLEXIO_D1          (1)
#define HW_FLEXIO_D2          (2)
#define HW_FLEXIO_D3          (3)
#define HW_FLEXIO_D4          (4)
#define HW_FLEXIO_D5          (5)
#define HW_FLEXIO_D6          (6)
#define HW_FLEXIO_D7          (7)


//!< API functions
void FLEXIO_Init(void);

/* FLEXIO PWM */
void FLEXIO_PWM_Init(uint32_t chl, uint32_t pin, uint32_t freq);
void FLEXIO_PWM_Start(uint32_t chl);
void FLEXIO_PWM_Stop(uint32_t chl, uint32_t pin);

/* FLEXIO UART */
void FLEXIO_UART_Init(uint32_t instance, uint32_t baudrate, uint32_t tx_pin, uint32_t rx_pin);
void FLEXIO_UART_PutChar(uint32_t instance, uint8_t ch);
uint8_t FLEXIO_UART_GetChar(uint32_t instance, uint8_t *ch);

/* Low Level API */
//void FLEXIO_SetShifterConfig(FLEXIO_Type *base, uint8_t index, const flexio_shifter_config_t *shifterConfig);
//void FLEXIO_SetTimerConfig(FLEXIO_Type *base, uint8_t index, const flexio_timer_config_t *timerConfig);


#endif

