/**
  ******************************************************************************
  * @file    adc.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.07.05
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "adc.h"
#include "common.h"

#ifndef ADC_BASES
#define ADC_BASES {ADC0};
#endif

ADC_Type * const ADCBases[] = ADC_BASES;

#if defined(SIM_SCGC6_ADC0_MASK)
static const Reg_t CLKTbl[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_ADC0_MASK},
#if defined(SIM_SCGC3_ADC1_MASK)
    {(void*)&(SIM->SCGC3), SIM_SCGC3_ADC1_MASK},
#endif
};
#elif defined(PCC0_PCC_ADC0_CGC_MASK)
static const Reg_t CLKTbl[] =
{
    {(void*)&(PCC0->PCC_ADC0), PCC0_PCC_ADC0_CGC_MASK, PCC0_PCC_ADC0_CGC_SHIFT},
};
#else
#error "no ADC clock gate defined!"
#endif

static const IRQn_Type ADC_IrqTbl[] = 
{
    (IRQn_Type)(ADC0_IRQn + 0),
};

/**
 * @brief  设置ADC模块引脚路由
 * @note   每个ADC通道可能连接外部2个引脚，称为A或者B, 次函数选择其中之一
 * @param  instance:
 *         @arg HW_ADC0 : ADC0模块
 *         @arg HW_ADC1 : ADC1模块
 * @param  mux:
 *         @arg kADC_ChlMuxA : AD模块A通道
 *         @arg kADC_ChlMuxB : AD模块B通道
 * @retval None
 */
void ADC_SetChlMux(uint32_t instance, ADC_ChlMux_t mux)
{
    (kADC_ChlMuxA == mux)?
    (ADCBases[instance]->CFG2 &= ~ADC_CFG2_MUXSEL_MASK):
    (ADCBases[instance]->CFG2 |= ADC_CFG2_MUXSEL_MASK); 
    LIB_TRACE("ADC chl mux set to:%d\r\n", mux);
}

/**
 * @brief  初始化配置AD模块
 * @note   None
 * @param  MAP: AD模块引脚位图，详见adc.h文件
 * @param  speed : 
 *         @arg kADC_SpeedHigh : 高速度
 *         @arg kADC_SpeedMiddle : 中速度
 *         @arg kADC_SpeedLow : 低速度
 * @retval HW_ADC0或HW_ADC1
 */
uint32_t ADC_Init(uint32_t MAP)
{
    map_t * pq = (map_t*)&(MAP);
    ADC_Type* ADCx = ADCBases[pq->ip];
    
    REG_SET(CLKTbl, pq->ip);
    
    /* select the ADACK ,16bit Long sample time */
    ADCx->CFG2 |=  ADC_CFG2_ADACKEN_MASK;
    ADCx->CFG1 = ADC_CFG1_ADICLK(3) | ADC_CFG1_MODE(3) | ADC_CFG1_ADLPC_MASK;
    
    /* use VREH and VREFL as V reference */
    ADCx->SC2 &= ~ADC_SC2_REFSEL_MASK;
    ADCx->SC2 |= ADC_SC2_REFSEL(0);
    
    /* software trigger */
    ADCx->SC2 &= ~ADC_SC2_ADTRG_MASK;
    
    ADC_SetSpeed(pq->ip, kADC_SpeedMiddle);
    
    ADC_SetChlMux(pq->ip, (ADC_ChlMux_t)(pq->reserved & 0x01));
    
    /* selecy single or diff */
    #if defined(ADC_SC1_DIFF_MASK)
    (pq->reserved & 0x80)?
    (ADCBases[pq->ip]->SC1[0] |= ADC_SC1_DIFF_MASK):
    (ADCBases[pq->ip]->SC1[0] &= ~ADC_SC1_DIFF_MASK);
    #endif
    
    /* select chl */
    ADCBases[pq->ip]->SC1[0] &= ~(ADC_SC1_ADCH_MASK);
    ADCBases[pq->ip]->SC1[0] |= ADC_SC1_ADCH(pq->chl);
    LIB_TRACE("ADC set chl to:%d\r\n", pq->chl);
    
    PIN_SET_MUX;
    
    return pq->ip;
}

void ADC_SetSpeed(uint32_t instance, ADC_Speed_t speed)
{
    ADC_Type* ADCx = ADCBases[instance];
    switch(speed)
    {
        case kADC_SpeedHigh:
            ADCx->CFG1 &= ~(ADC_CFG1_ADLSMP_MASK | ADC_CFG1_ADIV_MASK);
            ADCx->CFG1 |= ADC_CFG1_ADIV(0);
            break;
        case kADC_SpeedMiddle:
            ADCx->CFG1 &= ~(ADC_CFG1_ADLSMP_MASK | ADC_CFG1_ADIV_MASK);
            ADCx->CFG1 |= ADC_CFG1_ADIV(1);
            break;
        case kADC_SpeedLow:
            ADCx->CFG1 &= ~(ADC_CFG1_ADLSMP_MASK | ADC_CFG1_ADIV_MASK);
            ADCx->CFG1 |= ADC_CFG1_ADIV(3) | ADC_CFG1_ADLSMP_MASK;
            ADCx->CFG2 |= ADC_CFG2_ADLSTS(3) | ADC_CFG2_ADHSC_MASK;
            break;
        default:
            break;
    }
}

/**
 * @brief  设置AD模块触发模式
 * @note   None
 * @param  instance:
 *         @arg HW_ADC0 : ADC0模块
 *         @arg HW_ADC1 : ADC1模块
 * @param  trig:
 *         @arg TrigSoft : AD模块软件触发
 *         @arg TrigHard : AD模块硬件触发
 * @retval None
 */
void ADC_SetTrigMode(uint32_t instance, ADC_Trg_t trig)
{
    (trig == kTrgHard)?
    (ADCBases[instance]->SC2 |= ADC_SC2_ADTRG_MASK):
    (ADCBases[instance]->SC2 &= ~ADC_SC2_ADTRG_MASK);
}

/**
 * @brief  读取AD模块转换结果
 * @note   None
 * @param  instance:
 *         @arg HW_ADC0 : ADC0模块
 *         @arg HW_ADC1 : ADC1模块
 * @param  ch1: AD引脚通道
 * @retval AD转换结果
 */
int32_t ADC_SoftRead(uint32_t instance, uint32_t chl)
{
    ADC_SoftTrigger(instance, chl);
    if(!(ADCBases[instance]->SC1[0] & ADC_SC1_AIEN_MASK))
    {
        while((ADCBases[instance]->SC1[0] & ADC_SC1_COCO_MASK) == 0);
    }
    return ADCBases[instance]->R[0];
}

/**
 * @brief  软件触发AD转换
 * @note   None
 * @param  instance:
 *         @arg HW_ADC0 : ADC0模块
 *         @arg HW_ADC1 : ADC1模块
 * @param  ch1: AD引脚通道
 * @retval None
 */
void ADC_SoftTrigger(uint32_t instance, uint32_t chl)
{
    ADCBases[instance]->SC1[0] &= ~(ADC_SC1_ADCH_MASK);
    ADCBases[instance]->SC1[0] |= ADC_SC1_ADCH(chl);
}

/**
 * @brief  设置AD中断模式
 * @note   None
 * @param  instance:
 *         @arg HW_ADC0 : ADC0模块
 *         @arg HW_ADC1 : ADC1模块
 * @param  val: 
 *         @arg true : 开启中断
 *         @arg false : 关闭中断
 * @retval None
 */
void ADC_SetIntMode(uint32_t instance, bool val)
{
    (val)?
    (ADCBases[instance]->SC1[0] |= ADC_SC1_AIEN_MASK):
    (ADCBases[instance]->SC1[0] &= ~ADC_SC1_AIEN_MASK);
    NVIC_EnableIRQ(ADC_IrqTbl[instance]); 
}

//void ADC_SetAveMode(uint32_t instance, ADC_Ave_t mod)
//{
//    ADC_Type* ADCx = ADCBases[instance];
//    
//    if(mod == kADC_Ave1)
//    {
//        ADCx->SC3 &= ~ADC_SC3_AVGE_MASK;
//    }
//    else
//    {
//        ADCx->SC3 |= ADC_SC3_AVGE_MASK;
//        ADCx->SC3 &= ~ADC_SC3_AVGS_MASK;
//        ADCx->SC3 |= ADC_SC3_AVGS(mod);
//    }
//}

void ADC_IRQHandler(uint32_t instance)
{
    volatile uint32_t dummy;
    if(ADCBases[instance]->SC1[0] & ADC_SC1_COCO_MASK)
    {
        dummy = ADCBases[instance]->R[0];
    }
}


/*
reserved: [bit.7]: 0-SE 1-DIFF
reserved: [bit.0]: 0-A  1-B
const map_t ADC_QuickInitTable[] =
{
    { 0, 4, 0,16, 1, 1, 0}, //ADC0_SE1A_PE16
    { 0, 4, 0,17, 1, 5, 0}, //ADC0_SE5A_PE17
    { 0, 4, 0,18, 1, 2, 0}, //ADC0_SE2A_PE18
    { 0, 4, 0,19, 1, 6, 0}, //ADC0_SE6A_PE19
    { 0, 4, 0,20, 1, 0, 0}, //ADC0_SE0A_PE20
    { 0, 4, 0,21, 1, 4, 0}, //ADC0_SE4A_PE21
    { 0, 4, 0,22, 1, 3, 0}, //ADC0_SE3A_PE22
    { 0, 4, 0,23, 1, 7, 0}, //ADC0_SE7A_PE23
    { 0, 4, 0,29, 1, 4, 1}, //ADC0_SE4B_PE29
    { 0, 4, 0,30, 1,23, 0}, //ADC0_SE23A_PE30
    { 0, 1, 0, 0, 1, 8, 0}, //ADC0_SE8A_PB00
    { 0, 1, 0, 1, 1, 9, 0}, //ADC0_SE9A_PB01
    { 0, 1, 0, 2, 1,12, 0}, //ADC0_SE12A_PB02
    { 0, 1, 0, 3, 1,13, 0}, //ADC0_SE13A_PB03
    { 0, 2, 0, 0, 1,14, 0}, //ADC0_SE14A_PC00
    { 0, 2, 0, 1, 1,15, 0}, //ADC0_SE15A_PC01
    { 0, 2, 0, 2, 1,11, 0}, //ADC0_SE11A_PC02
    { 0, 3, 0, 1, 1, 5, 1}, //ADC0_SE5B_PD01
    { 0, 3, 0, 5, 1, 6, 1}, //ADC0_SE6B_PD05
    { 0, 3, 0, 6, 1, 7, 1}, //ADC0_SE7B_PD06
};
*/

