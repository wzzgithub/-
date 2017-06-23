/**
  ******************************************************************************
  * @file    lptmr.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.06.08
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "common.h"
#include "lptmr.h"

#ifndef LPTMR_BASES
#define LPTMR_BASES {LPTMR0};
#endif

LPTMR_Type * const LPTMRBases[] = LPTMR_BASES;

#if defined(SIM_SCGC5_LPTMR_MASK)
static const Reg_t LPTMRClkGate[] =
{
    {(void*)&(SIM->SCGC5), SIM_SCGC5_LPTMR_MASK, SIM_SCGC5_LPTMR_SHIFT},
};
#elif defined(SIM_SCGC5_LPTIMER_MASK)
static const Reg_t LPTMRClkGate[] =
{
    {(void*)&(SIM->SCGC5), SIM_SCGC5_LPTIMER_MASK, SIM_SCGC5_LPTIMER_SHIFT},
};
#elif defined(PCC0_PCC_LPTMR0_CGC_MASK)
static const Reg_t LPTMRClkGate[] =
{
    {(void*)&(PCC0->PCC_LPTMR0), PCC0_PCC_LPTMR0_CGC_MASK, PCC0_PCC_LPTMR0_CGC_SHIFT},
#if defined(PCC1_PCC_LPTMR1_CGC_MASK)
    {(void*)&(PCC1->PCC_LPTMR1), PCC1_PCC_LPTMR1_CGC_MASK, PCC1_PCC_LPTMR1_CGC_SHIFT},
#endif
};
#else
#error "No LPTMR clock gate defined!"
#endif

#if defined(LPTMR_IRQS)
static const IRQn_Type LPTMR_IRQTbl[] = LPTMR_IRQS;
#else
static const IRQn_Type LPTMR_IRQTbl[] = 
{
    #if (defined(MKL03Z4) || defined(MKV10Z7) || defined(MK26F18))
    LPTMR0_IRQn,
    #elif defined(MK80F25615)
    LPTMR0_LPTMR1_IRQn,
    #else
    LPTimer_IRQn,
    #endif
};
#endif /* LPTMR_IRQS */


uint32_t LPTMR_GetClock(uint32_t instance, uint32_t option)
{
    LPTMR_Type *LPTMRx = (LPTMR_Type*)LPTMRBases[instance];
    LPTMRx->PSR |= LPTMR_PSR_PCS(option);
    return 1000;
}

 /**
 * @brief  初始化配置LPTMR工作在计时器模式
 * @note   计时器单位是ms
 * @param  instance:
 *         @arg HW_LPTMR0 : LPTMR0模块
 *         @arg HW_LPTMR1 : LPTMR1模块
 * @param  ms: 计时时间
 * @retval None
 */
void LPTMR_TC_Init(uint32_t instance, uint32_t ms)
{
    REG_SET(LPTMRClkGate, instance);
    LPTMR_Type *LPTMRx = (LPTMR_Type*)LPTMRBases[instance];
    
    LPTMRx->CSR = 0x00; 
    LPTMRx->PSR = 0x00;
    LPTMRx->CMR = 0x00;
    
    /* disable module first */
    LPTMRx->CSR &= ~LPTMR_CSR_TEN_MASK;
    
    /* free counter will reset whenever compare register is writtened. */
    LPTMRx->CSR &= ~LPTMR_CSR_TFC_MASK;  
    
    /* timer counter mode */
    LPTMRx->CSR &= ~LPTMR_CSR_TMS_MASK; 
    
	/* bypass the prescaler, soruce clock is LPO 1KHZ */
    LPTMR_GetClock(instance, 1);
    LPTMRx->PSR |= LPTMR_PSR_PBYP_MASK; 
    
    /* set CMR(compare register) */
    LPTMRx->CMR = LPTMR_CMR_COMPARE(ms);
    
    /* enable moudle */
    LPTMRx->CSR |= LPTMR_CSR_TEN_MASK;  
}

 /**
 * @brief  设置LPTMR中断模式
 * @note   None
 * @param  instance:
 *         @arg HW_LPTMR0 : LPTMR0模块
 *         @arg HW_LPTMR1 : LPTMR1模块
 * @param  status: 
 *         @arg true : 开启中断
 *         @arg flase : 关闭中断
 * @retval None
 */
void LPTMR_SetIntMode(uint32_t instance, bool status)
{
    REG_SET(LPTMRClkGate, instance);
    
    NVIC_EnableIRQ(LPTMR_IRQTbl[instance]);

    (status)?
    (LPTMRBases[instance]->CSR |= LPTMR_CSR_TIE_MASK):
    (LPTMRBases[instance]->CSR &= ~LPTMR_CSR_TIE_MASK);
}

 /**
 * @brief  初始化配置LPTMR模块在脉冲计数模式
 * @note   None
 * @param  MAP ：引脚位图信息，详见lptmr.h
 * @param  polarity:
 *         @arg 0 : 下降沿脉冲计数
 *         @arg 1 : 上升沿脉冲计数
 * @param  overFlowValue：设置计数器计数最大值，以便设计中断
 * @retval None
 */
void LPTMR_PC_Init(uint32_t MAP, uint32_t polarity, uint32_t overFlowValue)
{
    uint32_t instance;
    
    map_t * pq = (map_t*)&(MAP);
    instance = pq->ip;
    LPTMR_Type *LPTMRx = (LPTMR_Type*)LPTMRBases[instance];
    REG_SET(LPTMRClkGate, instance);
    
    LPTMRx->CSR = 0x00; 
    LPTMRx->PSR = 0x00;
    LPTMRx->CMR = 0x00;
    
    /* disable module first */
    LPTMRx->CSR &= ~LPTMR_CSR_TEN_MASK;
    
    /* free counter will reset whenever compare register is writtened. */
    LPTMRx->CSR &= ~LPTMR_CSR_TFC_MASK;  
    
    /* pulse counter mode */
    LPTMRx->CSR |= LPTMR_CSR_TMS_MASK; 
    
	/* bypass the glitch filter, which mean we use 1KHZ LPO directly */
    LPTMR_GetClock(instance, 1);
    LPTMRx->PSR |= LPTMR_PSR_PBYP_MASK;
    
    /* set CMR(compare register) */
    LPTMRx->CMR = LPTMR_CMR_COMPARE(overFlowValue);
    
    /* input source */
    LIB_TRACE("LPTMR pin alt:%d\r\n", pq->chl);
    LPTMRx->CSR |= LPTMR_CSR_TPS(pq->chl);

    /* pin polarity */
    (polarity)?
    (LPTMRx->CSR |= LPTMR_CSR_TPP_MASK):
    (LPTMRx->CSR &= ~LPTMR_CSR_TPP_MASK);
    
    /* enable moudle */
    LPTMRx->CSR |= LPTMR_CSR_TEN_MASK; 
    
    PIN_SET_MUX;
}

 /**
 * @brief  读取LPTMR计数器计数值
 * @note   None
 * @param  instance:
 *         @arg HW_LPTMR0 : LPTMR0模块
 *         @arg HW_LPTMR1 : LPTMR1模块
 * @retval 返回计数器的计数值
 */
uint32_t LPTMR_ReadCounter(uint32_t instance)
{
    LPTMR_Type *LPTMRx = (LPTMR_Type*)LPTMRBases[instance];
    /* must first read */
    uint32_t *p = (uint32_t *)&LPTMRx->CNR;
    *p = 0x1234;
	return (uint32_t)((LPTMRx->CNR & LPTMR_CNR_COUNTER_MASK) >> LPTMR_CNR_COUNTER_SHIFT); 
}

 /**
 * @brief  设置LPTMR计时器时间
 * @note   None
 * @param  instance:
 *         @arg HW_LPTMR0 : LPTMR0模块
 *         @arg HW_LPTMR1 : LPTMR1模块
 * @param  ms：时间 ms
 * @retval None
 */
void LPTMR_SetTime(uint32_t instance, uint32_t ms)
{
    LPTMR_Type *LPTMRx = (LPTMR_Type*)LPTMRBases[instance];
    LPTMRx->CSR &= ~LPTMR_CSR_TEN_MASK; 
    LPTMRx->CMR = LPTMR_CMR_COMPARE(ms);
    LPTMRx->CSR |= LPTMR_CSR_TEN_MASK; 
}

 /**
 * @brief  重设LPTMR计时器时间
 * @note   清除计数器
 * @param  instance:
 *         @arg HW_LPTMR0 : LPTMR0模块
 *         @arg HW_LPTMR1 : LPTMR1模块
 * @retval None
 */
void LPTMR_ResetCounter(uint32_t instance)
{
    LPTMR_Type *LPTMRx = (LPTMR_Type*)LPTMRBases[instance];
    /* disable and reenable moudle to clear counter */
    LPTMRx->CSR &= ~LPTMR_CSR_TEN_MASK;
    LPTMRx->CSR |= LPTMR_CSR_TEN_MASK;
}



void LPTMR_IRQHandler(uint32_t instance)
{
    LPTMRBases[instance]->CSR |= LPTMR_CSR_TCF_MASK;
}

//static const map_t MAPTbl[] = 
//{
//    { 0, 0, 6,19, 1, 1}, //LPTMR_ALT1_PA19 6
//    { 0, 2, 3, 5, 1, 2}, //LPTMR_ALT2_PC05 4
//};


