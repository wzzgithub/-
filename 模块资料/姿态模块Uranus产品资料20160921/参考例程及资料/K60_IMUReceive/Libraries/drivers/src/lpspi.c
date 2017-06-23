/**
  ******************************************************************************
  * @file    lpspi.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.03.28
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
 #include "lpspi.h"
 #include "common.h"
 
#if defined(LPSPI0)
 
 
LPSPI_Type * const LPSPIBases[] = LPSPI_BASES;

 
static const Reg_t LPSPIClkGate[] =
{
    /* LPSPI0 */
#if defined(PCC1_PCC_LPSPI0_CGC_MASK)
    {(void*)&(PCC1->PCC_LPSPI0), PCC1_PCC_LPSPI0_CGC_MASK, PCC1_PCC_LPSPI0_CGC_SHIFT},
#else 
    #error "no LPSPI0 clock gate defined!"
#endif
    
    /* LPSPI1 */
#if defined(PCC1_PCC_LPSPI1_CGC_MASK)
    {(void*)&(PCC1->PCC_LPSPI1), PCC1_PCC_LPSPI1_CGC_MASK, PCC1_PCC_LPSPI1_CGC_SHIFT},
#else 
    #error "no LPSPI1 clock gate defined!"
#endif

    /* LPSPI2 */
#if defined(PCC0_PCC_LPSPI2_CGC_MASK)
    {(void*)&(PCC0->PCC_LPSPI2), PCC0_PCC_LPSPI2_CGC_MASK, PCC0_PCC_LPSPI2_CGC_SHIFT},
#else 
    #error "no LPSPI2 clock gate defined!"
#endif

};
 
static const IRQn_Type LPSPI_IRQTbl[] = 
{
    (IRQn_Type)(LPSPI0_IRQn),
    (IRQn_Type)(LPSPI1_IRQn),
    (IRQn_Type)(LPSPI2_IRQn),
};
 
static const uint8_t s_baudratePrescaler[] = {1, 2, 4, 8, 16, 32, 64, 128};
uint32_t LPSPI_MasterSetBaudRate(LPSPI_Type *base, uint32_t baudRate_Bps,  uint32_t srcClock_Hz, uint32_t *tcrPrescaleValue)            
{

    uint32_t prescaler, bestPrescaler;
    uint32_t scaler, bestScaler;
    uint32_t realBaudrate, bestBaudrate;
    uint32_t diff, min_diff;
    uint32_t desiredBaudrate = baudRate_Bps;

    /* find combination of prescaler and scaler resulting in baudrate closest to the
    * requested value
    */
    min_diff = 0xFFFFFFFFU;

    /* Set to maximum divisor value bit settings so that if baud rate passed in is less
    * than the minimum possible baud rate, then the SPI will be configured to the lowest
    * possible baud rate
    */
    bestPrescaler = 7;
    bestScaler = 255;

    bestBaudrate = 0; /* required to avoid compilation warning */

    /* In all for loops, if min_diff = 0, the exit for loop*/
    for (prescaler = 0; (prescaler < 8) && min_diff; prescaler++)
    {
        for (scaler = 0; (scaler < 256) && min_diff; scaler++)
        {
            realBaudrate = (srcClock_Hz / (s_baudratePrescaler[prescaler] * (scaler + 2U)));
            /* calculate the baud rate difference based on the conditional statement
            * that states that the calculated baud rate must not exceed the desired baud rate
            */
            if (desiredBaudrate >= realBaudrate)
            {
                diff = desiredBaudrate - realBaudrate;
                if (min_diff > diff)
                {
                    /* a better match found */
                    min_diff = diff;
                    bestPrescaler = prescaler;
                    bestScaler = scaler;
                    bestBaudrate = realBaudrate;
                }
            }
        }
    }

    /* Write the best baud rate scalar to the CCR.
    * Note, no need to check for error since we've already checked to make sure the module is
    * disabled and in master mode. Also, there is a limit on the maximum divider so we will not
    * exceed this.
    */
    base->CCR = (base->CCR & ~LPSPI_CCR_SCKDIV_MASK) | LPSPI_CCR_SCKDIV(bestScaler);

    /* return the best prescaler value for user to use later */
    *tcrPrescaleValue = bestPrescaler;

    /* return the actual calculated baud rate */
    return bestBaudrate;
}


uint32_t LPSPI_Init(uint32_t MAP, uint32_t baudrate)
{
    uint32_t acutalBaud, tcrPrescaleValue;
    map_t * pq = (map_t*)&(MAP);
    LPSPI_Type *LPSPIx = (LPSPI_Type*)LPSPIBases[pq->ip];
    
    REG_CLR(LPSPIClkGate, pq->ip);
    *((uint32_t*)LPSPIClkGate[pq->ip].addr) = PCC1_PCC_LPSPI0_PCS(3);
    REG_SET(LPSPIClkGate, pq->ip);
    
    /* reset module */
    LPSPIx->CR |= LPSPI_CR_RST_MASK;
    LPSPIx->CR |= LPSPI_CR_RRF_MASK | LPSPI_CR_RTF_MASK;
    LPSPIx->CR = 0x00U;
    LPSPIx->SR = 0xFFFFFFFF;
    
    /* master mode, pcs active low */
    LPSPIx->CFGR1 = LPSPI_CFGR1_MASTER_MASK | LPSPI_CFGR1_NOSTALL_MASK;
    
    acutalBaud = LPSPI_MasterSetBaudRate(LPSPIx, baudrate, GetClock(kBusClock), &tcrPrescaleValue);
    LIB_TRACE("Input clock:%dHz acutual:%dHz\r\n", GetClock(kBusClock), acutalBaud);
    
    LPSPIx->CCR |= LPSPI_CCR_SCKPCS(2) | LPSPI_CCR_PCSSCK(2) | LPSPI_CCR_DBT(2);
    
    /* water mark */
    LPSPIx->FCR = LPSPI_FCR_RXWATER(0) | LPSPI_FCR_RXWATER(0);
    
    /* Set Transmit Command Register*/
    LPSPIx->TCR = LPSPI_TCR_CPOL(0) | LPSPI_TCR_CPHA(0) | LPSPI_TCR_LSBF(0) | LPSPI_TCR_FRAMESZ(8-1) | LPSPI_TCR_PRESCALE(tcrPrescaleValue);

    LPSPIx->CR |= LPSPI_CR_MEN_MASK;
    
    PIN_SET_MUX;
    return pq->ip;
}

void LPSPI_SelectCS(uint32_t instance, uint32_t cs)
{
    LPSPI_Type *LPSPIx = (LPSPI_Type*)LPSPIBases[instance];
  //  LPSPIx->CR &= ~LPSPI_CR_MEN_MASK;
    
    LPSPIx->TCR &= ~LPSPI_TCR_PCS_MASK;
    LPSPIx->TCR |= LPSPI_TCR_PCS(cs);
    
  //  LPSPIx->CR |= LPSPI_CR_MEN_MASK;
}

void LPSPI_SetCS(uint32_t instance, uint32_t state)
{
    LPSPI_Type *LPSPIx = (LPSPI_Type*)LPSPIBases[instance];
    uint32_t tmp;
    
    tmp = LPSPIx->TCR;
    tmp &= ~LPSPI_TCR_CONT_MASK;
    tmp |= LPSPI_TCR_CONT((state)?(0):(1));
    LPSPIx->TCR = tmp;
}

uint32_t LPSPI_ReadWrite(uint32_t instance, uint32_t data)
{
    LPSPI_Type *LPSPIx = (LPSPI_Type*)LPSPIBases[instance];
    
    while((LPSPIx->SR & LPSPI_SR_TDF_MASK) == 0);
    LPSPIx->TDR = data;
    
    while((LPSPIx->SR & LPSPI_SR_RDF_MASK) == 0);
    return LPSPIx->RDR;    
}



#endif /* LPSPI0 */

