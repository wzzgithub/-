/**
  ******************************************************************************
  * @file    sdhc.c
  * @author  YANDLD
  * @version V3.0
  * @date    2016.2.13
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "sdhc.h"
#include "gpio.h"
#include "common.h"

#if defined(SDHC)

static const Reg_t SDHCClkGate[] =
{
#ifdef SIM_SCGC3_SDHC_MASK
    {(void*)&(SIM->SCGC3), SIM_SCGC3_SDHC_MASK},
#else
    {(void*)&(SIM->SCGC3), SIM_SCGC3_ESDHC_MASK},
#endif
};

typedef struct 
{
    uint32_t type;  
    uint32_t OCR;
    uint32_t CID[4];
    uint32_t CSD[4];
    uint16_t RCA;
    uint32_t CSR[2];
}SDCard_t;     


static SDCard_t SDCard_Handle;

#define ESDHC_XFERTYP_CMDTYP_NORMAL          (0x00)
#define ESDHC_XFERTYP_CMDTYP_SUSPEND         (0x01)
#define ESDHC_XFERTYP_CMDTYP_RESUME          (0x02)
#define ESDHC_XFERTYP_CMDTYP_ABORT           (0x03)

#define ESDHC_XFERTYP_RSPTYP_NO              (0x00)
#define ESDHC_XFERTYP_RSPTYP_136             (0x01)
#define ESDHC_XFERTYP_RSPTYP_48              (0x02)
#define ESDHC_XFERTYP_RSPTYP_48BUSY          (0x03)

#define ESDHC_CMD0   (SDHC_XFERTYP_CMDINX(0)  | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
#define ESDHC_CMD1   (SDHC_XFERTYP_CMDINX(1)  | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
#define ESDHC_CMD2   (SDHC_XFERTYP_CMDINX(2)  | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136))
#define ESDHC_CMD3   (SDHC_XFERTYP_CMDINX(3)  | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD4   (SDHC_XFERTYP_CMDINX(4)  | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
#define ESDHC_CMD5   (SDHC_XFERTYP_CMDINX(5)  | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD6   (SDHC_XFERTYP_CMDINX(6)  | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_ACMD6  (SDHC_XFERTYP_CMDINX(6)  | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD7   (SDHC_XFERTYP_CMDINX(7)  | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD8   (SDHC_XFERTYP_CMDINX(8)  | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD9   (SDHC_XFERTYP_CMDINX(9)  | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136))
#define ESDHC_CMD10  (SDHC_XFERTYP_CMDINX(10) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136))
#define ESDHC_CMD11  (SDHC_XFERTYP_CMDINX(11) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD12  (SDHC_XFERTYP_CMDINX(12) | SDHC_XFERTYP_CMDTYP(ESDHC_XFERTYP_CMDTYP_ABORT) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD13  (SDHC_XFERTYP_CMDINX(13) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_ACMD13 (SDHC_XFERTYP_CMDINX(13) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD15  (SDHC_XFERTYP_CMDINX(15) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
#define ESDHC_CMD16  (SDHC_XFERTYP_CMDINX(16) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD17  (SDHC_XFERTYP_CMDINX(17) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48) | SDHC_XFERTYP_DTDSEL_MASK | SDHC_XFERTYP_DPSEL_MASK)
#define ESDHC_CMD18  (SDHC_XFERTYP_CMDINX(18) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48) | SDHC_XFERTYP_DTDSEL_MASK | SDHC_XFERTYP_DPSEL_MASK | SDHC_XFERTYP_BCEN_MASK | SDHC_XFERTYP_MSBSEL_MASK | SDHC_XFERTYP_AC12EN_MASK)
#define ESDHC_CMD20  (SDHC_XFERTYP_CMDINX(20) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_ACMD22 (SDHC_XFERTYP_CMDINX(22) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48) | SDHC_XFERTYP_DTDSEL_MASK)
#define ESDHC_ACMD23 (SDHC_XFERTYP_CMDINX(23) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD24  (SDHC_XFERTYP_CMDINX(24) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48)|SDHC_XFERTYP_DPSEL_MASK)
#define ESDHC_CMD25  (SDHC_XFERTYP_CMDINX(25) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48) | SDHC_XFERTYP_MSBSEL_MASK | SDHC_XFERTYP_DPSEL_MASK | SDHC_XFERTYP_BCEN_MASK | SDHC_XFERTYP_AC12EN_MASK)
#define ESDHC_CMD26  (SDHC_XFERTYP_CMDINX(26) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD27  (SDHC_XFERTYP_CMDINX(27) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD28  (SDHC_XFERTYP_CMDINX(28) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD29  (SDHC_XFERTYP_CMDINX(29) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD30  (SDHC_XFERTYP_CMDINX(30) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD32  (SDHC_XFERTYP_CMDINX(32) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD33  (SDHC_XFERTYP_CMDINX(33) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD34  (SDHC_XFERTYP_CMDINX(34) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD35  (SDHC_XFERTYP_CMDINX(35) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD36  (SDHC_XFERTYP_CMDINX(36) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD37  (SDHC_XFERTYP_CMDINX(37) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD38  (SDHC_XFERTYP_CMDINX(38) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD39  (SDHC_XFERTYP_CMDINX(39) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD40  (SDHC_XFERTYP_CMDINX(40) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_ACMD41 (SDHC_XFERTYP_CMDINX(41) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD42  (SDHC_XFERTYP_CMDINX(42) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_ACMD42 (SDHC_XFERTYP_CMDINX(42) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_ACMD51 (SDHC_XFERTYP_CMDINX(51) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD52  (SDHC_XFERTYP_CMDINX(52) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD53  (SDHC_XFERTYP_CMDINX(53) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD55  (SDHC_XFERTYP_CMDINX(55) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48))
#define ESDHC_CMD56  (SDHC_XFERTYP_CMDINX(56) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD60  (SDHC_XFERTYP_CMDINX(60) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
#define ESDHC_CMD61  (SDHC_XFERTYP_CMDINX(61) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))


#define ESDHC_PROCTL_DTW_1BIT                (0x00)
#define ESDHC_PROCTL_DTW_4BIT                (0x01)
#define ESDHC_PROCTL_DTW_8BIT                (0x10)

//#define SDHC_HAL_ADMA2_DESC_VALID_MASK           (1 << 0)
//#define SDHC_HAL_ADMA2_DESC_END_MASK             (1 << 1)
//#define SDHC_HAL_ADMA2_DESC_INT_MASK             (1 << 2)
//#define SDHC_HAL_ADMA2_DESC_ACT1_MASK            (1 << 4)
//#define SDHC_HAL_ADMA2_DESC_ACT2_MASK            (1 << 5)
//#define SDHC_HAL_ADMA2_DESC_TYPE_NOP             (SDHC_HAL_ADMA2_DESC_VALID_MASK)
//#define SDHC_HAL_ADMA2_DESC_TYPE_RCV             (SDHC_HAL_ADMA2_DESC_ACT1_MASK | SDHC_HAL_ADMA2_DESC_VALID_MASK)
//#define SDHC_HAL_ADMA2_DESC_TYPE_TRAN            (SDHC_HAL_ADMA2_DESC_ACT2_MASK | SDHC_HAL_ADMA2_DESC_VALID_MASK)
//#define SDHC_HAL_ADMA2_DESC_TYPE_LINK            (SDHC_HAL_ADMA2_DESC_ACT1_MASK | SDHC_HAL_ADMA2_DESC_ACT2_MASK | SDHC_HAL_ADMA2_DESC_VALID_MASK)
//#define SDHC_HAL_ADMA2_DESC_LEN_SHIFT            (16)
//#define SDHC_HAL_ADMA2_DESC_LEN_MASK             (0xFFFFU)
//#define SDHC_HAL_ADMA2_DESC_MAX_LEN_PER_ENTRY    (SDHC_HAL_ADMA2_DESC_LEN_MASK)

/**
 * \brief 等待状态位
 * \param[in] mask 相关标志位
 * \return 相对应的状态
 */
uint32_t SDHC_StatusWait (uint32_t  mask)
{
    volatile uint32_t timeout, val;
    timeout = 0;
    do
    {
        timeout++;
        __NOP();
        val = SDHC->IRQSTAT & mask;
        if(timeout > 50*1000)
        {
            return CH_TIMEOUT;
        }            
    } 
    while (0 == val);
    return CH_OK;
}

//static void SetADMA2Table(uint32_t dir, uint32_t *buffer, uint32_t length)
//{
//    static uint32_t RAdmaTableAddr[sizeof(adma2_t)];
//    static uint32_t WAdmaTableAddr[sizeof(adma2_t)];
//    if(dir == 0)
//    {
//        ((adma2_t *)RAdmaTableAddr)->address = buffer;
//        ((adma2_t *)RAdmaTableAddr)->attribute = ((SDHC_HAL_ADMA2_DESC_LEN_MASK & length) << SDHC_HAL_ADMA2_DESC_LEN_SHIFT) | SDHC_HAL_ADMA2_DESC_TYPE_TRAN | SDHC_HAL_ADMA2_DESC_END_MASK; 
//        SDHC->ADSADDR = (uint32_t)RAdmaTableAddr;
//    }
//    else
//    {
//        ((adma2_t *)WAdmaTableAddr)->address = buffer;
//        ((adma2_t *)WAdmaTableAddr)->attribute = ((SDHC_HAL_ADMA2_DESC_LEN_MASK & length) << SDHC_HAL_ADMA2_DESC_LEN_SHIFT) | SDHC_HAL_ADMA2_DESC_TYPE_TRAN | SDHC_HAL_ADMA2_DESC_END_MASK; 
//        SDHC->ADSADDR = (uint32_t)WAdmaTableAddr; 
//    }
//}

/**
 * \brief SDHC wait function
 * \note internal function
 */
static uint32_t SDHC_WaitCommandLineIdle(void)
{
    volatile uint32_t timeout = 0;
    while (SDHC->PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK))
    {
        __NOP();
        timeout++;
        if(timeout > 50*1000)
        {
            return CH_TIMEOUT;
        }            
    }
    return CH_OK;
}

/**
 * @brief Set SDHC baud rate
 * \param[in] clock 时钟频率
 * \param[in] baudrate 波特率设置
 * \return None
 */                                                            
void SDHC_SetSDClk(uint32_t instance, uint32_t sdClk)
{
	uint32_t pres, div, best_pres, best_div, src_clock;
	uint32_t  best_match, calculated;
    
    best_match = 0xFFFFFFFF;
    calculated = 0;
    
    src_clock = GetClock(kCoreClock);
    /* SDHC clock is always Core/System clock */
    #ifdef SIM_SOPT2_ESDHCSRC_MASK
    SIM->SOPT2 &= ~SIM_SOPT2_ESDHCSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_ESDHCSRC(0);
    #else
    SIM->SOPT2 &= ~SIM_SOPT2_SDHCSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_SDHCSRC(0);
    #endif
    
    SDHC->SYSCTL &= (~ SDHC_SYSCTL_SDCLKEN_MASK);
    for (pres = 2; pres <= 256; pres <<= 1)
    {
        for (div = 1; div <= 16; div++)
        {
            calculated = ABS((int)(sdClk - (src_clock/pres/div)));
            if(calculated < best_match)
            {
                best_match = calculated;
                best_pres = pres;
                best_div = div;
            }
        }
    }
    
    LIB_TRACE("best_match:%d  best_pres:%d best_div:%d\r\n", src_clock/best_pres/best_div, best_pres, best_div);
    
    SDHC->SYSCTL &= ~(SDHC_SYSCTL_DTOCV_MASK | SDHC_SYSCTL_SDCLKFS_MASK | SDHC_SYSCTL_DVS_MASK);
	SDHC->SYSCTL |= (SDHC_SYSCTL_DTOCV(0x0E) | SDHC_SYSCTL_SDCLKFS(best_pres >> 1) | SDHC_SYSCTL_DVS(best_div - 1));
    
	while (0 == (SDHC->PRSSTAT & SDHC_PRSSTAT_SDSTB_MASK));
	SDHC->SYSCTL |= SDHC_SYSCTL_SDCLKEN_MASK;
} 

/**
 * @brief Set SDHC baud rate
 * \param[in] cmd 指令指针
 * \retval   0     CH_OK
 * \retval other error code
 */                                                            
uint32_t SDHC_SendCmd(SDHC_Cmd_t *cmd)
{
    uint32_t xfertyp;
    uint32_t blkattr;
    xfertyp = cmd->cmd;
    
    if(SDHC_WaitCommandLineIdle() != CH_OK)
    {
        return CH_TIMEOUT;
    }
    
    if(cmd->blkCount >= 1)
    {
        xfertyp |= SDHC_XFERTYP_DPSEL_MASK;
    }
    
    /* set block size and block cnt */
    blkattr = SDHC_BLKATTR_BLKSIZE(cmd->blkSize) | SDHC_BLKATTR_BLKCNT(cmd->blkCount);
    
    if (cmd->blkCount > 1)
    {
        xfertyp |= SDHC_XFERTYP_DPSEL_MASK;
        xfertyp |= SDHC_XFERTYP_BCEN_MASK;
        xfertyp |= SDHC_XFERTYP_MSBSEL_MASK;
    }
    
    /* clear status */
    SDHC->IRQSTAT = 0xFFFFFFFF;
        
    /* issue cmd */
    SDHC->CMDARG = cmd->arg;
    SDHC->BLKATTR = blkattr;
    SDHC->XFERTYP = xfertyp;

    /* waitting for respond */
    if(SDHC_StatusWait(SDHC_IRQSTAT_CC_MASK) != CH_OK)
    {
        SDHC->IRQSTAT = 0xFFFFFFFF;
        return CH_IO_ERR;
    }
    
    if(SDHC->IRQSTAT & SDHC_IRQSTAT_CTOE_MASK)
    {
        return CH_IO_ERR;
    }
    
    /* get respond data */
    if ((xfertyp & SDHC_XFERTYP_RSPTYP_MASK) != SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
    {
        cmd->resp[0] = SDHC->CMDRSP[0];
        cmd->resp[1] = SDHC->CMDRSP[1];
        cmd->resp[2] = SDHC->CMDRSP[2];
        cmd->resp[3] = SDHC->CMDRSP[3];
    }
    return CH_OK;
}

/**
 * @brief SDHC initialize card
 * \note internal function
 * \param[in] cmd 指令指针
 * \retval   0     CH_OK
 * \retval other error code
 */                                                            
uint32_t SD_CardInit(uint32_t instance)
{
	volatile uint32_t delay = 0;
	uint8_t result;  
	uint32_t i = 0;
	bool hc = false;     
    SDHC_Cmd_t cmd;
    
	/* initalize 80 clock */
	SDHC->SYSCTL |= SDHC_SYSCTL_INITA_MASK;
	while (SDHC->SYSCTL & SDHC_SYSCTL_INITA_MASK){};

    /* CMD0 -> CMD8 -> while(CMD55+ACMD41) ->CMD2 -> CMD3 ->CMD9 -> CMD7-> CMD16->(CMD55+ACMD6) */
	cmd.cmd = ESDHC_CMD0;
	cmd.arg = 0;
	cmd.blkCount = 0;
    cmd.blkSize = 512;
	result = SDHC_SendCmd(&cmd);
	if(result != CH_OK) 
    {
        LIB_TRACE("CMD0 error\r\n");
        return CH_IO_ERR;
    }
    
	/* CMD8 */
	cmd.cmd = ESDHC_CMD8;
	cmd.arg =0x000001AA;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if (result > 0) 
	{
		result = CH_IO_ERR;
	}
	if (result == 0) //SDHC
	{
        LIB_TRACE("SDHC detected\r\n");
		hc = true;  					
	}
    
	do 
	{								 
		for(delay=0; delay<1000; delay++);
		i++;   
		cmd.cmd = ESDHC_CMD55;
		cmd.arg = 0;
        cmd.blkCount = 0;
        result = SDHC_SendCmd(&cmd);
		
		cmd.cmd = ESDHC_ACMD41;
		if(hc)
		{
			cmd.arg = 0x40300000;
		}
		else
		{
			cmd.arg = 0x00300000;
		}
		result = SDHC_SendCmd(&cmd);
	}while ((0 == (cmd.resp[0] & 0x80000000)) && (i < 300));
    if(i >= 300)
    {
        LIB_TRACE("SDcard Timeout\r\n");
        return CH_IO_ERR;
    }
    
	/* CMD2 CID */
	cmd.cmd = ESDHC_CMD2;
	cmd.arg = 0;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != CH_OK) 
    {
        return CH_IO_ERR;    
    }

	SDCard_Handle.CID[0] = cmd.resp[0];
	SDCard_Handle.CID[1] = cmd.resp[1];
	SDCard_Handle.CID[2] = cmd.resp[2];
	SDCard_Handle.CID[3] = cmd.resp[3];
	LIB_TRACE("CID[0]:0x%08X\r\n", SDCard_Handle.CID[0]);
    LIB_TRACE("CID[1]:0x%08X\r\n", SDCard_Handle.CID[1]);
    LIB_TRACE("CID[2]:0x%08X\r\n", SDCard_Handle.CID[2]);
    LIB_TRACE("CID[3]:0x%08X\r\n", SDCard_Handle.CID[3]);
	/* CMD3 RCA */
	cmd.cmd = ESDHC_CMD3;
	cmd.arg = 0;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != CH_OK) return CH_IO_ERR;   
    SDCard_Handle.RCA = cmd.resp[0]>>16;
    
	/* CMD9 CSD */
	cmd.cmd = ESDHC_CMD9;
	cmd.arg = SDCard_Handle.RCA<<16;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != CH_OK) return CH_IO_ERR; 
    /* The response is from bit 127:8 in R2, corrisponding to command.response[3U]:command.response[0U][31U:8]. */    
	SDCard_Handle.CSD[0] = cmd.resp[0];
	SDCard_Handle.CSD[1] = cmd.resp[1];
	SDCard_Handle.CSD[2] = cmd.resp[2];
	SDCard_Handle.CSD[3] = cmd.resp[3];
	LIB_TRACE("CSD[0]:0x%08X\r\n", SDCard_Handle.CSD[0]);
    LIB_TRACE("CSD[1]:0x%08X\r\n", SDCard_Handle.CSD[1]);
    LIB_TRACE("CSD[2]:0x%08X\r\n", SDCard_Handle.CSD[2]);
    LIB_TRACE("CSD[3]:0x%08X\r\n", SDCard_Handle.CSD[3]);
    
	/* CMD7 */ 
	cmd.cmd = ESDHC_CMD7;
	cmd.arg = SDCard_Handle.RCA<<16;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != CH_OK) return CH_IO_ERR;  
	/* MD16 */ 
	cmd.cmd = ESDHC_CMD16;
	cmd.arg = 512;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != CH_OK) return CH_IO_ERR;  
	
	/* CMD55 */
	cmd.cmd = ESDHC_CMD55;
	cmd.arg = SDCard_Handle.RCA<<16;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);
	if(result != CH_OK) return CH_IO_ERR;  
	/* ACMD6 */ 
	cmd.cmd = ESDHC_ACMD6;
	cmd.arg = 2;
	cmd.blkCount = 0;
	result = SDHC_SendCmd(&cmd);//
	if(result != CH_OK) return CH_IO_ERR;  

	SDHC->PROCTL &= (~SDHC_PROCTL_DTW_MASK);
	SDHC->PROCTL |=  SDHC_PROCTL_DTW(ESDHC_PROCTL_DTW_4BIT);

	if((SDCard_Handle.CSD[3]>>22)&0x03)
	{
        SDCard_Handle.type = CARD_TYPE_SDHC;
	}
	else
	{
        SDCard_Handle.type = CARD_TYPE_SD;
	}
    
	return CH_OK;	
}


/**
 * @brief SDHC initialize
 * \param[in] MAP
 * \param[in] sd clock
 * \retval CH_OK  successfull
 * \retval other  failed
 */
uint32_t SDHC_Init(uint32_t MAP, uint32_t sdClk)
{
    int i, ret;
    map_t * pq = (map_t*)&(MAP);
    
    MPU->CESR &= ~0x01;
    REG_SET(SDHCClkGate, 0);
    
    /* reset module */
	SDHC->SYSCTL = SDHC_SYSCTL_RSTA_MASK | SDHC_SYSCTL_RSTC_MASK | SDHC_SYSCTL_RSTD_MASK;
	while(SDHC->SYSCTL & SDHC_SYSCTL_RSTA_MASK);

    /* use ADMA2 mode */
	//SDHC->PROCTL = SDHC_PROCTL_EMODE(2)|SDHC_PROCTL_DMAS(2); 
    SDHC->PROCTL = SDHC_PROCTL_EMODE(2);
    
    /* set watermark */
	SDHC->WML = SDHC_WML_RDWML(0x80) | SDHC_WML_WRWML(0x80);

	SDHC_SetSDClk(pq->ip, sdClk);
	SDHC->IRQSTAT = 0xFFFFFFFF;
	SDHC->IRQSTATEN = 0xFFFFFFFF;

    PIN_SET_MUX;
    PIN_SET_PULL(1);
    
    for(i=0; i<10; i++)
    {
        ret = SD_CardInit(pq->ip);
        if(ret == CH_OK)
        {
            return CH_OK;
        }
        LIB_TRACE("SD init try%d\r\n", i);
    }
	return CH_ERR;
}

/**
 * @brief SDHC 块读操作
 * \param[in] sector 块
 * \param[out] buf 数据的存放地址
 * \param[in] len 个数
 * \retval   0     CH_OK
 * \retval other error code
 */                                                            
uint32_t SD_ReadBlock(uint32_t sector, uint8_t *buf, uint32_t cnt)
{
    volatile uint32_t timeout;
    uint32_t ret, i,j;
    uint32_t *p;
	SDHC_Cmd_t cmd; 
	if(SDCard_Handle.type == CARD_TYPE_SD)
    {
        sector = sector<<9;
    }
        
    cmd.cmd = (cnt == 1)?(ESDHC_CMD17):(ESDHC_CMD18);
    cmd.arg = sector; 
    cmd.blkSize = 512;
    cmd.blkCount = cnt;
    
   // SetADMA2Table(0, (uint32_t*)buffer, 512*len);
    if (((uint32_t)buf & 0x03) == 0)
    {
        ret = SDHC_SendCmd(&cmd);
        if(ret != CH_OK)
        {
            return CH_IO_ERR; 
        }
        
        p = (uint32_t*)buf;
        for(i=0; i<cnt; i++)
        {
            while(((SDHC->PRSSTAT & SDHC_PRSSTAT_BREN_MASK) == 0))
            {
                if(SDHC->IRQSTAT & (SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DEBE_MASK))
                {
                    LIB_TRACE("SDHC_WriteBlock error!\r\n");
                    return CH_IO_ERR;
                }
            }
            
            for (j = (512+3)>>2; j!=0; j--)
            {
                *p++ = SDHC->DATPORT;
            }
        }
    }
    
    return CH_OK;
}

/**
 * @brief SDHC 块写操作
 * \param[in] sector 块
 * \param[in] buf 待写入数据的地址
 * \param[in] len 个数
 * \retval   0     CH_OK
 * \retval other error code
 */                                                            
uint32_t SD_WriteBlock(uint32_t sector, uint8_t *buf, uint32_t cnt)
{
    uint16_t ret, i, j;
    uint32_t *p;
	SDHC_Cmd_t cmd; 
	if(SDCard_Handle.type == CARD_TYPE_SD)
    {
        sector = sector<<9;
    }
        
    cmd.cmd = (cnt == 1)?(ESDHC_CMD24):(ESDHC_CMD25);
    cmd.arg = sector; 
    cmd.blkSize = 512;
    cmd.blkCount = cnt;

   // SetADMA2Table(1, (uint32_t*)buf, 512*len);
    if (((uint32_t)buf & 0x03) == 0)
    {
    
        ret = SDHC_SendCmd(&cmd);
        if(ret != CH_OK)
        {
            return CH_IO_ERR; 
        }
    
        p = (uint32_t*)buf;
    
        for(i = 0; i < cnt; i++)
        {
            while((SDHC->PRSSTAT & SDHC_PRSSTAT_BWEN_MASK) == 0)
            {
                __NOP();
                if(SDHC->IRQSTAT & (SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DEBE_MASK))
                {
                    LIB_TRACE("SDHC_WriteBlock error!\r\n");
                    return CH_IO_ERR;
                }
            }

            for (j = (512+3)>>2;j!= 0;j--)
            {
                SDHC->DATPORT = *p++;
            }
        }
    }

    /* waitting for card is OK */
    i = 0;
    do
    {
        i++;
        cmd.cmd = ESDHC_CMD13;
        cmd.arg = SDCard_Handle.RCA<<16;
        cmd.blkCount = 0;
        
        cmd.resp[0] = 0;
        
        ret = SDHC_SendCmd(&cmd);
        
        if(ret != CH_OK)
        {
            continue;
        }
        if (cmd.resp[0] & 0xFFD98008)
        {
            break;
        }
        DelayMs(1);
    } while ((0x000000900 != (cmd.resp[0] & 0x00001F00)) && (i < 1000));
   return CH_OK;
}

/**
 * \brief read SD single block data
 * \param[in] sector 块
 * \param[out] buf 数据的存放地址
 * \retval   0     CH_OK
 * \retval other error code
 */ 
uint32_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buf)
{
    return SD_ReadBlock(sector, buf, 1);
}

/**
 * \brief write SD single block data
 * \param[in] sector 块
 * \param[in] buf 待写入数据的地址
 * \retval   0     CH_OK
 * \retval other error code
 */ 
uint32_t SD_WriteSingleBlock(uint32_t sector, uint8_t *buf)
{
    return SD_WriteBlock(sector, buf, 1);
}

/**
 * @brief GetSD size
 * @retval size in MB
 */ 
static uint32_t SD_GetSizeInMBFromCSD(uint32_t *csd)
{
	uint32_t block_number;
	uint32_t muti;
	uint32_t block_size;
	uint32_t size_in_mb;
	if((csd[3]>>22) & 0x03) /* SDHC  */
	{
		block_size = (csd[2]>>24) & 0xFF;
		size_in_mb = ((csd[1]>>8) & 0xFFFFFF)+1;

		size_in_mb = (size_in_mb + 1)/2;
        LIB_TRACE("SDHC card, block size:%d total size:%dMB\r\n", block_size, size_in_mb);
		return size_in_mb;
	}
	else /* non SDHC */
	{
        muti = (csd[1]>>7) & 0x7;
        muti = 2<<(muti+1);
        block_number = ((csd[2]>>0)&0x03);
        block_number = (block_number<<10) + ((csd[1]>>22) & 0x0FFF);
        block_number++;
        block_number = block_number * muti;
        block_size = (csd[2]>>8) & 0x0F;
        block_size = 2<<(block_size-1);
        size_in_mb = block_number * block_size;
        size_in_mb = size_in_mb/1024/1024;
        LIB_TRACE("SD card, block size:%d total size:%dMB\r\n", block_size, size_in_mb);
        return size_in_mb;
	}
}

uint32_t SD_GetSizeInMB(void)
{
    return SD_GetSizeInMBFromCSD(SDCard_Handle.CSD);
}


/**
 * @brief SD_ReadMultiBlock legcy support
 * \note this function is same as SDHC_ReadBlock(...)
 */ 		
inline uint32_t SD_ReadMultiBlock(uint32_t sector, uint8_t *buf, uint32_t cnt)
{
    return SD_ReadBlock(sector, buf, cnt);
}

/**
 * @brief SD_WriteMultiBlock legcy support
 * \note this function is same as SDHC_WriteBlock(...)
 */ 	
inline uint32_t SD_WriteMultiBlock(uint32_t sector, uint8_t *buf, uint32_t cnt)
{
    return SD_WriteBlock(sector, buf, cnt);
}

#endif
