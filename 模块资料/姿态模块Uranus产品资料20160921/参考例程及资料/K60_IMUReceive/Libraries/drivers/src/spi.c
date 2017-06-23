/**
  ******************************************************************************
  * @file    spi.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.06.12
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "spi.h"
#include "common.h"

#if defined(SPI0)

/************************************* DSPI 模块 *******************************/
#if defined(SPI_MCR_HALT_MASK)

#define SPI_CTAR0       (0x00)
#define SPI_CTAR1       (0x01)

#ifndef SPI_BASES
    #if defined(SPI2)
    #define SPI_BASES {SPI0, SPI1, SPI2};
    #elif defined(SPI1)
    #define SPI_BASES {SPI0, SPI1};
    #else
    #define SPI_BASES {SPI0};
    #endif
#endif

SPI_Type * const SPIBases[] = SPI_BASES;

static const Reg_t SPIClkGate[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_SPI0_MASK, SIM_SCGC6_SPI0_SHIFT},
#if defined(SIM_SCGC6_SPI1_MASK)
    {(void*)&(SIM->SCGC6), SIM_SCGC6_SPI1_MASK, SIM_SCGC6_SPI1_SHIFT},
#endif
#if defined(SIM_SCGC3_SPI2_MASK)
    {(void*)&(SIM->SCGC3), SIM_SCGC3_SPI2_MASK, SIM_SCGC3_SPI2_SHIFT},
#endif
};

static const IRQn_Type SPI_IRQTbl[] =
{
    (IRQn_Type)(SPI0_IRQn + 0),
#if defined(SPI1)
    (IRQn_Type)(SPI1_IRQn + 0),
#endif
 #if defined(SPI2)
    (IRQn_Type)(SPI2_IRQn + 0),
 #endif
};


/* Defines constant value arrays for the baud rate pre-scalar and scalar divider values.*/
static const uint32_t s_baudratePrescaler[] = { 2, 3, 5, 7 };
static const uint32_t s_baudrateScaler[] = { 2, 4, 6, 8, 16, 32, 64, 128, 256, 512, 1024, 2048,
                                           4096, 8192, 16384, 32768 };

																					 
static uint32_t dspi_hal_set_baud(uint32_t instance, uint8_t whichCtar, uint32_t bitsPerSec, uint32_t sourceClockInHz)                    
{
    uint32_t prescaler, bestPrescaler;
    uint32_t scaler, bestScaler;
    uint32_t dbr, bestDbr;
    uint32_t realBaudrate, bestBaudrate;
    uint32_t diff, min_diff;
    uint32_t baudrate = bitsPerSec;
    /* for master mode configuration, if slave mode detected, return 0*/
    if(!(SPIBases[instance]->MCR & SPI_MCR_MSTR_MASK))
    {
        return 0;
    }
    /* find combination of prescaler and scaler resulting in baudrate closest to the */
    /* requested value */
    min_diff = 0xFFFFFFFFU;
    bestPrescaler = 0;
    bestScaler = 0;
    bestDbr = 1;
    bestBaudrate = 0; /* required to avoid compilation warning */

    /* In all for loops, if min_diff = 0, the exit for loop*/
    for (prescaler = 0; (prescaler < 4) && min_diff; prescaler++)
    {
        for (scaler = 0; (scaler < 16) && min_diff; scaler++)
        {
            for (dbr = 1; (dbr < 3) && min_diff; dbr++)
            {
                realBaudrate = ((sourceClockInHz * dbr) /
                                (s_baudratePrescaler[prescaler] * (s_baudrateScaler[scaler])));

                /* calculate the baud rate difference based on the conditional statement*/
                /* that states that the calculated baud rate must not exceed the desired baud rate*/
                if (baudrate >= realBaudrate)
                {
                    diff = baudrate-realBaudrate;
                    if (min_diff > diff)
                    {
                        /* a better match found */
                        min_diff = diff;
                        bestPrescaler = prescaler;
                        bestScaler = scaler;
                        bestBaudrate = realBaudrate;
                        bestDbr = dbr;
                    }
                }
            }
        }
    }

    uint32_t temp;
    /* write the best dbr, prescalar, and baud rate scalar to the CTAR*/
    temp = SPIBases[instance]->CTAR[whichCtar];
    temp &= ~(SPI_CTAR_DBR_MASK| SPI_CTAR_PBR_MASK | SPI_CTAR_BR_MASK);
    if((bestDbr-1))
    {
        temp |= SPI_CTAR_DBR_MASK|SPI_CTAR_PBR(bestPrescaler)|SPI_CTAR_BR(bestScaler);
    }
    else
    {
        temp |= SPI_CTAR_PBR(bestPrescaler)|SPI_CTAR_BR(bestScaler);
    }
    SPIBases[instance]->CTAR[whichCtar] = temp;
    /* return the actual calculated baud rate*/
    LIB_TRACE("bestBaudrate:%d\r\n", bestBaudrate);
    return bestBaudrate;
}

/**
 * @brief  配置SPI模块
 * @note   None
 * @param  instance:
 *         @arg HW_SPI0 : SPI0模块
 *         @arg HW_SPI1 : SPI1模块
 *         @arg HW_SPI2 : SPI2模块
 * @param  MAP:引脚位图，详见spi.h文件
 * @param  format: 通信格式
 * @retval None
 */
void SPI_SetFormat(uint32_t instance, SPI_Format_t format)
{
    SPI_Type *SPIx = SPIBases[instance];
    uint8_t bit;
    
    SPIx->MCR |= SPI_MCR_HALT_MASK;
    
    /* data size */
    //bit =  (format & SPI_CONFIG_16BIT)?(16):(8);
    bit = 8;
    LIB_TRACE("SPI frame bit is:%d\r\n", bit);
    SPIx->CTAR[SPI_CTAR0] &= ~SPI_CTAR_FMSZ_MASK;
    SPIx->CTAR[SPI_CTAR0] |= SPI_CTAR_FMSZ(bit - 1);
    
    switch(format)
    {
        case kSPI_CPOL0_CPHA0:
            SPIx->CTAR[SPI_CTAR0] &= ~(SPI_CTAR_CPOL_MASK | SPI_CTAR_CPHA_MASK);
            break;
        case kSPI_CPOL0_CPHA1:
            SPIx->CTAR[SPI_CTAR0] &= ~SPI_CTAR_CPOL_MASK;
            SPIx->CTAR[SPI_CTAR0] |= SPI_CTAR_CPHA_MASK;
            break;
        case kSPI_CPOL1_CPHA1:
            SPIx->CTAR[SPI_CTAR0] |= (SPI_CTAR_CPOL_MASK | SPI_CTAR_CPHA_MASK);
            break;
        case kSPI_CPOL1_CPHA0:
            SPIx->CTAR[SPI_CTAR0] |= SPI_CTAR_CPOL_MASK;
            SPIx->CTAR[SPI_CTAR0] &= ~SPI_CTAR_CPHA_MASK;
            break;
        default:
            break;
    }

    SPIx->CTAR[SPI_CTAR0] &= ~SPI_CTAR_LSBFE_MASK;
    
    /* add more CS time */
    SPIx->CTAR[SPI_CTAR0] |= SPI_CTAR_ASC(1)|SPI_CTAR_CSSCK(1)|SPI_CTAR_PASC(1)|SPI_CTAR_PCSSCK(1);  
    
    SPIx->MCR &= ~SPI_MCR_HALT_MASK;
}
/**
 * @brief  设置SPI模块发送先入先出模式
 * @note   None
 * @param  instance:
 *         @arg HW_SPI0 : SPI0模块
 *         @arg HW_SPI1 : SPI1模块
 *         @arg HW_SPI2 : SPI2模块
 * @param  val: 
 *         @arg true : 开启
 *         @arg false: 关闭
 * @retval None
 */
void SPI_SetTxFIFO(uint32_t instance, bool val)
{
    SPI_Type *SPIx = SPIBases[instance];
    (val == true)?(SPIx->MCR &= ~SPI_MCR_DIS_TXF_MASK):(SPIx->MCR |= SPI_MCR_DIS_TXF_MASK);
}

/**
 * @brief  设置SPI模块接收先入先出模式
 * @note   None
 * @param  instance:
 *         @arg HW_SPI0 : SPI0模块
 *         @arg HW_SPI1 : SPI1模块
 *         @arg HW_SPI2 : SPI2模块
 * @param  val: 
 *         @arg true : 开启
 *         @arg false: 关闭
 * @retval None
 */
void SPI_SetRxFIFO(uint32_t instance, bool val)
{
    SPI_Type *SPIx = SPIBases[instance];
    (val == true)?(SPIx->MCR &= ~SPI_MCR_DIS_RXF_MASK):(SPIx->MCR |= SPI_MCR_DIS_RXF_MASK);
}

/**
 * @brief  初始化配置SPI
 * @note   None
 * @param  MAP:引脚位图，详见spi.h文件
 * @param  baudRate : 通信速度，单位Hz
 * @retval 0:SPI0模块；1:SPI1模块；2:SPI2模块
 */
uint32_t SPI_Init(uint32_t MAP, uint32_t baudrate)
{
    map_t * pq = (map_t*)&(MAP);
    uint32_t instance = pq->ip;
    SPI_Type *SPIx = SPIBases[instance];
    uint32_t clock = GetClock(kBusClock);
    REG_SET(SPIClkGate, instance);

    /* halt mode, disable FIFO, master */
    SPIx->MCR = SPI_MCR_HALT_MASK;
    SPIx->MCR |= SPI_MCR_MSTR_MASK | SPI_MCR_PCSIS_MASK | SPI_MCR_CLR_TXF_MASK | SPI_MCR_CLR_RXF_MASK | SPI_MCR_DIS_TXF_MASK | SPI_MCR_DIS_RXF_MASK;
    
    SPI_SetFormat(instance, kSPI_CPOL0_CPHA0);
    
    /* enable SPI clock */
    SPIx->MCR &= ~SPI_MCR_MDIS_MASK;

    dspi_hal_set_baud(instance, SPI_CTAR0, baudrate, clock);
    SPIx->SR = 0xFFFFFFFF;
    SPIx->MCR &= ~SPI_MCR_HALT_MASK;
    PIN_SET_MUX;
    return instance;
}

/**
 * @brief  SPI模块读写
 * @note   在主模式下
 * @param  instance:
 *         @arg HW_SPI0 : SPI0模块
 *         @arg HW_SPI1 : SPI1模块
 *         @arg HW_SPI2 : SPI2模块
 * @param  ctar : 通信属性选择0或1
 * @param  data : 写数据
 * @param  cs : 片选通道
 * @param  states : 通信状态，1开启传输；0关闭传输
 * @retval 读数据
 */
uint32_t SPI_ReadWriteEx(uint32_t instance, uint32_t ctar, uint32_t data, uint16_t cs, uint32_t states)
{
    SPIBases[instance]->SR |= SPI_SR_TCF_MASK;
    
    SPIBases[instance]->PUSHR = ((states)?(0):(SPI_PUSHR_CONT_MASK)) | SPI_PUSHR_CTAS(ctar) | SPI_PUSHR_PCS(1 << cs) | SPI_PUSHR_TXDATA(data);

    /* waitting for complete */
    while(!(SPIBases[instance]->SR & SPI_SR_TCF_MASK));
    
    return (uint16_t)SPIBases[instance]->POPR; 
}

/**
 * @brief  SPI模块读写
 * @note   None
 * @param  instance:
 *         @arg HW_SPI0 : SPI0模块
 *         @arg HW_SPI1 : SPI1模块
 *         @arg HW_SPI2 : SPI2模块
 * @param  data : 写数据
 * @retval 读数据
 */
inline uint32_t SPI_ReadWrite(uint32_t instance, uint32_t data)
{
    return SPI_ReadWriteEx(instance, SPI_CTAR0, data, 0, 0);
}

 /**
 * @brief  设置SPI中断模式
 * @note   None
 * @param  instance:
 *         @arg HW_SPI0 : SPI0模块
 *         @arg HW_SPI1 : SPI1模块
 *         @arg HW_SPI2 : SPI2模块
 * @param  status: 
 *         @arg true : 开启中断
 *         @arg flase : 关闭中断
* @retval  0:成功；其它：错误
 */
uint32_t SPI_SetIntMode(uint32_t instance, SPI_Int_t mode, bool val)
{
    REG_SET(SPIClkGate, instance);
    SPI_Type * SPIx = (SPI_Type*)SPIBases[instance];
    NVIC_EnableIRQ(SPI_IRQTbl[instance]);
    switch(mode)
    {
        case kSPI_TCF:
            (val)?(SPIx->RSER |= SPI_RSER_TCF_RE_MASK):(SPIx->RSER &= ~SPI_RSER_TCF_RE_MASK);
            break;
        default:
            break;
    }
    return 0;
}



#endif



/************************************* SPI 模块 *******************************/
#if defined(SPI_C1_SPE_MASK)

SPI_Type * const SPIBases[] = SPI_BASES;

static const Reg_t SPIClkGate[] =
{
    {(void*)&(SIM->SCGC4), SIM_SCGC4_SPI0_MASK, SIM_SCGC4_SPI0_SHIFT},
#if defined(SPI1)
    {(void*)&(SIM->SCGC4), SIM_SCGC4_SPI1_MASK, SIM_SCGC4_SPI1_SHIFT},
#endif
};
static const IRQn_Type SPI_IRQTbl[] = 
{
    SPI0_IRQn,
#if defined(SPI1)
    SPI1_IRQn,
#endif
};

/*FUNCTION**********************************************************************
 *
 * @note 此功能依据用户输入的期望频率计算出单片机能够提供的最相近频率，
 *       需要用户输入SPI模块的时钟频率，该函数返回相近的通信频率
 *
 *END**************************************************************************/
static uint32_t SPI_HAL_SetBaud(uint32_t instance, uint32_t bitsPerSec, uint32_t sourceClockInHz)
{
    uint32_t prescaler, bestPrescaler;
    uint32_t rateDivisor, bestDivisor;
    uint32_t rateDivisorValue;
    uint32_t realBaudrate, bestBaudrate;
    uint32_t diff, min_diff;
    uint32_t baudrate = bitsPerSec;

    /* find combination of prescaler and scaler resulting in baudrate closest to the
     * requested value
     */
    min_diff = 0xFFFFFFFFU;
    bestPrescaler = 0;
    bestDivisor = 0;
    bestBaudrate = 0; /* required to avoid compilation warning */

    /* In all for loops, if min_diff = 0, the exit for loop*/
    for (prescaler = 0; (prescaler <= 7) && min_diff; prescaler++)
    {
        rateDivisorValue = 2U;  /* Initialize to div-by-2 */

        for (rateDivisor = 0; (rateDivisor <= 8U) && min_diff; rateDivisor++)
        {
            /* calculate actual baud rate, note need to add 1 to prescaler */
            realBaudrate = ((sourceClockInHz) /
                            ((prescaler + 1) * rateDivisorValue));

            /* calculate the baud rate difference based on the conditional statement*/
            /* that states that the calculated baud rate must not exceed the desired baud rate*/
            if (baudrate >= realBaudrate)
            {
                diff = baudrate-realBaudrate;
                if (min_diff > diff)
                {
                    /* a better match found */
                    min_diff = diff;
                    bestPrescaler = prescaler; /* Prescale divisor SPIx_BR register bit setting */
                    bestDivisor = rateDivisor; /* baud rate divisor SPIx_BR register bit setting */
                    bestBaudrate = realBaudrate;
                }
            }
            /* Multiply by 2 for each iteration, possible divisor values: 2, 4, 8, 16, ... 512 */
            rateDivisorValue *= 2U;
        }
    }

    /* write the best prescalar and baud rate scalar */
    SPIBases[instance]->BR &= ~SPI_BR_SPR_MASK;
    SPIBases[instance]->BR &= ~SPI_BR_SPPR_MASK;
    SPIBases[instance]->BR |= SPI_BR_SPR(bestDivisor);
    SPIBases[instance]->BR |= SPI_BR_SPPR(bestPrescaler);

    /* return the actual calculated baud rate*/
    return bestBaudrate;
}

/**
 * @brief  初始化配置SPI
 * @note   None
 * @param  MAP:引脚位图，详见spi.h文件
 * @param  baudRate_Hz: 通信速度
 * @retval 0:SPI0模块；1:SPI1模块；2:SPI2模块
 */
uint32_t SPI_Init(uint32_t MAP, uint32_t baudrate)
{
    map_t * pq = (map_t*)&(MAP);
    uint32_t instance = pq->ip;
    SPI_Type *SPIx = SPIBases[instance];
    REG_SET(SPIClkGate, instance);
    
    SPIx->C1 &= ~SPI_C1_SPE_MASK;
    
    /* config*/
    SPIx->C1 = 0;

    /* config SS as PCS0 output */
    SPIx->C2 |= SPI_C2_MODFEN_MASK;
    SPIx->C1 |= SPI_C1_SSOE_MASK;
    
    if(instance == HW_SPI0)
    {
        SPI_HAL_SetBaud(instance, baudrate, GetClock(kBusClock));
    }
    else
    {
        SPI_HAL_SetBaud(instance, baudrate, GetClock(kCoreClock)); 
    }
    SPIx->C1 |= SPI_C1_SPE_MASK;
    PIN_SET_MUX;
    return instance;
}

/**
 * @brief  配置SPI模块
 * @note   None
 * @param  instance:无意义
 *         @arg HW_SPI0 : SPI0模块
 *         @arg HW_SPI1 : SPI1模块
 *         @arg HW_SPI2 : SPI2模块
 * @param  MAP:引脚位图，详见spi.h文件
 * @param  format: 通信格式
 * @retval None
 */
void SPI_SetFormat(uint32_t instance, uint32_t format)
{
    SPI_Type *SPIx = SPIBases[instance];
    SPIx->C1 &= ~SPI_C1_SPE_MASK;
    
    (format & SPI_CONFIG_SLAVE)?(SPIx->C1 &= ~SPI_C1_MSTR_MASK):(SPIx->C1 |= SPI_C1_MSTR_MASK);
    (format & SPI_CONFIG_LSB)?(SPIx->C1 |= SPI_C1_LSBFE_MASK):(SPIx->C1 &= ~SPI_C1_LSBFE_MASK);
    (format & SPI_CONFIG_CPOL)?(SPIx->C1 |= SPI_C1_CPOL_MASK):(SPIx->C1 &= ~SPI_C1_CPOL_MASK);
    (format & SPI_CONFIG_CPHA)?(SPIx->C1 |= SPI_C1_CPHA_MASK):(SPIx->C1 &= ~SPI_C1_CPHA_MASK);
    
    SPIx->C1 |= SPI_C1_SPE_MASK;
}

/**
 * @brief  SPI模块读写
 * @note   None
 * @param  instance:
 *         @arg HW_SPI0 : SPI0模块
 *         @arg HW_SPI1 : SPI1模块
 *         @arg HW_SPI2 : SPI2模块
 * @param  data : 写数据
 * @retval 读数据
 */
uint32_t SPI_ReadWrite(uint32_t instance, uint32_t data)
{
    uint16_t temp;
    while(!(SPIBases[instance]->S & SPI_S_SPTEF_MASK));
    #ifdef SPI_DL_Bits_MASK
    SPIBases[instance]->DL = (data & 0x00FF);
    SPIBases[instance]->DH = (data & 0xFF00)>>8;
    while(!(SPIBases[instance]->S & SPI_S_SPRF_MASK));
    temp =  SPIBases[instance]->DL + (SPIBases[instance]->DH<<8);
    #endif
    
    #ifdef SPI_D_Bits_MASK
    SPIBases[instance]->D = (data & 0x00FF);
    while(!(SPIBases[instance]->S & SPI_S_SPRF_MASK));
    temp =  SPIBases[instance]->D;
    #endif
    return temp;
}

 /**
 * @brief  设置SPI中断模式
 * @note   None
 * @param  instance:
 *         @arg HW_SPI0 : SPI0模块
 *         @arg HW_SPI1 : SPI1模块
 *         @arg HW_SPI2 : SPI2模块
 * @param  status: 
 *         @arg true : 开启中断
 *         @arg flase : 关闭中断
* @retval  0:成功；其它：错误
 */
uint32_t SPI_SetIntMode(uint32_t instance, SPI_Int_t mode, bool val)
{
    REG_SET(SPIClkGate, instance);
    SPI_Type * SPIx = (SPI_Type*)SPIBases[instance];
    NVIC_EnableIRQ(SPI_IRQTbl[instance]);

    switch(mode)
    {
        case kSPI_TCF:
            (val)?(SPIx->C1 |= SPI_C1_SPTIE_MASK):(SPIx->C1 &= ~SPI_C1_SPTIE_MASK);
            break;
        default:
            break;
    }
    return 0;
}

void SPI_IRQHandler(uint32_t instance)
{
    volatile uint8_t dummy;
    dummy = dummy;
    if((SPIBases[HW_SPI0]->C1 & SPI_C1_SPIE_MASK) && (SPIBases[HW_SPI0]->S & SPI_S_SPRF_MASK))
    {
        /* make sure clear SPRF bit */
        #ifdef SPI_DL_Bits_MASK
        dummy = SPIBases[HW_SPI0]->DL;
        #endif
        
        #ifdef SPI_D_Bits_MASK
        dummy = SPIBases[HW_SPI0]->D;
        #endif
    }
}


#endif

#if 0
static const QuickInit_Type SPI_QuickInitTable[] =
{
    { 1, 4, 2, 0, 3, 0}, //SPI1_SCK_PE02_MOSI_PE01_MISO_PE00
    { 0, 4, 2,17, 3, 0}, //SPI0_SCK_PE17_MOSI_PE18_MISO_PE19
    { 0, 0, 2,15, 3, 0}, //SPI0_SCK_PA15_MOSI_PA16_MISO_PA17
    { 0, 2, 2, 5, 3, 0}, //SPI0_SCK_PC05_MOSI_PC06_MISO_PC07
    { 0, 3, 2, 1, 3, 0}, //SPI0_SCK_PD01_MOSI_PD02_MISO_PD03
    { 1, 3, 2, 5, 3, 0}, //SPI1_SCK_PD05_MOSI_PD06_MISO_PD07
};
#endif

#endif
