/**
  ******************************************************************************
  * @file    i2c.c
  * @author  YANDLD
  * @version V3.0
  * @date    2016.6.7
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "i2c.h"
#include "common.h"


#ifndef GPIO_I2C
#define GPIO_I2C        1
#endif

#if (GPIO_I2C == 0)

#if defined(LPI2C0)

static bool gLPI2C_IsStartByte = true;
/**
  ******************************************************************************
  * @note    LPI2C module
  ******************************************************************************
  */

#if defined(LPI2C_BASES)
static const void* LPI2CBases[] = LPI2C_BASES;
#else
static const void* LPI2CBases[] = LPI2C_BASE_PTRS;
#endif


static const Reg_t LPI2CClkGate[] =
{
    #if defined(SIM_SCGC4_LPI2C0_MASK)
    {(void*)&(SIM->SCGC4), SIM_SCGC4_LPI2C0_MASK, SIM_SCGC4_LPI2C0_SHIFT},
    #elif defined(PCC1_PCC_LPI2C0_CGC_MASK)
    {(void*)&(PCC1->PCC_LPI2C0), PCC1_PCC_LPI2C0_CGC_MASK, PCC1_PCC_LPI2C0_CGC_SHIFT},
    #else
    #error "No LPI2C0 clock gate"
    #endif
    
    #if defined(LPI2C1)
        #if defined(SIM_SCGC4_LPI2C1_MASK)
        {(void*)&(SIM->SCGC4), SIM_SCGC4_LPI2C1_MASK, SIM_SCGC4_LPI2C1_SHIFT},
        #elif defined(PCC1_PCC_LPI2C1_CGC_MASK)
        {(void*)&(PCC1->PCC_LPI2C1), PCC1_PCC_LPI2C1_CGC_MASK, PCC1_PCC_LPI2C1_CGC_SHIFT},
        #else
        #error "No LPI2C1 clock gate"
        #endif
    #endif
    
    #if defined(LPI2C2)
        #if defined(SIM_SCGC4_LPI2C2_MASK)
        {(void*)&(SIM->SCGC4), SIM_SCGC4_LPI2C2_MASK, SIM_SCGC4_LPI2C2_SHIFT},
        #elif defined(PCC0_PCC_LPI2C2_CGC_MASK)
        {(void*)&(PCC0->PCC_LPI2C2), PCC0_PCC_LPI2C2_CGC_MASK, PCC0_PCC_LPI2C2_CGC_SHIFT},
        #else
        #error "No LPI2C2 clock gate"
        #endif
    #endif

};

static void LPI2C_MasterSetBaudRate(LPI2C_Type *base, uint32_t sourceClock_Hz, uint32_t baudRate_Hz)
{
    uint32_t prescale   = 0;
    uint32_t bestPre    = 0;
    uint32_t bestclkHi  = 0;
    uint32_t bestError  = 0xffffffffu;
    uint32_t clockInKHz = sourceClock_Hz / 1000u;
    uint32_t kbps       = baudRate_Hz / 1000u;
    uint32_t value;
    int i;

    /* Disable master mode. */
    base->MCR &= ~LPI2C_MCR_MEN_MASK;

    /*
     * Baud rate = (sourceClockInHZ/2^prescale)/(CLKLO+1+CLKHI+1)
     * Assume CLKLO = 2*CLKHI, SETHOLD = CLKHI, DATAVD = CLKHI/2.
     */
    for (prescale = 1; (prescale <= 128) && (bestError != 0); prescale = 2 * prescale)
    {
        uint32_t clkHiCycle;
        for (clkHiCycle = 1; clkHiCycle < 32; clkHiCycle++)
        {
            uint32_t computedRate = (clockInKHz / prescale) / (3 * clkHiCycle + 2);
            uint32_t absError     = kbps > computedRate ? kbps - computedRate : computedRate - kbps;
            if (absError < bestError)
            {
                bestPre   = prescale;
                bestclkHi = clkHiCycle;
                bestError = absError;

                /*
                 * If the error is 0, then we can stop searching
                 * because we won't find a better match.
                 */
                if (absError == 0)
                {
                    break;
                }
            }
        }
    }
    LIB_TRACE("LPI2C: Pre:%d clkHi:%d Error:%d\r\n", bestPre, bestclkHi, bestError);

    /* Standard, fast, fast mode plus and ultra-fast transfers. */
    value = LPI2C_MCCR0_CLKHI(bestclkHi);
    if (bestclkHi < 2)
    {
        value |= LPI2C_MCCR0_CLKLO(3) | LPI2C_MCCR0_SETHOLD(2) | LPI2C_MCCR0_DATAVD(1);
    }
    else
    {
        value |= LPI2C_MCCR0_CLKLO(2 * bestclkHi) | LPI2C_MCCR0_SETHOLD(bestclkHi) | LPI2C_MCCR0_DATAVD(bestclkHi / 2);
    }
    base->MCCR0 = value;

    for (i = 0; i < 8; i++)
    {
        if (bestPre == (1U << i))
        {
            bestPre = i;
            break;
        }
    }
    base->MCFGR1 = (base->MCFGR1 & ~LPI2C_MCFGR1_PRESCALE_MASK) | LPI2C_MCFGR1_PRESCALE(bestPre);

    /* Restore master mode. */
    base->MCR |= LPI2C_MCR_MEN_MASK;
}

static int32_t LPI2CGetClock(uint32_t instance)
{
    int clk;
    #if defined(PCC1_PCC_LPI2C0_CGC_MASK)
    REG_CLR(LPI2CClkGate, instance);
    *((uint32_t*)LPI2CClkGate[instance].addr) = PCC1_PCC_LPI2C0_PCS(3);
    REG_SET(LPI2CClkGate, instance);
    clk = 48*1000*1000;
    #endif
    
#ifdef SIM_SOPT2_LPI2C0SRC_MASK
    clk = GetClock(kMCGOutClock);
    if(instance == 0)
    {
        SIM->SOPT2 &= ~SIM_SOPT2_LPI2C0SRC_MASK;
        SIM->SOPT2 |= SIM_SOPT2_LPI2C0SRC(1);  
    }
    
    if(instance == 1)
    {
        SIM->SOPT2 &= ~SIM_SOPT2_LPI2C1SRC_MASK;
        SIM->SOPT2 |= SIM_SOPT2_LPI2C1SRC(1);  
    }

    #ifdef MCG_C6_PLLS_MASK
        if(MCG->C6 & MCG_C6_PLLS_MASK) /* PLL */
        {
            LIB_TRACE("PLL is slected\r\n");
            SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
            clk /= 2;
        }
        else /* FLL */
        {
            LIB_TRACE("FLL is slected\r\n");
            SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
        }
        #endif /* MCG_C6_PLLS_MASK */
#endif
    return clk;
}

/**
 * @brief  初始化 I2C模块
 * @note   None
 * @param  MAP:初始化信息，详见I2C.h
 * @param  baudrate: 通信速度
 * @retval I2Cx  I2C模块号
 */
uint32_t I2C_Init(uint32_t MAP, uint32_t baudrate)
{
    map_t * pq = (map_t*)&(MAP);
    uint32_t instance = pq->ip;
    LPI2C_Type *LPI2Cx = (LPI2C_Type*)LPI2CBases[instance];

    REG_CLR(LPI2CClkGate, instance);
    REG_SET(LPI2CClkGate, instance);
    
    /* reset */
    LPI2Cx->MCR = LPI2C_MCR_RST_MASK;
    LPI2Cx->MCR = LPI2C_MCR_DBGEN_MASK | LPI2C_MCR_DOZEN_MASK;
    LPI2Cx->MSR = 0xFFFFFFFF;

    /* watermark */
    LPI2Cx->MFCR = LPI2C_MFCR_TXWATER(0) | LPI2C_MFCR_RXWATER(0);
    LPI2C_MasterSetBaudRate(LPI2Cx, LPI2CGetClock(instance), baudrate);
    
    PIN_SET_MUX;
    LPI2Cx->MCR |= LPI2C_MCR_MEN_MASK;
    
    return pq->ip;
}

uint8_t I2C_GetByte(uint32_t instance)
{
    uint32_t data;
    LPI2C_Type *LPI2Cx = (LPI2C_Type*)LPI2CBases[instance];
    LPI2Cx->MTDR = LPI2C_MTDR_CMD(1);
    while(1)
    {
        data = LPI2Cx->MRDR;
        if((data & LPI2C_MRDR_RXEMPTY_MASK) == 0)
        {
            return data & 0xff;
        }
    }
}

static void I2C_Start(uint32_t instance)
{
    gLPI2C_IsStartByte = true;
}

static void I2C_Stop(uint32_t instance)
{
    LPI2C_Type *LPI2Cx = (LPI2C_Type*)LPI2CBases[instance];
    LPI2Cx->MTDR = LPI2C_MTDR_CMD(2);
}

uint8_t I2C_SendByte(uint32_t instance, uint8_t data)
{
    uint8_t ret = 0;
    LPI2C_Type *LPI2Cx = (LPI2C_Type*)LPI2CBases[instance];
    
    while (((LPI2Cx->MFSR & LPI2C_MFSR_TXCOUNT_MASK) >> LPI2C_MFSR_TXCOUNT_SHIFT) != 0) {};
    LPI2Cx->MTDR = LPI2C_MTDR_CMD((gLPI2C_IsStartByte)?(4):(0)) | data;
    DelayUs(100); 

    /* get ack status */
    (LPI2Cx->MSR & LPI2C_MSR_NDF_MASK)?(ret = CH_ERR):(ret = CH_OK);
    LPI2Cx->MSR = LPI2C_MSR_NDF_MASK;
    gLPI2C_IsStartByte = false;
    return ret;
}

/**
 * @brief  I2C连续性读取多字节
 * @note   None
 * @param  instance:
 *         @arg HW_I2C0 : I2C0模块
 *         @arg HW_I2C1 : I2C1模块
 *         @arg HW_I2C1 : I2C1模块
 * @param  addr: 目标设备地址
 * @param  regAddr: 寄存器地址
 * @param  regLen: 寄存器长度
 * @param  buf: 准备发送的数据地址指针
 * @param  len: 数据长度
 * @retval CH_OK：成功；CH_ERR：失败
 */
uint32_t I2C_BurstRead(uint32_t instance, uint8_t addr, uint32_t regAddr, uint32_t regLen, uint8_t *buf, uint32_t len)
{
    uint8_t ret;
    uint8_t *p;
    uint32_t tmp;

    p = (uint8_t*)&regAddr;
    
    LPI2C_Type *LPI2Cx = (LPI2C_Type*)LPI2CBases[instance];
    addr <<= 1;
    
    /* start & addr */
    while(LPI2Cx->MSR & LPI2C_MSR_BBF_MASK);
    I2C_Start(instance);
    ret = I2C_SendByte(instance, addr);
    
    if(ret != CH_OK)
    {
        I2C_Stop(instance);
        return ret;
    }
    
    while(regLen--)
    {
        I2C_SendByte(instance, *p++);
    }
    
    /* restart */
    I2C_Start(instance);
    I2C_SendByte(instance, addr | 1);
    
    LPI2Cx->MTDR = LPI2C_MTDR_CMD(1) | (len-1);
    while(len--)
    {
        do
        {
             tmp = LPI2Cx->MRDR;
        }while(tmp & LPI2C_MRDR_RXEMPTY_MASK);
        
        *buf++ = tmp & 0xFF;
    }
    
    I2C_Stop(instance);
    return CH_OK;
}

#endif

#if defined(I2C0)

/**
  ******************************************************************************
  * @note    I2C module
  ******************************************************************************
  */

#ifndef I2C_BASES
#define I2C_BASES {I2C0};
#endif

#if !defined(I2C_S_REG)
#define  I2C_S_REG(base)                   ((base)->S)
#endif

const void* I2CBases[] = I2C_BASES;

static const Reg_t I2CClkGate[] =
{
    {(void*)&(SIM->SCGC4), SIM_SCGC4_I2C0_MASK, SIM_SCGC4_I2C0_SHIFT},
    
#if defined(SIM_SCGC4_I2C1_MASK) 
    {(void*)&(SIM->SCGC4), SIM_SCGC4_I2C1_MASK, SIM_SCGC4_I2C1_SHIFT},
#endif
    
#if defined(SIM_SCGC1_I2C2_MASK) 
    {(void*)&(SIM->SCGC1), SIM_SCGC1_I2C2_MASK, SIM_SCGC1_I2C2_SHIFT},
#endif
    
};

//static const IRQn_Type I2C_IRQTbl[] =
//{
//    (IRQn_Type)(I2C0_IRQn + 0),
//    (IRQn_Type)(I2C1_IRQn + 0),
//};

typedef struct I2CDividerTableEntry {
    uint8_t icr;            /*!< F register ICR value.*/
    uint16_t sclDivider;    /*!< SCL clock divider.*/
} i2c_divider_table_entry_t;

const i2c_divider_table_entry_t kI2CDividerTable[] = {
        /* ICR  Divider*/
        { 0x00, 20 },
        { 0x01, 22 },
        { 0x02, 24 },
        { 0x03, 26 },
        { 0x04, 28 },
        { 0x05, 30 },
        { 0x09, 32 },
        { 0x06, 34 },
        { 0x0a, 36 },
        { 0x07, 40 },
        { 0x0c, 44 },
        { 0x10, 48 },
        { 0x11, 56 },
        { 0x12, 64 },
        { 0x0f, 68 },
        { 0x13, 72 },
        { 0x14, 80 },
        { 0x15, 88 },
        { 0x19, 96 },
        { 0x16, 104 },
        { 0x1a, 112 },
        { 0x17, 128 },
        { 0x1c, 144 },
        { 0x1d, 160 },
        { 0x1e, 192 },
        { 0x22, 224 },
        { 0x1f, 240 },
        { 0x23, 256 },
        { 0x24, 288 },
        { 0x25, 320 },
        { 0x26, 384 },
        { 0x2a, 448 },
        { 0x27, 480 },
        { 0x2b, 512 },
        { 0x2c, 576 },
        { 0x2d, 640 },
        { 0x2e, 768 },
        { 0x32, 896 },
        { 0x2f, 960 },
        { 0x33, 1024 },
        { 0x34, 1152 },
        { 0x35, 1280 },
        { 0x36, 1536 },
        { 0x3a, 1792 },
        { 0x37, 1920 },
        { 0x3b, 2048 },
        { 0x3c, 2304 },
        { 0x3d, 2560 },
        { 0x3e, 3072 },
        { 0x3f, 3840 }
    };

		
static void I2C_HAL_SetBaudRate(uint32_t instance,  uint32_t sourceClockInHz, uint32_t kbps, uint32_t * absoluteError_Hz)
{
    uint32_t mult, i, multiplier, computedRate, absError;
    uint32_t hz = kbps * 1000u;
    uint32_t bestError = 0xffffffffu;
    uint32_t bestMult = 0u;
    uint32_t bestIcr = 0u;

    /* Search for the settings with the lowest error.
     * mult is the MULT field of the I2C_F register, and ranges from 0-2. It selects the
     * multiplier factor for the divider. */
    for (mult = 0u; (mult <= 2u) && (bestError != 0); ++mult)
    {
        multiplier = 1u << mult;

        /* Scan table to find best match.*/
        for (i = 0u; i < ARRAY_SIZE(kI2CDividerTable); ++i)
        {
            computedRate = sourceClockInHz / (multiplier * kI2CDividerTable[i].sclDivider);
            absError = hz > computedRate ? hz - computedRate : computedRate - hz;

            if (absError < bestError)
            {
                bestMult = mult;
                bestIcr = kI2CDividerTable[i].icr;
                bestError = absError;

                /* If the error is 0, then we can stop searching
                 * because we won't find a better match.*/
                if (absError == 0)
                {
                    break;
                }
            }
        }
    }

    /* Set the resulting error.*/
    if (absoluteError_Hz)
    {
        *absoluteError_Hz = bestError;
    }

    /* Set frequency register based on best settings.*/
    I2C_Type *I2Cx = (I2C_Type*)I2CBases[instance];
    I2Cx->F = I2C_F_MULT(bestMult) | I2C_F_ICR(bestIcr);
}

static void I2C_Start(uint32_t instance)
{
    I2C_Type *I2Cx = (I2C_Type*)I2CBases[instance];
    if(I2Cx->C1 & I2C_C1_MST_MASK)
    {
        I2Cx->C1 |= I2C_C1_RSTA_MASK;
    }
    else
    {
        I2Cx->C1 |= I2C_C1_MST_MASK | I2C_C1_TX_MASK;
    }
  //  DelayUs(50);
}

static uint8_t I2C_SendByte(uint32_t instance, uint8_t data)
{
    I2C_Type *I2Cx = (I2C_Type*)I2CBases[instance];
    I2Cx->D =  data;
    while (!(I2C_S_REG(I2Cx) & I2C_S_IICIF_MASK)) {};
    I2C_S_REG(I2Cx) |= I2C_S_IICIF_MASK;
    return (I2C_S_REG(I2Cx) & I2C_S_RXAK_MASK)?(1):(0);
}

static uint8_t I2C_GetByte(uint32_t instance)
{
    I2C_Type *I2Cx = (I2C_Type*)I2CBases[instance];
    uint8_t data = I2Cx->D;
    while (!(I2C_S_REG(I2Cx) & I2C_S_IICIF_MASK)) {};
    I2C_S_REG(I2Cx) |= I2C_S_IICIF_MASK;

    return data;
}

static void I2C_Stop(uint32_t instance)
{
    I2C_Type *I2Cx = (I2C_Type*)I2CBases[instance];
    I2Cx->C1 &= ~I2C_C1_MST_MASK;
    while(I2C_S_REG(I2Cx) & I2C_S_BUSY_MASK);

}

static void I2C_SendAck(uint32_t instance)
{
    I2C_Type *I2Cx = (I2C_Type*)I2CBases[instance];
    I2Cx->C1 &= ~I2C_C1_TXAK_MASK;
}

static void I2C_SendNack(uint32_t instance)
{
    I2C_Type *I2Cx = (I2C_Type*)I2CBases[instance];
    I2Cx->C1 |= I2C_C1_TXAK_MASK;
}
/**
 * @brief  初始化配置I2C
 * @note   None
 * @param  MAP:引脚位图，详见I2C.h
 * @param  baudRate_Hz: 通信速度
 * @retval  0:I2C0模块；1:I2C1模块；2:I2C2模块
 */
uint32_t I2C_Init(uint32_t MAP, uint32_t baudrate)
{
    map_t * pq = (map_t*)&(MAP);
    I2C_Type *I2Cx = (I2C_Type*)I2CBases[pq->ip];
    
    REG_SET(I2CClkGate, pq->ip);

    I2C_HAL_SetBaudRate(pq->ip,  GetClock(kBusClock), baudrate/1000, NULL);
    I2C_S_REG(I2Cx) |= I2C_S_IICIF_MASK;
    I2Cx->C1 = I2C_C1_IICEN_MASK | I2C_C1_TX_MASK;
    
    PIN_SET_MUX;
    
    return pq->ip;
}

/**
 * @brief  I2C连续性读取多字节
 * @note   None
 * @param  instance:
 *         @arg HW_I2C0 : I2C0模块
 *         @arg HW_I2C1 : I2C1模块
 *         @arg HW_I2C1 : I2C1模块
 * @param  addr: 目标设备地址
 * @param  regAddr: 寄存器地址
 * @param  regLen: 寄存器长度
 * @param  buf: 准备发送的数据地址指针
 * @param  len: 数据长度
 * @retval CH_OK：成功；CH_ERR：失败
 */
uint32_t I2C_BurstRead(uint32_t instance, uint8_t addr, uint32_t regAddr, uint32_t regLen, uint8_t *buf, uint32_t len)
{
    int i;
    uint8_t *p;
    uint8_t err;
    I2C_Type *I2Cx = (I2C_Type*)I2CBases[instance];
    p = (uint8_t*)&regAddr;
    err = CH_OK;
    addr <<= 1;
    
    I2C_Start(instance);
    err += I2C_SendByte(instance, addr);
    
    while(regLen--)
    {
        err += I2C_SendByte(instance, *p++);
    }
    
    I2C_Start(instance);
    err += I2C_SendByte(instance, addr+1);
    I2Cx->C1 &= ~I2C_C1_TX_MASK;

    if(len == 1)
    {
        I2C_SendNack(instance);
    }
    else
    {
        I2C_SendAck(instance);
    }
    
    I2C_GetByte(instance);
    len --;
    
    for(i=len; i>=0; i--)
    {
        switch (i)
        {
            case 0x0U:
                I2C_Stop(instance);
                break;
            case 0x1U:
                I2C_SendNack(instance);
                break;
            default :
                I2C_SendAck(instance);
                break;
        }
        
        /* Read recently received byte into buffer and update buffer index */
        if (i==0)
        {
            *buf++ = I2Cx->D;
        }
        else
        {
            *buf++ = I2C_GetByte(instance);
        }
    }
    return err;
}

#endif /* I2C0 */

#else /* GPIO I2C */

/**
  ******************************************************************************
  * @note    GPIO 软件模仿 I2C
  ******************************************************************************
  */

#include "gpio.h"

typedef struct
{
    uint32_t instace;
    uint32_t sda;
    uint32_t scl;
}i2c_gpio;


static i2c_gpio i2c;

#define SDA_DDR_OUT()       GPIO_SetPinDir(i2c.instace, i2c.sda, 1)
#define SDA_DDR_IN()        GPIO_SetPinDir(i2c.instace, i2c.sda, 0)
#define SDA_H()             GPIO_PinWrite(i2c.instace, i2c.sda, 1)
#define SDA_L()             GPIO_PinWrite(i2c.instace, i2c.sda, 0)
#define SCL_H()             GPIO_PinWrite(i2c.instace, i2c.scl, 1)
#define SCL_L()             GPIO_PinWrite(i2c.instace, i2c.scl, 0)
#define SDA_IN()            GPIO_PinRead(i2c.instace, i2c.sda)
#define I2C_DELAY()         DelayUs(1);

uint32_t I2C_InitEx(uint32_t port, uint32_t sda_pin, uint32_t scl_pin, uint32_t baudrate)
{
    GPIO_Init(port, scl_pin, kGPIO_OPPH);
    GPIO_Init(port, sda_pin, kGPIO_OPPH);
    SetPinPull(port, sda_pin, 1);
    SetPinPull(port, scl_pin, 1);
    
    i2c.instace = port;
    i2c.scl = scl_pin;
    i2c.sda = sda_pin;
    return 0;
}

/**
 * @brief  初始化配置I2C
 * @note   None
 * @param  MAP:引脚位图，详见I2C.h
 * @param  baudRate_Hz: 通信速度
 * @retval 0:I2C0模块；1:I2C1模块；2:I2C2模块
 */
uint32_t I2C_Init(uint32_t MAP, uint32_t baudrate)
{
    uint8_t i;
    map_t * pq = (map_t*)&(MAP);
    
    /* push pull and pull up */
    for(i = 0; i < pq->pin_count; i++)
    {
        GPIO_Init(pq->io, pq->pin_start + i, kGPIO_OPPH);
        GPIO_Init(pq->io, pq->pin_start + i, kGPIO_OPPH);
    }

    /* i2c_gpio struct setup */
    i2c.instace = pq->io;
    
    switch(MAP)
    {
        case I2C1_SCL_PE01_SDA_PE00:
            i2c.scl = 1;i2c.sda = 0;
            break;
        case I2C0_SCL_PE19_SDA_PE18:
            i2c.scl = 19;i2c.sda = 18;
            break;
        case I2C0_SCL_PF22_SDA_PF23:
            i2c.scl = 22;i2c.sda = 23;
            break;
        case I2C0_SCL_PB00_SDA_PB01:
            i2c.scl = 0;i2c.sda = 1;
            break;
        case I2C0_SCL_PB02_SDA_PB03:
            i2c.scl = 2;i2c.sda = 3;
            break;
        case I2C1_SCL_PC10_SDA_PC11:
            i2c.scl = 10;i2c.sda = 11;
            break;
        case I2C0_SCL_PD08_SDA_PD09:
            i2c.scl = 8;i2c.sda = 9;
            break;
        case I2C0_SCL_PE24_SDA_PE25:
            i2c.scl = 24;i2c.sda = 25;
            break;
        case I2C1_SCL_PC01_SDA_PC02:
            i2c.scl = 1;i2c.sda = 2;
            break;
        case I2C0_SCL_PB03_SDA_PB04:
            i2c.scl = 3;i2c.sda = 4;
            break;
        case I2C1_SCL_PD07_SDA_PD06:
            i2c.scl = 7;i2c.sda = 6;
            break;
        case I2C0_SCL_PC06_SDA_PC07:
            i2c.scl = 6;i2c.sda = 7;
            break;
        case I2C2_SCL_PA11_SDA_PA10:
            i2c.scl = 11;i2c.sda = 10;
            break;
        default:
            LIB_TRACE("GPIO I2C: no PINMAP found!\r\n");
            break;
    }
    return pq->ip;
}

static bool I2C_Start(uint32_t instance)
{
    SDA_DDR_OUT();
    SDA_H();
    SCL_H();
    I2C_DELAY();
    SDA_L();
    I2C_DELAY();
    SCL_L();
    return true;
}

static void I2C_Stop(uint32_t instance)
{
    SCL_L();
    SDA_L();
    I2C_DELAY();
    SCL_H();
    SDA_H();
    I2C_DELAY();
}

static void I2C_Ack(uint32_t instance)
{
    SCL_L();
    SDA_L();
    I2C_DELAY();
    SCL_H();
    I2C_DELAY();
    SCL_L();
    I2C_DELAY();
}

static void I2C_NAck(uint32_t instance)
{
    SCL_L();
    I2C_DELAY();
    SDA_H();
    I2C_DELAY();
    SCL_H();
    I2C_DELAY();
    SCL_L();
    I2C_DELAY();
}

static bool I2C_WaitAck(uint32_t instance)
{
    uint8_t ack;
    SDA_DDR_IN();
    SCL_L();
    
    I2C_DELAY();
    SCL_H();
    I2C_DELAY();
    ack = SDA_IN();
    SCL_L();
    SDA_DDR_OUT();
    
    return ack;
}

static bool I2C_SendByte(uint32_t instance, uint8_t data)
{
    volatile uint8_t i;
    
    i = 8;
    while(i--)
    {
        if(data & 0x80) SDA_H();
        else SDA_L();
        data <<= 1;
        I2C_DELAY();
        SCL_H();
        I2C_DELAY();
        SCL_L();
    }
    return I2C_WaitAck(instance);
}

static uint8_t I2C_GetByte(uint32_t instance)
{
    uint8_t i, byte;
    
    i = 8;
    byte = 0;

    SDA_DDR_IN();
    while(i--)
    {
        SCL_L();
        I2C_DELAY();
        SCL_H();
        I2C_DELAY();
        byte = (byte<<1)|(SDA_IN() & 1);
    }
    SCL_L();
    SDA_DDR_OUT();
    return byte;
}

/**
 * @brief  I2C连续性读取多字节
 * @note   None
 * @param  instance:
 *         @arg HW_I2C0 : I2C0模块
 *         @arg HW_I2C1 : I2C1模块
 *         @arg HW_I2C1 : I2C1模块
 * @param  addr: 目标设备地址
 * @param  regAddr: 寄存器地址
 * @param  regLen: 寄存器长度
 * @param  buf: 准备发送的数据地址指针
 * @param  len: 数据长度
 * @retval 0：成功；其它：失败
 */
uint32_t I2C_BurstRead(uint32_t instance ,uint8_t addr, uint32_t regAddr, uint32_t regLen, uint8_t *buf, uint32_t len)
{
    uint8_t *p;
    uint8_t err;
    
    p = (uint8_t*)&regAddr;
    err = CH_OK;
    addr <<= 1;
    
    I2C_Start(instance);
    err += I2C_SendByte(instance, addr);
    
    while(regLen--)
    {
        err += I2C_SendByte(instance, *p++);
    }
    
    I2C_Start(instance);
    err += I2C_SendByte(instance, addr+1);
    
    while(len--)
    {
        *buf++ = I2C_GetByte(instance);
        if(len)
        {
            I2C_Ack(instance);
        }
    }
    
    I2C_NAck(instance);
    I2C_Stop(instance);
    
    return err;
}

/**
 * @brief  读取单个寄存器数值
 * @param  instance:
 *         @arg HW_I2C0 : I2C0模块
 *         @arg HW_I2C1 : I2C1模块
 *         @arg HW_I2C1 : I2C1模块
 * @param  addr: 目标设备地址
 * @param  regAddr: 寄存器地址
 * @param  buf: 读取的数据地址指针
 * @retval 0：成功；其它：错误
 */
uint32_t SCCB_ReadReg(uint32_t instance, uint8_t addr, uint8_t regAddr, uint8_t* buf)
{
    uint8_t err;
    uint8_t retry;
    
    retry = 10;
    addr <<= 1;
    
    while(retry--)
    {
        err = 0;
        I2C_Start(instance);
        err += I2C_SendByte(instance, addr);
        
        err += I2C_SendByte(instance, regAddr);;
        
        I2C_Stop(instance);
        I2C_Start(instance);
        err += I2C_SendByte(instance, addr+1);
        
        *buf = I2C_GetByte(instance);
        
        I2C_NAck(instance);
        I2C_Stop(instance);
        if(!err)
        {
            break;
        }
    }

    return err;
}

/**
 * @brief  写单个寄存器
 * @param  instance:
 *         @arg HW_I2C0 : I2C0模块
 *         @arg HW_I2C1 : I2C1模块
 *         @arg HW_I2C1 : I2C1模块
 * @param  addr: 目标设备地址
 * @param  regAddr: 寄存器地址
 * @param  data: 准备写入的数据
 * @retval 0：成功；其它：错误
 */
uint32_t SCCB_WriteReg(uint32_t instance, uint8_t addr, uint8_t regAddr, uint8_t data)
{
    uint8_t err;
    uint8_t retry;
    
    retry = 10;
    
    while(retry--)
    {
        err = I2C_WriteReg(instance, addr, regAddr, data);
        if(!err)
        {
            break;
        }
    }
    return err;
}

#endif


/**
  ******************************************************************************
  * @note   通用程序接口
  ******************************************************************************
  */


/**
 * @brief  探测I2C总线设备
 * @note   检测目标设备是否可用
 * @param  instance:
 *         @arg HW_I2C0 : I2C0模块
 *         @arg HW_I2C1 : I2C1模块
 *         @arg HW_I2C1 : I2C1模块
 * @param  addr: 目标设备地址
 * @retval 0：成功；其它：错误
 */
uint32_t I2C_Probe(uint32_t instance, uint8_t addr)
{
    uint8_t err;
    
    err = CH_OK;
    addr <<= 1;
    
    I2C_Start(instance);
    err = I2C_SendByte(instance, addr);
    I2C_Stop(instance);
    return err;
}

/**
 * @brief  I2C连续性写入多字节
 * @note   None
 * @param  instance:
 *         @arg HW_I2C0 : I2C0模块
 *         @arg HW_I2C1 : I2C1模块
 *         @arg HW_I2C1 : I2C1模块
 * @param  addr: 目标设备地址
 * @param  regAddr: 寄存器地址
 * @param  regLen: 寄存器长度
 * @param  buf: 准备发送的数据地址指针
 * @param  len: 数据长度
 * @retval 0：成功；其它：失败
 */
uint32_t I2C_BurstWrite(uint32_t instance ,uint8_t addr, uint32_t regAddr, uint32_t regLen, uint8_t *buf, uint32_t len)
{
    uint8_t *p;
    uint8_t err;
    
    p = (uint8_t*)&regAddr;
    err = CH_OK;
    addr <<= 1;
    
    I2C_Start(instance);
    err += I2C_SendByte(instance, addr);

    while(regLen--)
    {
        err += I2C_SendByte(instance, *p++);
    }
    
    while(len--)
    {
        err += I2C_SendByte(instance, *buf++);
    }

    I2C_Stop(instance);
    return err;
}

/**
 * @brief  读取单个寄存器数值
 * @param  instance:
 *         @arg HW_I2C0 : I2C0模块
 *         @arg HW_I2C1 : I2C1模块
 *         @arg HW_I2C1 : I2C1模块
 * @param  addr: 目标设备地址
 * @param  regAddr: 寄存器地址
 * @param  buf: 读取的数据地址指针
 * @retval 0：成功；其它：错误
 */
uint32_t I2C_ReadReg(uint32_t instance, uint8_t addr, uint8_t regAddr, uint8_t* buf)
{
    return I2C_BurstRead(instance, addr, regAddr, 1, buf, 1);
}

/**
 * @brief  写单个寄存器
 * @param  instance:
 *         @arg HW_I2C0 : I2C0模块
 *         @arg HW_I2C1 : I2C1模块
 *         @arg HW_I2C1 : I2C1模块
 * @param  addr: 目标设备地址
 * @param  regAddr: 寄存器地址
 * @param  data: 准备写入的数据
 * @retval 0：成功；其它：错误
 */
uint32_t I2C_WriteReg(uint32_t instance, uint8_t addr, uint8_t regAddr, uint8_t data)
{
    return I2C_BurstWrite(instance, addr, regAddr, 1, &data, 1);
}

/**
 * \brief I2C总线扫描测试
 * @param  instance:
 *         @arg HW_I2C0 : I2C0模块
 *         @arg HW_I2C1 : I2C1模块
 *         @arg HW_I2C1 : I2C1模块
 * @note   串口发送扫描到的设备ID
 */
void I2C_Scan(uint32_t instance)
{
    uint8_t i;
    uint8_t ret;
    for(i = 1; i < 127; i++)
    {
        ret = I2C_Probe(instance , i);
        if(!ret)
        {
            LIB_TRACE("ADDR:0x%02X(7BIT) | 0x%02X(8BIT) found!\r\n", i, i<<1);
        }
    }
}






