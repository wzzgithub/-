/**
  ******************************************************************************
  * @file    tpm.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.06.12
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "tpm.h"
#include "common.h"

#if defined(TPM0)

/* global vars */
#if defined(TPM_BASE_PTRS)
#define TPM_BASES   TPM_BASE_PTRS
#endif

TPM_Type * const TPMBases[] = TPM_BASES;

static const uint32_t TPM_ChlNum[] = {5,2,2}; /* How many chl in a TPM instance */

#if defined(SIM_SCGC6_TPM0_MASK)
static const Reg_t TPMClkGate[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_TPM0_MASK},
#if defined(SIM_SCGC6_TPM1_MASK)
    {(void*)&(SIM->SCGC6), SIM_SCGC6_TPM1_MASK},
#endif
#if defined(SIM_SCGC6_TPM2_MASK)
    {(void*)&(SIM->SCGC6), SIM_SCGC6_TPM2_MASK},
#endif
};
#elif defined(PCC1_PCC_TPM0_CGC_MASK)
static const Reg_t TPMClkGate[] =
{
    {(void*)&(PCC1->PCC_TPM0), PCC1_PCC_TPM0_CGC_MASK},
    {(void*)&(PCC1->PCC_TPM1), PCC1_PCC_TPM1_CGC_MASK},
};
#else
#error "No TPM clock gate defined!"
#endif

static const IRQn_Type TPM_IRQTbl[] = 
{
    (IRQn_Type)(TPM0_IRQn + 0),
    (IRQn_Type)(TPM0_IRQn + 1),
    (IRQn_Type)(TPM0_IRQn + 2),
};


void TPM_SetMoudleParam(uint32_t instance, uint32_t modulo, uint32_t ps)
{
    /* disable FTM, we must set CLKS(0) before config FTM! */
    TPMBases[instance]->SC &= ~(TPM_SC_PS_MASK | TPM_SC_CMOD_MASK);
    
    TPMBases[instance]->CNT = 0;
    TPMBases[instance]->MOD = modulo;
    
    TPMBases[instance]->SC &= ~TPM_SC_PS_MASK;
    TPMBases[instance]->SC |= TPM_SC_PS(ps) | TPM_SC_CMOD(1);
    
    LIB_TRACE("TPM ps:%d\r\n", (1<<ps));
    LIB_TRACE("TPM modulo:%d\r\n", modulo);
}


static uint32_t TPMGetClock(uint32_t instance)
{
    /* for KL27, TPM clock is fixed to IRC 48M */
    #if defined(SIM_SOPT2_TPMSRC_MASK)
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    return 48*1000*1000;
    #elif defined(PCC1_PCC_TPM0_PCS_MASK)
    REG_CLR(TPMClkGate, instance);
    *((uint32_t*)TPMClkGate[instance].addr) = PCC1_PCC_TPM0_PCS(3);
    REG_SET(TPMClkGate, instance);
    return 48*1000*1000;
    #endif
}

 /**
 * @brief  初始化TPM模块为PWM模式
 * @note   默认输出占空比0%
 * @param  MAP: 初始化信息，详见tpm.h文件
 * @param  mode: PWM模式，详见tpm.h文件
 * @param  Hz: 输出频率
 * @retval CH_OK ：成功；其他:失败
 */
uint32_t TPM_PWM_Init(uint32_t MAP, TPM_t mode, uint32_t Hz)
{
    map_t * pq = (map_t*)&(MAP);
    uint32_t instance = pq->ip;
    TPM_Type *TPMx = (TPM_Type*)TPMBases[instance];
    uint32_t clk = TPMGetClock(pq->ip);
    int32_t i, modulo;
    uint8_t ps = 0;
    
    REG_SET(TPMClkGate, instance);
    TPMx->SC = 0;
    TPMx->CNT = 0;
    
    if(Hz)
    {
        switch(mode)
        {
            case kTPM_Disabled:
                TPMx->CONTROLS[pq->chl].CnSC &= ~(TPM_CnSC_MSA_MASK | TPM_CnSC_MSB_MASK);
                break;
            case kTPM_InputCapture:
                
                break;
            case kTPM_PWM_EdgeAligned:
                TPMx->CONTROLS[pq->chl].CnSC |= TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
                TPMx->CONTROLS[pq->chl].CnSC &= ~TPM_CnSC_MSA_MASK;
                TPMx->SC &= ~TPM_SC_CPWMS_MASK;
                break;
            case kTPM_PWM_CenterAligned:
                TPMx->CONTROLS[pq->chl].CnSC |= TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
                TPMx->CONTROLS[pq->chl].CnSC &= ~TPM_CnSC_MSA_MASK;
                TPMx->SC |= TPM_SC_CPWMS_MASK;
                Hz *= 2;
                break;
        }
    }
    else
    {
        TPMx->CONTROLS[pq->chl].CnSC &= ~(TPM_CnSC_MSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK);
    }
    
    /* calucate modulo and ps */
    for(i=0; i<7; i++)
    {
        if((clk/Hz)/(1<<i) < TPM_MOD_MOD_MASK)
        {
            ps = i;
            modulo = (clk/Hz)/(1<<i) - 1;
            break;
        }
    }
    
    if(i == 7)
    {
        LIB_TRACE("error! cannot reach %dHz!\r\n", Hz);
        ps = 7;
        modulo = TPM_MOD_MOD_MASK - 1;
    }
    
    LIB_TRACE("target frequency:%dHz\r\n", Hz);
    LIB_TRACE("input clk:%d\r\n", clk);
    
    TPMx->CONTROLS[pq->chl].CnV = 0;
    TPM_SetMoudleParam(instance, modulo, ps);
    
    PIN_SET_MUX;
    return pq->ip;
}

/**
 * @brief  改变PWM输出极性    
 * @note   调转PWM输出极性
 * @param  instance:
 *         @arg HW_TPM0 : TPM0模块
 *         @arg HW_TPM1 : TPM1模块
 * @param  chl : PWM通道
 * @retval None
 */
void TPM_PWM_Invert(uint32_t instance, uint8_t chl)
{
    #if defined(TPM_POL_POL0_MASK)
    TPM_Type *TPMx = (TPM_Type*)TPMBases[instance];
    (TPMx->POL & (1<<chl))?(TPMx->POL &= ~(1<<chl)):(TPMx->POL |= (1<<chl));
    #endif
}

/**
 * @brief  改变PWM占空比     
 * @note   None
 * @param  instance:
 *         @arg HW_TPM0 : TPM0模块
 *         @arg HW_TPM1 : TPM1模块
 * @param  chl : PWM通道
 * @param  duty : 占空比0~10000 对应 0-100%
 * @retval None
 */
void TPM_PWM_SetDuty(uint32_t instance, uint8_t chl, uint32_t duty)
{
    if(duty < 10000)
    {
        TPMBases[instance]->CONTROLS[chl].CnV = ((TPMBases[instance]->MOD) * duty) / 10000;
    }
    else
    {
        TPMBases[instance]->CONTROLS[chl].CnV = (TPMBases[instance]->MOD)+1;
    }
}

/**
 * @brief  设置TPM中断模式  
 * @note   None
 * @param  instance:
 *         @arg HW_TPM0 : TPM0模块
 *         @arg HW_TPM1 : TPM1模块
 * @param  mode : 中断模式，详见tpm.h文件
 * @param  duty : 占空比0~10000
 * @param  val : 1：开启；0关闭
 * @retval None
 */
void TPM_SetIntMode(uint32_t instance, TPM_Int_t mode, bool val)
{
    REG_SET(TPMClkGate, instance);
    NVIC_EnableIRQ(TPM_IRQTbl[instance]);
    
    TPM_Type *TPMx = (TPM_Type*)TPMBases[instance];
    
    LIB_TRACE("TPM int mode:%d\r\n", mode);
    switch(mode)
    {
        case kTPM_IntCh0:
        case kTPM_IntCh1:
        case kTPM_IntCh2:
        case kTPM_IntCh3:
        case kTPM_IntCh4:
        case kTPM_IntCh5:
            (val)?(TPMx->CONTROLS[mode].CnSC |= TPM_CnSC_CHIE_MASK):(TPMx->CONTROLS[mode].CnSC &= ~TPM_CnSC_CHIE_MASK);
            break;
        case kTPM_IntCounterOverFlow:
            (val)?(TPMx->SC |= TPM_SC_TOIE_MASK):(TPMx->SC &= ~TPM_SC_TOIE_MASK);
            break;
        default:
            break;
    }
}

/**
 * @brief  获得TPM通道计数值
 * @param  instance:
 *         @arg HW_TPM0 : TPM0模块
 *         @arg HW_TPM1 : TPM1模块
 * @param  chl : 通道0~5
 * @retval 计数值
 */
uint32_t TPM_GetChlCounter(uint32_t instance, uint32_t chl)
{
    return TPMBases[instance]->CONTROLS[chl].CnV;
}

/**
 * @brief  设置TPM通道计数值
 * @param  instance:
 *         @arg HW_TPM0 : TPM0模块
 *         @arg HW_TPM1 : TPM1模块
 * @param  chl : 通道0~5
 * @param  val : 计数器数值
 * @retval None
 */
void TPM_SetChlCounter(uint32_t instance, uint32_t chl, uint32_t val)
{
    TPMBases[instance]->CONTROLS[chl].CnV = val;
}

/**
 * @brief  设置TPM主通道计数值
 * @param  instance:
 *         @arg HW_TPM0 : TPM0模块
 *         @arg HW_TPM1 : TPM1模块
 * @param  val : 计数值
 * @retval None
 */
void TPM_SetMoudlo(uint32_t instance, uint32_t val)
{
    TPMBases[instance]->CNT = val;
}

/**
 * @brief  获得TPM主通道计数值
 * @param  instance:
 *         @arg HW_TPM0 : TPM0模块
 *         @arg HW_TPM1 : TPM1模块
 * @retval  计数器数值
 */
uint32_t TPM_GetMoudlo(uint32_t instance)
{
    return TPMBases[instance]->MOD;
}

void TPM_IRQHandler(uint32_t instance)
{
    uint32_t i;
    TPMBases[instance]->SC |= TPM_SC_TOF_MASK;
    for(i=0; i<TPM_ChlNum[instance]; i++)
    {
        TPMBases[instance]->CONTROLS[i].CnSC |= TPM_CnSC_CHF_MASK;
    }
}

/*

static const QuickInit_Type TPM_QuickInitTable[] =
{
    { 0, 4, 3, 24, 1, 0}, //TPM0_CH0_PE24
    { 0, 0, 3,  3, 1, 0}, //TPM0_CH0_PA03
    { 0, 2, 4,  1, 1, 0}, //TPM0_CH0_PC01
    { 0, 3, 4,  0, 1, 0}, //TPM0_CH0_PD00
    { 0, 4, 3, 25, 1, 1}, //TPM0_CH1_PE25
    { 0, 0, 3,  4, 1, 1}, //TPM0_CH1_PA04
    { 0, 2, 4,  2, 1, 1}, //TPM0_CH1_PC02
    { 0, 3, 4,  1, 1, 1}, //TPM0_CH1_PD01
    { 0, 3, 4,  2, 1, 2}, //TPM0_CH2_PD02
    { 0, 4, 3, 29, 1, 2}, //TPM0_CH2_PE29
    { 0, 0, 3,  5, 1, 2}, //TPM0_CH2_PA05
    { 0, 2, 4,  3, 1, 2}, //TPM0_CH2_PC03
    { 0, 3, 4,  3, 1, 3}, //TPM0_CH3_PD03
    { 0, 4, 3, 30, 1, 3}, //TPM0_CH3_PE30
    { 0, 0, 3,  6, 1, 3}, //TPM0_CH3_PA06
    { 0, 2, 4,  4, 1, 3}, //TPM0_CH3_PC04
    { 0, 2, 3,  8, 1, 4}, //TPM0_CH4_PC08
    { 0, 3, 4,  4, 1, 4}, //TPM0_CH4_PD04
    { 0, 4, 3, 31, 1, 4}, //TPM0_CH4_PE31
    { 0, 0, 3,  7, 1, 4}, //TPM0_CH4_PA07
    { 0, 4, 3, 26, 1, 5}, //TPM0_CH5_PE26
    { 0, 0, 3,  0, 1, 5}, //TPM0_CH5_PA00
    { 0, 2, 3,  9, 1, 5}, //TPM0_CH5_PC09
    { 0, 3, 4,  5, 1, 5}, //TPM0_CH5_PD05
    { 1, 0, 3, 12, 1, 0}, //TPM1_CH0_PA12
    { 1, 1, 3,  0, 1, 0}, //TPM1_CH0_PB00
    { 1, 4, 3, 20, 1, 0}, //TPM1_CH0_PE20
    { 1, 4, 3, 21, 1, 1}, //TPM1_CH1_PE21
    { 1, 0, 3, 13, 1, 1}, //TPM1_CH1_PA13
    { 1, 1, 3,  1, 1, 1}, //TPM1_CH1_PB01
    { 2, 0, 3,  1, 1, 0}, //TPM2_CH0_PA01
    { 2, 1, 3,  2, 1, 0}, //TPM2_CH0_PB02
    { 2, 1, 3, 18, 1, 0}, //TPM2_CH0_PB18
    { 2, 4, 3, 23, 1, 1}, //TPM2_CH1_PE23
    { 2, 0, 3,  2, 1, 1}, //TPM2_CH1_PA02
    { 2, 1, 3,  3, 1, 1}, //TPM2_CH1_PB03
    { 2, 1, 3, 19, 1, 1}, //TPM2_CH1_PB19
};
*/


#endif
