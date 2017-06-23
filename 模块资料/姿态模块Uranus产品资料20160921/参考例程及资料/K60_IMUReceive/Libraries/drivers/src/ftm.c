/**
  ******************************************************************************
  * @file    ftm.c
  * @author  YANDLD
  * @version V3.0
  * @date    2016.5.09
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "ftm.h"
#include "common.h"

#if (defined(FTM0))

static const uint32_t FTM_ChlNum[] = {8,2,2};
FTM_Type * const FTMBases[] = FTM_BASES;

static const Reg_t FTMClkGate[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FTM0_MASK},
#if defined(SIM_SCGC6_FTM1_MASK)
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FTM1_MASK},
#endif
#if defined(SIM_SCGC6_FTM2_MASK)
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FTM2_MASK},
#endif
#if defined(SIM_SCGC2_FTM3_MASK)
    {(void*)&(SIM->SCGC2), SIM_SCGC2_FTM3_MASK},
#endif
};

static const IRQn_Type FTM_IRQTbl[] = 
{
    (IRQn_Type)(FTM0_IRQn + 0),
    (IRQn_Type)(FTM0_IRQn + 1),
    (IRQn_Type)(FTM0_IRQn + 2),
};



static uint32_t FTMGetClock(uint32_t instance)
{
    return GetClock(kBusClock);
}

void FTM_SetMoudleParam(uint32_t instance, uint32_t modulo, uint32_t ps)
{
    /* disable FTM, we must set CLKS(0) before config FTM! */
    FTMBases[instance]->SC &= ~(FTM_SC_PS_MASK | FTM_SC_CLKS_MASK);
    
    FTMBases[instance]->CNT = 0;
    FTMBases[instance]->MOD = modulo;
    
    FTMBases[instance]->SC &= ~FTM_SC_PS_MASK;
    FTMBases[instance]->SC |= FTM_SC_PS(ps) | FTM_SC_CLKS(1);
    
    LIB_TRACE("FTM ps:%d\r\n", (1<<ps));
    LIB_TRACE("FTM modulo:%d\r\n", modulo);
}

uint32_t FTM_PWM_Init(uint32_t MAP, FTM_t mode, uint32_t Hz)
{
    map_t * pq = (map_t*)&(MAP);
    uint32_t instance = pq->ip;
    FTM_Type *FTMx = FTMBases[instance];
    uint32_t clk = FTMGetClock(pq->ip);
    int32_t i, modulo;
    uint8_t ps = 0;
    
    REG_SET(FTMClkGate, instance);
    FTMx->SC = 0;
    FTMx->CNT = 0;
    
    if(Hz)
    {
        switch(mode)
        {
            case kFTM_Disabled:
                FTMx->CONTROLS[pq->chl].CnSC &= ~(FTM_CnSC_MSA_MASK | FTM_CnSC_MSB_MASK);
                break;
            case kFTM_InputCapture:
                
                break;
            case kFTM_PWM_EdgeAligned:
                FTMx->CONTROLS[pq->chl].CnSC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
                FTMx->CONTROLS[pq->chl].CnSC &= ~FTM_CnSC_MSA_MASK;
                FTMx->SC &= ~FTM_SC_CPWMS_MASK;
                break;
            case kFTM_PWM_CenterAligned:
                FTMx->CONTROLS[pq->chl].CnSC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
                FTMx->CONTROLS[pq->chl].CnSC &= ~FTM_CnSC_MSA_MASK;
                FTMx->SC |= FTM_SC_CPWMS_MASK;
                Hz *= 2;
                break;
        }
    }
    else
    {
        FTMx->CONTROLS[pq->chl].CnSC &= ~(FTM_CnSC_MSA_MASK | FTM_CnSC_MSB_MASK | FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSB_MASK);
    }
    
    /* calucate modulo and ps */
    for(i=0; i<7; i++)
    {
        if((clk/Hz)/(1<<i) < FTM_MOD_MOD_MASK)
        {
            ps = i;
            modulo = (clk/Hz)/(1<<i) - 1;
            LIB_TRACE("ftm ps:%d modulo:%d\r\n", ps, modulo);
            break;
        }
    }
    
    if(i == 7)
    {
        LIB_TRACE("error! cannot reach %dHz!\r\n", Hz);
        ps = 7;
        modulo = FTM_MOD_MOD_MASK - 1;
    }
    
    FTMx->CONTROLS[pq->chl].CnV = 0;
    FTM_SetMoudleParam(instance, modulo, ps);
    
    PIN_SET_MUX;
    return pq->ip;
}

uint32_t FTM_QD_Init(uint32_t MAP, FTM_QD_t mode)
{
    map_t * pq = (map_t*)&(MAP);
    FTM_Type *FTMx = FTMBases[pq->ip];
    
    REG_SET(FTMClkGate, pq->ip);
    FTMx->SC = 0x00;
    FTMx->CNT = 0x00;
    FTMx->CNTIN = 0x00;
    
    FTM_SetMoudleParam(pq->ip, 0xFFFF, 0);
    
    FTMx->MODE |= FTM_MODE_WPDIS_MASK;
    FTMx->MODE |= FTM_MODE_FTMEN_MASK;
    
    FTMx->QDCTRL = FTM_QDCTRL_QUADEN_MASK;
    
    (mode == kFTM_QD_AB)?(FTMx->QDCTRL &= ~FTM_QDCTRL_QUADMODE_MASK):(FTMx->QDCTRL |= FTM_QDCTRL_QUADMODE_MASK);

    PIN_SET_MUX;
    return pq->ip;
}

void FTM_QD_Get(uint32_t instance, int16_t* val, uint8_t* dir)
{
    FTM_Type *FTMx = (FTM_Type*)FTMBases[instance];
    *dir = ((FTMx->QDCTRL >> FTM_QDCTRL_QUADIR_SHIFT) & 1);
	*val = (FTMx->CNT & 0xFFFF);
}

void FTM_PWM_Invert(uint32_t instance, uint8_t chl)
{
    #if defined(FTM_POL_POL0_MASK)
    FTM_Type *FTMx = (FTM_Type*)FTMBases[instance];
    (FTMx->POL & (1<<chl))?(FTMx->POL &= ~(1<<chl)):(FTMx->POL |= (1<<chl));
    #endif
}


/**
 * @brief  改变PWM占空比
 * @endcode         
 * @param  instance       : 模块号
 * @param  chl            : 通道
 * @param  pwmDuty        : 占空比
 * @retval None
 */
void FTM_PWM_SetDuty(uint32_t instance, uint8_t chl, uint32_t duty)
{
    if(duty < 10000)
    {
        FTMBases[instance]->CONTROLS[chl].CnV = ((FTMBases[instance]->MOD) * duty) / 10000;
    }
    else
    {
        FTMBases[instance]->CONTROLS[chl].CnV = (FTMBases[instance]->MOD)+1;
    }
}

void FTM_SetIntMode(uint32_t instance, FTM_Int_t mode, bool val)
{
    REG_SET(FTMClkGate, instance);
    NVIC_EnableIRQ(FTM_IRQTbl[instance]);
    
    FTM_Type *FTMx = (FTM_Type*)FTMBases[instance];
    
    LIB_TRACE("FTM int mode:%d\r\n", mode);
    switch(mode)
    {
        case kFTM_IntCh0:
        case kFTM_IntCh1:
        case kFTM_IntCh2:
        case kFTM_IntCh3:
        case kFTM_IntCh4:
        case kFTM_IntCh5:
            (val)?(FTMx->CONTROLS[mode].CnSC |= FTM_CnSC_CHIE_MASK):(FTMx->CONTROLS[mode].CnSC &= ~FTM_CnSC_CHIE_MASK);
            break;
        case kFTM_IntCounterOverFlow:
            (val)?(FTMx->SC |= FTM_SC_TOIE_MASK):(FTMx->SC &= ~FTM_SC_TOIE_MASK);
            break;
        default:
            break;
    }
}

/**
 * @brief  获得FTM 通道计数值
 * @param  instance     : 模块号
 * @param  chl          : 通道号
 * @retval 计数值
 */
uint32_t FTM_GetChlCounter(uint32_t instance, uint32_t chl)
{
    return FTMBases[instance]->CONTROLS[chl].CnV;
}

void FTM_SetChlCounter(uint32_t instance, uint32_t chl, uint32_t val)
{
    FTMBases[instance]->CONTROLS[chl].CnV = val;
}

/**
 * @brief  设置FTM主通道计数值
 * @param  instance     : 模块号
 * @param  val          : value
 * @retval None
 */
void FTM_SetMoudlo(uint32_t instance, uint32_t val)
{
    FTMBases[instance]->CNT = val;
}

/**
 * @brief  Get FTM modulo
 * @param  instance
 * @retval modulo value
 */
uint32_t FTM_GetMoudlo(uint32_t instance)
{
    return FTMBases[instance]->MOD;
}

void FTM_IRQHandler(uint32_t instance)
{
    uint32_t i;
    FTMBases[instance]->SC &= ~FTM_SC_TOF_MASK;
    for(i=0; i<FTM_ChlNum[instance]; i++)
    {
        FTMBases[instance]->CONTROLS[i].CnSC &= ~FTM_CnSC_CHF_MASK;
    }
}

#endif




