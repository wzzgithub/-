/**
  ******************************************************************************
  * @file    adc.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.06.03
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_ADC_H__
#define __CH_LIB_ADC_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* ADC instance */
#define HW_ADC0  (0)
     
#if defined(ADC1)
#define HW_ADC1  (1)
#endif

#define ADC0_SE1A_PE16      (0x00086020U)
#define ADC0_SE5A_PE17      (0x00286220U)
#define ADC0_SE2A_PE18      (0x00106420U)
#define ADC0_SE6A_PE19      (0x00306620U)
#define ADC0_SE0A_PE20      (0x00006820U)
#define ADC0_SE4A_PE21      (0x00206A20U)
#define ADC0_SE3A_PE22      (0x00186C20U)
#define ADC0_SE7A_PE23      (0x00386E20U)
#define ADC0_SE4B_PE29      (0x01207A20U)
#define ADC0_SE23A_PE30     (0x00B87C20U)
#define ADC0_SE8A_PB00      (0x00404008U)
#define ADC0_SE9A_PB01      (0x00484208U)
#define ADC0_SE12A_PB02     (0x00604408U)
#define ADC0_SE13A_PB03     (0x00684608U)
#define ADC0_SE14A_PC00     (0x00704010U)
#define ADC0_SE15A_PC01     (0x00784210U)
#define ADC0_SE11A_PC02     (0x00584410U)
#define ADC0_SE5B_PD01      (0x01284218U)
#define ADC0_SE6B_PD05      (0x01304A18U)
#define ADC0_SE7B_PD06      (0x01384C18U)
#define ADC0_SE19_DM0       (0x00980000U)
#define ADC0_SE20_DM1       (0x00A00000U)

/* ADC 通道复用选择 */
typedef enum
{
    kADC_ChlMuxA,
    kADC_ChlMuxB,
}ADC_ChlMux_t;
    
/* ADC 速度选择 */
typedef enum
{
    kADC_SpeedHigh,
    kADC_SpeedMiddle,
    kADC_SpeedLow,
}ADC_Speed_t;

/* ADC 触发模式 */
typedef enum
{
    kTrgSoft,   /* 软件触发 */
    kTrgHard,   /* 硬件触发 */
}ADC_Trg_t;



/* 功能接口 */
uint32_t ADC_Init(uint32_t MAP);
void ADC_SetSpeed(uint32_t instance, ADC_Speed_t speed);
int32_t ADC_SoftRead(uint32_t instance, uint32_t chl);
void ADC_SetIntMode(uint32_t instance, bool val);
void ADC_SetTrigMode(uint32_t instance, ADC_Trg_t trig);
void ADC_SetChlMux(uint32_t instance, ADC_ChlMux_t mux);
void ADC_SoftTrigger(uint32_t instance, uint32_t chl);

#ifdef __cplusplus
}
#endif


#endif
