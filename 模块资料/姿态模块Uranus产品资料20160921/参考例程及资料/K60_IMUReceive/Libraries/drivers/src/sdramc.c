/**
  ******************************************************************************
  * @file    sdramc.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.2.16
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "sdramc.h"
#include "common.h"

#if defined(SDRAM)


void SDRAMC_Init(SDRAMC_Init_t *Init)
{
    uint32_t fbReg;
    
    SIM->SOPT2 |= SIM_SOPT2_FBSL(3);
    SIM->SCGC7 |= SIM_SCGC7_FLEXBUS_MASK | SIM_SCGC7_SDRAMC_MASK;
    
    fbReg = FB->CSPMCR & ~FB_CSPMCR_GROUP2_MASK;
    FB->CSPMCR = fbReg | FB_CSPMCR_GROUP2(2);
    fbReg = FB->CSPMCR & ~FB_CSPMCR_GROUP3_MASK;
    FB->CSPMCR = fbReg | FB_CSPMCR_GROUP3(2);
    fbReg = FB->CSPMCR & ~FB_CSPMCR_GROUP4_MASK;
    FB->CSPMCR = fbReg | FB_CSPMCR_GROUP4(2);
    fbReg = FB->CSPMCR & ~FB_CSPMCR_GROUP5_MASK;
    FB->CSPMCR = fbReg | FB_CSPMCR_GROUP5(2);
    
    uint32_t count;

    /* Initialize sdram Auto refresh timing.
        for most SDRAM, the max data reflash time is 64ms( for one row), and one bank contain 4096 rows.
        so, the data reflash time for total chip is: 64ms/4096 = 15.625us.
    */
    count = ((GetClock(kBusClock)/1000000)*15.625);
    LIB_TRACE("busclock:%dHz, reflash 64ms need:%d cycle\r\n ", GetClock(kBusClock), count);
    count = count/16 - 1;
    LIB_TRACE("RC COUNT:0x%X\r\n", count);
    SDRAM->CTRL = SDRAM_CTRL_RC(count) | SDRAM_CTRL_RTIM(Init->refreshTime);

    /* Set the sdram block control. */
    SDRAM->BLOCK[0].AC = SDRAM_AC_PS(Init->portSize) | SDRAM_AC_CASL(Init->latency) | SDRAM_AC_CBM(Init->location) | (Init->baseAddress & SDRAM_AC_BA_MASK);
    SDRAM->BLOCK[0].CM = (Init->addressMask & SDRAM_CM_BAM_MASK) | SDRAM_CM_V_MASK;
}


uint32_t SDRAMC_SendCommand(uint32_t block, SDRAMC_Cmd_t command)
{
    uint32_t result = CH_OK;
    uint32_t count = 0xFFFF;

    switch (command)
    {
        /* Initiate mrs command. */
        case kSDRAMC_ImrsCommand:
            SDRAM->BLOCK[block].AC |= SDRAM_AC_IMRS_MASK;
            while (count--)
            {
                if (!(SDRAM->BLOCK[block].AC & SDRAM_AC_IMRS_MASK))
                {
                    break;
                }
            }

            if (!count)
            {
                /* Timeout the mrs command is unfinished. */
                result = CH_ERR;
            }
            break;
        /* Initiate precharge command. */
        case kSDRAMC_PrechargeCommand:
            SDRAM->BLOCK[block].AC |= SDRAM_AC_IP_MASK;
            while (count--)
            {
                if (!(SDRAM->BLOCK[block].AC & SDRAM_AC_IP_MASK))
                {
                    break;
                }
            }

            /* Timeout the precharge command is unfinished. */
            if (!count)
            {
                result = CH_ERR;
            }
            break;
        /* Enable Auto refresh command. */
        case kSDRAMC_AutoRefreshEnableCommand:
            SDRAM->BLOCK[block].AC |= SDRAM_AC_RE_MASK;
            break;
        /* Disable Auto refresh command. */
        case kSDRAMC_AutoRefreshDisableCommand:
            SDRAM->BLOCK[block].AC &= ~SDRAM_AC_RE_MASK;
            break;
        /* Enter self-refresh command. */
        case kSDRAMC_SelfrefreshEnterCommand:
            SDRAM->CTRL |= SDRAM_CTRL_IS_MASK;
            break;
        /* Exit self-refresh command. */
        case kSDRAMC_SelfrefreshExitCommand:
            SDRAM->CTRL &= ~SDRAM_CTRL_IS_MASK;
            break;
        default:
            break;
    }
    return result;
}

/* SDRAM device driver */

/*! @brief Define the physical connection--16bit port 8 column. */
/* SDRAM device         SDRAMC(Kinetis SDRAM pins ) see RM SDRAMC chapter */

#define SDRAM_A0        16
#define SDRAM_A1        15
#define SDRAM_A2        14
#define SDRAM_A3        13
#define SDRAM_A4        12
#define SDRAM_A5        11
#define SDRAM_A6        10
#define SDRAM_A7        9
#define SDRAM_A8        17
#define SDRAM_A9        18
#define SDRAM_A10       19
#define SDRAM_A11       20
#define SDRAM_A12       21
#define SDRAM_A13       22
#define SDRAM_A14       23

/*! @brief SDRAM Mode register write burst mode setting. */
typedef enum
{
    kSDRAM_MrsWriteBurst = 0x0U, /*!< Write burst mode. */
    kSDRAM_MrsWriteSingle        /*!< Write single location mode.  */
} sdram_write_burst_mode_t;

/*! @brief SDRAM Mode register operation mode setting. */
typedef enum
{
    kSDRAM_MrsStandOperation = 0x0U, /*!< Standard operation mode. */
    kSDRAM_MrsAllOtherReserve        /*!< All other states reserved.  */
} sdram_operation_mode_t;

/*! @brief SDRAM Mode register CAS latency setting. */
typedef enum
{
    kSDRAM_MrsLatencyOne = 0x1U, /*!< Latency one. */
    kSDRAM_MrsLatencyTwo,        /*!< Latency two. */
    kSDRAM_MrsLatencyThree       /*!< Latency three. */
} sdram_cas_latency_t;

/*! @brief SDRAM Mode register burst type setting. */
typedef enum
{
    kSDRAM_MrsSequential = 0x0U, /*!< Sequential. */
    kSDRAM_MrsInterleaved,       /*!< Interleaved. */
} sdram_burst_type_t;

/*! @brief SDRAM Mode register burst length setting. */
typedef enum
{
    kSDRAM_MrsBurstLenOne = 0x0U,    /*!< 1. */
    kSDRAM_MrsBurstLenTwo = 0x1U,    /*!< 2. */
    kSDRAM_MrsBurstLenFour = 0x2U,   /*!< 4. */
    kSDRAM_MrsBurstLenEight = 0x3U,  /*!< 8. */
    kSDRAM_MrsBurstLenAllPage = 0x7U /*!< Full page only for sequential burst type. */
} sdram_burst_len_t;



/* Defines the sdram nop delay times in initialize sequence for about 1us. */
#define SDRAM_DELAYTIMES_ONEMICROSECOND (125U)

static uint32_t SDRAM_InitSequence(uint32_t whichBlock,
                               sdram_burst_len_t burstLen,
                               sdram_burst_type_t burstType,
                               sdram_cas_latency_t casLatency,
                               sdram_operation_mode_t opMode,
                               sdram_write_burst_mode_t writeBurstMode)
{
    uint32_t count = SDRAM_DELAYTIMES_ONEMICROSECOND;
    uint8_t *mrsAddr = 0;
    uint32_t addr = 0;

    /* Issue a PALL command. */
    if (SDRAMC_SendCommand(whichBlock, kSDRAMC_PrechargeCommand) == CH_ERR)
    {
        return CH_ERR;
    }

    /* Accessing a SDRAM location. */
   // *(uint8_t *)(SDRAM_START_ADDRESS) = SDRAM_COMMAND_ACCESSVALUE;

    /* Enable the refresh. */
    SDRAMC_SendCommand(whichBlock, kSDRAMC_AutoRefreshEnableCommand);

    /* Wait for 8 refresh cycles less than one microsecond. */
    while (count != 0)
    {
        __NOP();
        count--;
    }

    /* Issue MSR command. */
    if (SDRAMC_SendCommand(whichBlock, kSDRAMC_ImrsCommand) == CH_ERR)
    {
        return CH_ERR;
    }
    /* Put the right value on SDRAM address bus for SDRAM mode register,
    *  The address of SDRAM Pins is as below:
    *  A2 ~ A0:   burst length   0
    *     000->1
    *     001->2
    *     010->4
    *     011->8
    *     res
    * A3:   burst type
    *        0 -> seq
    *        1 -> Interleave
    *
    * A6 ~ A4:  CAS latency  (should be set to equal to the tcasl in "sdram_latency_t")
    *       000-> res
    *       001-> 1
    *       010-> 2
    *       011-> 3
    *       res
    * A8 ~ A7:  Operationg Mode
    *       00->Stardard Operation
    *       res
    * A9:    Write Burst Mode
    *       0-> Programmed Burst Length
    *      1-> Single Location Access
    */
    /* A2-A0. */
    if (burstLen & 0x1)
    {
        addr |= 1 << SDRAM_A0;
    }
    if (burstLen & 0x2)
    {
        addr |= 1 << SDRAM_A1;
    }
    if (burstLen & 0x4)
    {
        addr |= 1 << SDRAM_A2;
    }

    /* Sdram address A3. */
    if (burstType & 0x1)
    {
        addr |= 1 << SDRAM_A3;
    }

    /* Sdram address A6-A4. */
    if (casLatency & 0x1)
    {
        addr |= 1 << SDRAM_A4;
    }
    if (casLatency & 0x2)
    {
        addr |= 1 << SDRAM_A5;
    }
    if (casLatency & 0x4)
    {
        addr |= 1 << SDRAM_A6;
    }

    /* Sdram address A8-A7. */
    if (opMode & 0x1)
    {
        addr |= 1 << SDRAM_A7;
    }
    if (opMode & 0x2)
    {
        addr |= 1 << SDRAM_A8;
    }

    /* Sdram address A9. */
    if (writeBurstMode & 0x1)
    {
        addr |= 1 << SDRAM_A9;
    }
    /* Set MRS register. */
    mrsAddr = (uint8_t *)(SDRAM_START_ADDRESS + addr);
    /* Access sdram location. */
    *mrsAddr = 0xFF;

    return CH_OK;
}

/**
 * @brief  ³õÊ¼»¯SDRAM
 * @retval None
 */
uint32_t SDRAM_Init(void)
{
    SDRAMC_Init_t Init;

    /* SDRAM controller configuration. */
    /* Port size: 16 bit, Command bit location: bit 19. */
    Init.portSize = kSDRAMC_PortSize16Bit;
    Init.location = kSDRAMC_Commandbit19;
    
    Init.latency = kSDRAMC_LatencyOne;
    Init.baseAddress = SDRAM_START_ADDRESS;
    Init.addressMask = kSDRAM_addressMask_8M;
    Init.refreshTime = kSDRAMC_RefreshSixClocks;
    /* SDRAM controller initialization. */
    SDRAMC_Init(&Init);

    /* SDRAM initialization sequence. */
    return SDRAM_InitSequence(0, kSDRAM_MrsBurstLenOne, kSDRAM_MrsSequential, kSDRAM_MrsLatencyOne, kSDRAM_MrsStandOperation, kSDRAM_MrsWriteBurst);
                       
}


#endif


