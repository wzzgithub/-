/**
  ******************************************************************************
  * @file    ftm.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.5.9
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#ifndef __CH_LIB_FTM_H__
#define __CH_LIB_FTM_H__

#include <stdint.h>
#include <stdbool.h>

/* FTM instance */
#define HW_FTM0    (0x00)
#define HW_FTM1    (0x01)
#define HW_FTM2    (0x02) 
#define HW_FTM3    (0x02) 

/* FTM chl */
#define HW_FTM_CH0 (0x00)
#define HW_FTM_CH1 (0x01)
#define HW_FTM_CH2 (0x02)
#define HW_FTM_CH3 (0x03)
#define HW_FTM_CH4 (0x04)
#define HW_FTM_CH5 (0x05)

/* internal use, FTM mode select */
typedef enum
{
    kFTM_Disabled,
    kFTM_InputCapture,
    kFTM_PWM_EdgeAligned,
    kFTM_PWM_CenterAligned,
}FTM_t;

typedef enum
{
    kFTM_QD_AB,             /* AB相编码器(AB Phase) */
    kFTM_QD_CD,             /* 计数方向式编码器(Counter Direction) */
}FTM_QD_t;

//!< TPM 中断及DMA配置 
typedef enum
{
    kFTM_IntCounterOverFlow = 50,
    kFTM_IntCh0 = 0,
    kFTM_IntCh1 = 1,
    kFTM_IntCh2 = 2,
    kFTM_IntCh3 = 3,
    kFTM_IntCh4 = 4,
    kFTM_IntCh5 = 5,
    kFTM_IntCh6 = 6,
}FTM_Int_t;

#define FTM0_CH4_PB12   (0x205908U)
#define FTM0_CH5_PB13   (0x285b08U)
#define FTM0_CH5_PA00   (0x2840c0U)
#define FTM0_CH6_PA01   (0x3042c0U)
#define FTM0_CH7_PA02   (0x3844c0U)
#define FTM0_CH0_PA03   (0x46c0U)
#define FTM0_CH1_PA04   (0x848c0U)
#define FTM0_CH2_PA05   (0x104ac0U)
#define FTM0_CH3_PA06   (0x184cc0U)
#define FTM0_CH4_PA07   (0x204ec0U)
#define FTM0_CH0_PC01   (0x4310U)
#define FTM0_CH1_PC02   (0x84510U)
#define FTM0_CH2_PC03   (0x104710U)
#define FTM0_CH3_PC04   (0x184910U)
#define FTM0_CH4_PD04   (0x204918U)
#define FTM0_CH5_PD05   (0x284b18U)
#define FTM0_CH6_PD06   (0x304d18U)
#define FTM0_CH7_PD07   (0x384f18U)
#define FTM1_CH0_PB12   (0x58c9U)
#define FTM1_CH1_PB13   (0x85ac9U)
#define FTM1_CH0_PA08   (0x50c1U)
#define FTM1_CH1_PA09   (0x852c1U)
#define FTM1_CH0_PA12   (0x58c1U)
#define FTM1_CH1_PA13   (0x85ac1U)
#define FTM1_CH0_PB00   (0x40c9U)
#define FTM1_CH1_PB01   (0x842c9U)
#define FTM2_CH0_PA10   (0x54c2U)
#define FTM2_CH1_PA11   (0x856c2U)
#define FTM2_CH0_PB18   (0x64caU)
#define FTM2_CH1_PB19   (0x866caU)
#define FTM3_CH0_PE05   (0X00004BA3U)
#define FTM3_CH1_PE06   (0X00084DA3U)
#define FTM3_CH2_PE07   (0X00104FA3U)
#define FTM3_CH3_PE08   (0X001851A3U)
#define FTM3_CH4_PE09   (0X002053A3U)
#define FTM3_CH5_PE10   (0X002855A3U)
#define FTM3_CH6_PE11   (0X003057A3U)
#define FTM3_CH7_PE12   (0X003859A3U)
#define FTM3_CH4_PC08   (0X002050D3U)
#define FTM3_CH5_PC09   (0X002852D3U)
#define FTM3_CH6_PC10   (0X003054D3U)
#define FTM3_CH7_PC11   (0X003856D3U)
#define FTM3_CH0_PD00   (0X0000411BU)
#define FTM3_CH1_PD01   (0X0008431BU)
#define FTM3_CH2_PD02   (0X0010451BU)
#define FTM3_CH3_PD03   (0X0018471BU)

#define FTM1_QD_PHA_PA08_PHB_PA09       (0x9181U)
#define FTM1_QD_PHA_PA12_PHB_PA13       (0x99c1U)
#define FTM1_QD_PHA_PB00_PHB_PB01       (0x8189U)
#define FTM2_QD_PHA_PA10_PHB_PA11       (0x9582U)
#define FTM2_QD_PHA_PB18_PHB_PB19       (0xa58aU)

//!< API functions
uint32_t FTM_PWM_Init(uint32_t MAP, FTM_t mode, uint32_t Hz);
void FTM_PWM_Invert(uint32_t instance, uint8_t chl);
void FTM_PWM_SetDuty(uint32_t instance, uint8_t chl, uint32_t duty);
void FTM_SetIntMode(uint32_t instance, FTM_Int_t mode, bool val);

/* 正交解码器 */
uint32_t FTM_QD_Init(uint32_t MAP, FTM_QD_t mode);
void FTM_QD_Get(uint32_t instance, int16_t* val, uint8_t* dir);



/* 底层函数 */
uint32_t FTM_GetChlCounter(uint32_t instance, uint32_t chl);
void FTM_SetChlCounter(uint32_t instance, uint32_t chl, uint32_t val);
void FTM_SetMoudlo(uint32_t instance, uint32_t val);
uint32_t FTM_GetMoudlo(uint32_t instance);


#endif

