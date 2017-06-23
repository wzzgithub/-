/**
  ******************************************************************************
  * @file    dma.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.06.04
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "dma.h"
#include "common.h"

/************************************* EDMA moudle *******************************/
#if defined(DMA_CR_EDBG_MASK) /* EDMA */

#ifndef DMAMUX_BASES
#define DMAMUX_BASES {DMAMUX};
#endif

static DMAMUX_Type * const DMAMUXBases[] = DMAMUX_BASES;

static const IRQn_Type DMA_IRQTbl[] = 
{
    (IRQn_Type)(0 + 0),
    (IRQn_Type)(0 + 1),
    (IRQn_Type)(0 + 2),
    (IRQn_Type)(0 + 3),
    (IRQn_Type)(0 + 4),
    (IRQn_Type)(0 + 5),
    (IRQn_Type)(0 + 6),
    (IRQn_Type)(0 + 7),
    (IRQn_Type)(0 + 8),
    (IRQn_Type)(0 + 9),
    (IRQn_Type)(0 + 10),
    (IRQn_Type)(0 + 11),
    (IRQn_Type)(0 + 12),
    (IRQn_Type)(0 + 13),
    (IRQn_Type)(0 + 14),
    (IRQn_Type)(0 + 15),
};

 /**
 * @brief  初始化配置DMA模块
 * @note   None
 * @param  Init: DMA配置参数结构体，详见dma.h文件
 * @retval None
 */
void DMA_Init(DMA_Init_t *Init)
{
    uint8_t chl;
    
	/* enable DMA and DMAMUX clock */
#if defined(SIM_SCGC6_DMAMUX0_MASK)
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX0_MASK;
#endif
#if defined(PCC0_PCC_DMAMUX0_CGC_MASK)
    PCC0->PCC_DMAMUX0 |= PCC0_PCC_DMAMUX0_CGC_MASK;
#endif

#if  defined(DMAMUX)
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
#endif /* DMAMUX */
	
#if defined(SIM_SCGC7_DMA_MASK)
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
#endif
#if defined(PCC0_PCC_DMA0_CGC_MASK)
    PCC0->PCC_DMA0 |= PCC0_PCC_DMA0_CGC_MASK;
#endif
    
    chl = Init->chl;

    /* disable chl first */
    DMA0->CERQ |= DMA_CERQ_CERQ(chl);
    
    /* dma chl source config */
    DMAMUXBases[0]->CHCFG[chl] = DMAMUX_CHCFG_SOURCE(Init->trigSrc & 0xFF);
    
    /* trigger mode: normal */
    DMAMUXBases[0]->CHCFG[chl] &= ~DMAMUX_CHCFG_TRIG_MASK;

    DMA0->TCD[chl].ATTR  = 0;
    /* auto close enable(disable req on major loop complete)*/
    DMA0->TCD[chl].CSR   = DMA_CSR_DREQ_MASK;
    
    /* minor loop cnt */
    DMA0->TCD[chl].NBYTES_MLNO = DMA_NBYTES_MLNO_NBYTES(Init->sDataWidth);
    /* major loop cnt */
	DMA0->TCD[chl].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(Init->totalByte/Init->sDataWidth);
	DMA0->TCD[chl].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(Init->totalByte/Init->sDataWidth);
    
    /* source config */
    DMA0->TCD[chl].SADDR = Init->sAddr;
    (Init->sAddrIsInc)?(DMA0->TCD[chl].SOFF = Init->sDataWidth):(DMA0->TCD[chl].SOFF = 0);
    DMA0->TCD[chl].ATTR |= DMA_ATTR_SSIZE(Init->sDataWidth/2);
    (Init->sAddrIsInc)?(DMA0->TCD[chl].SLAST = -DMA0->TCD[chl].CITER_ELINKNO):(DMA0->TCD[chl].SLAST = 0);
    
    /* destation config */
    DMA0->TCD[chl].DADDR = Init->dAddr;
    (Init->dAddrIsInc)?(DMA0->TCD[chl].DOFF = Init->dDataWidth):(DMA0->TCD[chl].DOFF = 0);
    DMA0->TCD[chl].ATTR |= DMA_ATTR_DSIZE(Init->dDataWidth/2);
    (Init->dAddrIsInc)?(DMA0->TCD[chl].DLAST_SGA = -DMA0->TCD[chl].CITER_ELINKNO):(DMA0->TCD[chl].DLAST_SGA = 0);

	/* enable DMAMUX */
	DMAMUXBases[0]->CHCFG[chl] |= DMAMUX_CHCFG_ENBL_MASK;
    
}

 /**
 * @brief  启动DMA
 * @note   None
 * @param  chl: DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval None
 */
void DMA_Start(uint8_t chl)
{
    DMA0->CDNE = DMA_CDNE_CDNE(chl);
    DMA0->SERQ = DMA_SERQ_SERQ(chl);
}

 /**
 * @brief  检查DMA是否传输完成
 * @note   None
 * @param  chl: DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval CH_OK：传输完成；CH_ERR：传输错误
 */
uint32_t DMA_IsTransDone(uint8_t chl)
{
    if(DMA0->TCD[chl].CSR & DMA_CSR_DONE_MASK)
    {
        return CH_OK;
    }
    else
    {
        return CH_ERR;
    }
}

 /**
 * @brief  设置DMA传输计数器
 * @note   None
 * @param  chl: DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @param  val: DMA传输字节个数
 * @retval None
 */
void DMA_SetTransCnt(uint8_t chl, uint32_t val)
{
	DMA0->TCD[chl].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(val);
	DMA0->TCD[chl].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(val);
}

 /**
 * @brief  获得DMA传输计数器
 * @note   None
 * @param  chl: DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval DMA传输字节个数
 */
uint32_t DMA_GetTransCnt(uint8_t chl)
{
    return (DMA0->TCD[chl].CITER_ELINKNO & DMA_CITER_ELINKNO_CITER_MASK) >> DMA_CITER_ELINKNO_CITER_SHIFT;
}

 /**
 * @brief  暂停DMA传输
 * @note   None
 * @param  chl: DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval None
 */
void DMA_Stop(uint8_t chl)
{
    DMA0->SERQ = DMA_CERQ_CERQ(chl);
}

 /**
 * @brief  设置DMA中断模式
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @param  instance:
 *         @arg kDMAInt_All : DMA中断
 * @param  val :
 *         @arg true : 开启中断
 *         @arg false : 关闭中断
 * @retval None
 */
void DMA_SetIntMode(uint8_t chl, DMA_Int_t mode, bool val)
{
    NVIC_EnableIRQ(DMA_IRQTbl[chl]);
    DMA0->TCD[chl].CSR &= ~DMA_CSR_INTHALF_MASK;
    switch(mode)
    {
        case kDMAInt_All:
            (val)?
            (DMA0->TCD[chl].CSR |= DMA_CSR_INTMAJOR_MASK):
            (DMA0->TCD[chl].CSR &= ~DMA_CSR_INTMAJOR_MASK);
            break; 
        default:
            break;
    }
}

 /**
 * @brief  获得DMA目标传输地址
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval 返回目标地址
 */
uint32_t DMA_GetDestAddr(uint8_t chl)
{
    return DMA0->TCD[chl].DADDR;
}

 /**
 * @brief  获得DMA源地址
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval 返回源地址
 */
uint32_t DMA_GetSrcAddr(uint8_t chl)
{
    return DMA0->TCD[chl].SADDR;
}

 /**
 * @brief  设置DMA目标地址
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @param  addr : 目标地址
 * @retval None
 */
void DMA_SetDestAddr(uint8_t chl, uint32_t addr)
{
    DMA0->TCD[chl].DADDR = addr; 
}

 /**
 * @brief  设置DMA源地址
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @param  addr : 源地址
 * @retval None
 */
void DMA_SetSrcAddr(uint8_t chl, uint32_t addr)
{
    DMA0->TCD[chl].SADDR = addr; 
}

 /**
 * @brief  清除DMA中断标志
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval None
 */
void DMA_ClearIntFlag(uint32_t chl)
{ 
    DMA0->TCD[chl].CSR &= ~DMA_CSR_DONE_MASK;
    DMA0->CINT = DMA_CINT_CINT(chl);
}

void DMA_IRQHandler(uint32_t instance)
{
    DMA0->CINT = DMA_CINT_CINT(instance);
}

#endif /* DMA_CR_EDBG_MASK EDMA */


/************************************* DMA moudle *******************************/
#if defined(DMA_SAR_SAR_MASK)

static const IRQn_Type DMA_IRQTbl[] = 
{
    DMA0_IRQn,
    DMA1_IRQn,
    DMA2_IRQn,
    DMA3_IRQn,
};

 /**
 * @brief  初始化配置DMA模块
 * @note   None
 * @param  Init: DMA配置参数结构体，详见dma.h文件
 * @retval None
 */
void DMA_Init(DMA_Init_t *Init)
{
	/* enable DMA and DMAMUX clock */
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;    
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
    
    /* disable chl first */
    DMA0->DMA[Init->chl].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    DMA0->DMA[Init->chl].DCR &= ~DMA_DCR_ERQ_MASK;
    
    /* dma chl source config */
    DMAMUX0->CHCFG[Init->chl] = (DMAMUX_CHCFG_SOURCE(Init->trigSrc) & 0xFF);
    
    /* trigger mode */
    switch(Init->trigSrcMod)
    {
        case kDMA_TrigSrc_Normal:
            DMAMUX0->CHCFG[Init->chl] &= ~DMAMUX_CHCFG_TRIG_MASK;
            break;
        case kDMA_TrigSrc_Periodic:
            DMAMUX0->CHCFG[Init->chl] |= DMAMUX_CHCFG_TRIG_MASK;
            break;
        default:
            break;
    }
    
    /* defaut: cycle steal and enable auto disable req */
    DMA0->DMA[Init->chl].DCR = DMA_DCR_CS_MASK | DMA_DCR_D_REQ_MASK;
    
    /* transfer bytes cnt */
    DMA0->DMA[Init->chl].DSR_BCR = DMA_DSR_BCR_BCR(Init->totalByte);
    
    /* source config */
    DMA0->DMA[Init->chl].SAR  = Init->sAddr;
    DMA0->DMA[Init->chl].DCR |= DMA_DCR_SSIZE(Init->sDataWidth);
    (Init->sAddrIsInc)?(DMA0->DMA[Init->chl].DCR |= DMA_DCR_SINC_MASK):(DMA0->DMA[Init->chl].DCR &= ~DMA_DCR_SINC_MASK);
    
    /* dest config */
    DMA0->DMA[Init->chl].DAR  = Init->dAddr;
    DMA0->DMA[Init->chl].DCR |= DMA_DCR_DSIZE(Init->sDataWidth);
    (Init->dAddrIsInc)?(DMA0->DMA[Init->chl].DCR |= DMA_DCR_DINC_MASK):(DMA0->DMA[Init->chl].DCR &= ~DMA_DCR_DINC_MASK);

    /* enable chl */
    DMAMUX0->CHCFG[Init->chl] |= DMAMUX_CHCFG_ENBL_MASK;
}

 /**
 * @brief  设置DMA源字节数
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @param  sMod: DMA源字节数，详见dma.h文件
 * @retval None
 */
void DMA_SetSrcMod(uint8_t chl, DMA_Modulo_t sMod)
{
    DMA0->DMA[chl].DCR |= DMA_DCR_SMOD(sMod); 
}

 /**
 * @brief  设置DMA目标字节数
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @param  sMod: DMA目标字节数，详见dma.h文件
 * @retval None
 */
void DMA_SetDestMod(uint8_t chl, DMA_Modulo_t dMod)
{
    DMA0->DMA[chl].DCR |= DMA_DCR_DMOD(dMod); 
}

 /**
 * @brief  设置DMA源地址
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @param  addr : 源地址
 * @retval None
 */
void DMA_SetSrcAddr(uint8_t chl, uint32_t addr)
{
    DMA0->DMA[chl].SAR  = addr;
}

 /**
 * @brief  设置DMA目标地址
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @param  addr : 目标地址
 * @retval None
 */
void DMA_SetDestAddr(uint8_t chl, uint32_t addr)
{
    DMA0->DMA[chl].DAR  = addr;
}

 /**
 * @brief  获得DMA传输计数器
 * @note   None
 * @param  chl: DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval DMA传输字节个数
 */
uint32_t DMA_GetTransCnt(uint8_t chl)
{
    volatile uint32_t cnt;
    cnt = (DMA0->DMA[chl].DSR_BCR & 0x0000FFFF )>>DMA_DSR_BCR_BCR_SHIFT;
    if(!cnt)
    {
        DMA0->DMA[chl].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    }
    return cnt;
}

 /**
 * @brief  检查DMA是否传输完成
 * @note   None
 * @param  chl: DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval CH_OK：传输完成；CH_ERR：传输错误
 */
uint32_t DMA_IsTransDone(uint8_t chl)
{
    if(DMA0->DMA[chl].DSR_BCR & DMA_DSR_BCR_DONE_MASK)
    {
        return CH_OK;
    }
    else
    {
        return CH_ERR;
    }
}

 /**
 * @brief  设置DMA传输计数器
 * @note   None
 * @param  chl: DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @param  val: DMA传输字节个数
 * @retval None
 */
void DMA_SetTransCnt(uint8_t chl, uint32_t val)
{
    DMA0->DMA[chl].DSR_BCR = DMA_DSR_BCR_BCR(val);
}

 /**
 * @brief  设置DMA中断模式
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @param  instance:
 *         @arg kDMAInt_All : DMA中断
 * @param  val :
 *         @arg true : 开启中断
 *         @arg false : 关闭中断
 * @retval None
 */
void DMA_SetIntMode(uint8_t chl, DMA_Int_t mode, bool val)
{
    NVIC_EnableIRQ(DMA_IRQTbl[chl]);
    
    switch(mode)
    {
        case kDMAInt_All:
            (val)?
            (DMA0->DMA[chl].DCR |= DMA_DCR_EINT_MASK):
            (DMA0->DMA[chl].DCR &= ~DMA_DCR_EINT_MASK);
            break; 
        default:
            break;
    }
}

 /**
 * @brief  清除DMA中断标志
 * @note   None
 * @param  chl : DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval None
 */
void DMA_ClearIntFlag(uint32_t chl)
{
    DMA0->DMA[chl].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
}

 /**
 * @brief  开启DMA传输
 * @note   None
 * @param  chl: DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval None
 */
void DMA_Start(uint8_t chl)
{
    DMA0->DMA[chl].DCR |= DMA_DCR_ERQ_MASK;
}

 /**
 * @brief  暂停DMA传输
 * @note   None
 * @param  chl: DMA通道HW_DMA_CH0-HW_DMA_CH3
 * @retval None
 */
void DMA_Stop(uint8_t chl)
{
    DMA0->DMA[chl].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    DMA0->DMA[chl].DCR &= ~DMA_DCR_ERQ_MASK;
}


//void DMA_CancelTransfer(uint8_t chl)
//{
//    DMA0->DMA[chl].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
//}

//void DMA0_IRQHandler(void)
//{
//    DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
//    if(DMA_CallBackTable[0]) DMA_CallBackTable[0]();
//}



#endif /* DMA_SAR_SAR_MASK DMA */
