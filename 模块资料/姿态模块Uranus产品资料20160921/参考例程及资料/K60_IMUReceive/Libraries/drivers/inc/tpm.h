/**
  ******************************************************************************
  * @file    tpm.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.6.12
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#ifndef __CH_LIB_TPM_H__
#define __CH_LIB_TPM_H__

#include <stdint.h>
#include <stdbool.h>

/* TPM instance */
#define HW_TPM0    (0x00)
#define HW_TPM1    (0x01)
#if defined(TPM2)
#define HW_TPM2    (0x02) 
#endif

/* TPM chl */
#define HW_TPM_CH0 (0x00)
#define HW_TPM_CH1 (0x01)
#define HW_TPM_CH2 (0x02)
#define HW_TPM_CH3 (0x03)
#define HW_TPM_CH4 (0x04)
#define HW_TPM_CH5 (0x05)

/* internal use, FTM mode select */
typedef enum
{
    kTPM_Disabled,
    kTPM_InputCapture,
    kTPM_PWM_EdgeAligned,
    kTPM_PWM_CenterAligned,
}TPM_t;

//!< TPM 中断及DMA配置 
typedef enum
{
    kTPM_IntCounterOverFlow = 50,
    kTPM_IntCh0 = 0,
    kTPM_IntCh1 = 1,
    kTPM_IntCh2 = 2,
    kTPM_IntCh3 = 3,
    kTPM_IntCh4 = 4,
    kTPM_IntCh5 = 5,
}TPM_Int_t;

/* FTM MAP info */
#define TPM0_CH0_PE24   (0x000070E0U)
#define TPM0_CH0_PA03   (0x000046C0U)
#define TPM0_CH0_PC01   (0x00004310U)
#define TPM0_CH0_PD00   (0x00004118U)
#define TPM0_CH1_PE25   (0x000872E0U)
#define TPM0_CH1_PA04   (0x000848C0U)
#define TPM0_CH1_PC02   (0x00084510U)
#define TPM0_CH1_PD01   (0x00084318U)
#define TPM0_CH2_PD02   (0x00104518U)
#define TPM0_CH2_PE29   (0x00107AE0U)
#define TPM0_CH2_PA05   (0x00104AC0U)
#define TPM0_CH2_PC03   (0x00104710U)
#define TPM0_CH3_PD03   (0x00184718U)
#define TPM0_CH3_PE30   (0x00187CE0U)
#define TPM0_CH3_PA06   (0x00184CC0U)
#define TPM0_CH3_PC04   (0x00184910U)
#define TPM0_CH4_PC08   (0x002050D0U)
#define TPM0_CH4_PD04   (0x00204918U)
#define TPM0_CH4_PE31   (0x00207EE0U)
#define TPM0_CH4_PA07   (0x00204EC0U)
#define TPM0_CH5_PE26   (0x002874E0U)
#define TPM0_CH5_PA00   (0x002840C0U)
#define TPM0_CH5_PC09   (0x002852D0U)
#define TPM0_CH5_PD05   (0x00284B18U)
#define TPM1_CH0_PA12   (0x000058C1U)
#define TPM1_CH0_PB00   (0x000040C9U)
#define TPM1_CH0_PE20   (0x000068E1U)
#define TPM1_CH1_PE21   (0x00086AE1U)
#define TPM1_CH1_PA13   (0x00085AC1U)
#define TPM1_CH1_PB01   (0x000842C9U)
#define TPM2_CH0_PA01   (0x000042C2U)
#define TPM2_CH0_PB02   (0x000044CAU)
#define TPM2_CH0_PB18   (0x000064CAU)
#define TPM2_CH1_PE23   (0x00086EE2U)
#define TPM2_CH1_PA02   (0x000844C2U)
#define TPM2_CH1_PB03   (0x000846CAU)
#define TPM2_CH1_PB19   (0x000866CAU)

/* API 功能接口 */
uint32_t TPM_PWM_Init(uint32_t MAP, TPM_t mode, uint32_t Hz);
void TPM_PWM_Invert(uint32_t instance, uint8_t chl);
void TPM_PWM_SetDuty(uint32_t instance, uint8_t chl, uint32_t duty);
void TPM_SetIntMode(uint32_t instance, TPM_Int_t mode, bool val);

void TPM_SetChlCounter(uint32_t instance, uint32_t chl, uint32_t val);
uint32_t TPM_GetChlCounter(uint32_t instance, uint32_t chl);
void TPM_SetMoudlo(uint32_t instance, uint32_t val);
uint32_t TPM_GetMoudlo(uint32_t instance);


#endif

