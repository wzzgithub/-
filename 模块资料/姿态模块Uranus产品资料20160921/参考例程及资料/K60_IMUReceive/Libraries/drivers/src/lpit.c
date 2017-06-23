/**
  ******************************************************************************
  * @file    lpit.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2015.11.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "lpit.h"
#include "common.h"

#if defined(LPIT0)

static uint32_t fac_us;


#if defined(PCC0_PCC_LPIT0_CGC_MASK)
static const Reg_t LPITClkGate[] =
{
    {(void*)&(PCC0->PCC_LPIT0), PCC0_PCC_LPIT0_CGC_MASK, PCC0_PCC_LPIT0_CGC_SHIFT},
#if defined(PCC0_PCC_LPIT1_CGC_MASK)
    {(void*)&(PCC0->PCC_LPIT1), PCC0_PCC_LPIT1_CGC_MASK, PCC0_PCC_LPIT1_CGC_SHIFT},
#endif
};
#else
#error "no LPIT clock gate defined!"
#endif

static const IRQn_Type LPIT_IRQTbl[] =
{
    (IRQn_Type)(LPIT0_IRQn + 0),
};

uint32_t LPIT_GetClock(void)
{
    uint32_t clk;
    
    REG_CLR(LPITClkGate, 0);
    *((uint32_t*)LPITClkGate[0].addr) = PCC1_PCC_LPUART0_PCS(3);
    REG_SET(LPITClkGate, 0);
    clk = (48*1000*1000);
    return clk;
}

void LPIT_Init(uint32_t chl, uint32_t us)
{
    REG_SET(LPITClkGate, 0);
    
    LPIT0->MCR = LPIT_MCR_M_CEN_MASK  | LPIT_MCR_DOZE_EN_MASK;
    fac_us = US_TO_COUNT(1, LPIT_GetClock());
    LPIT0->CHANNEL[chl].TVAL = fac_us * us;
    LPIT0->CHANNEL[chl].TCTRL = LPIT_TCTRL_T_EN_MASK | LPIT_TCTRL_MODE(0);
}

void LPIT_AdvConfig(uint32_t chl, LPIT_AdvConfig_t *config)
{
    uint32_t temp;
    LPIT0->CHANNEL[chl].TCTRL &= ~LPIT_TCTRL_T_EN_MASK;
    
    temp = LPIT0->CHANNEL[chl].TCTRL;
    temp &= ~(LPIT_TCTRL_TSOT_MASK | LPIT_TCTRL_TSOI_MASK | LPIT_TCTRL_TROT_MASK | LPIT_TCTRL_TRG_SEL_MASK);
    
    temp |= LPIT_TCTRL_TRG_SEL(config->triggerSource);
    temp |= (config->startOnTrigger == true)?(LPIT_TCTRL_TSOT_MASK):(0);
    temp |= (config->stopOnTimeout == true)?(LPIT_TCTRL_TSOI_MASK):(0);
    temp |= (config->reloadOnTrigger == true)?(LPIT_TCTRL_TROT_MASK):(0);
    
    
    LPIT0->CHANNEL[chl].TCTRL |= LPIT_TCTRL_T_EN_MASK | temp;
}


void LPIT_SetIntMode(uint32_t chl, bool val)
{
    if(val)
    {
        NVIC_EnableIRQ(LPIT_IRQTbl[0]);
        LPIT0->MIER |= (1<<chl);
    }
    else
    {
        LPIT0->MIER &= ~(1<<chl);
    }
}

void LPIT_SetValue(uint32_t chl, uint32_t val)
{
    LPIT0->CHANNEL[chl].TCTRL &= ~LPIT_TCTRL_T_EN_MASK;
    LPIT0->CHANNEL[chl].TVAL = val;
    LPIT0->CHANNEL[chl].TCTRL |= LPIT_TCTRL_T_EN_MASK;
}

void LPIT_SetTime(uint32_t chl, uint32_t us)
{
    LPIT_SetValue(chl, fac_us*us);
}

uint32_t LPIT_GetValue(uint32_t chl)
{
    return LPIT0->CHANNEL[chl].CVAL;
}

uint32_t LPIT_GetTime(uint32_t chl)
{
    return LPIT_GetValue(chl)/fac_us;
}



#endif


