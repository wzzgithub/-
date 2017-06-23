/**
  ******************************************************************************
  * @file    qspi.c
  * @author  YANDLD
  * @version V3.0
  * @date    2016.7.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include <string.h>
#include "qspi.h"
#include "common.h"

#define QSPI_AHB_BUFFER_REG(base, index) (*((uint32_t *)&(base->BUF0CR) + index))

#define FLASH_PAGE_SIZE         (256U)
#define FLASH_SECTOR_SIZE       (4096U)
#define FLASH_SIZE              (0x00400000U)

QSPI_FlashConfig_t single_config = {
    .flashA1Size = FLASH_SIZE, /* 4MB */
    .flashA2Size = 0,
    .flashB1Size = FLASH_SIZE,
    .flashB2Size = 0,
    .lookuptable =
        {    /* Seq0 :Single Read */
          //   [0] = 0x08180403, [1] = 0x00001C80,
             /* Seq0 :Quad Read */
             [0] = 0x0A1804EB, [1] = 0x1E800E06, [2] = 0x2400, 
              
             /* Seq1: Write Enable */
             /* CMD:      0x06 - Write Enable, Single pad */
             [4] = 0x406,

             /* Seq2: Erase All */
             /* CMD:    0x60 - Erase All chip, Single pad */
             [8] = 0x460,

             /* Seq3: Read Status */
             /* CMD:    0x05 - Read Status, single pad */
             /* READ:   0x01 - Read 1 byte */
             [12] = 0x1c010405,

             /* Seq4: Page Program */
             /* CMD:    0x02 - Page Program, Single pad */
             /* ADDR:   0x18 - 24bit address, Single pad */
             /* WRITE:  0x80 - Write 128 bytes at one pass, Single pad */
             [16] = 0x08180402, [17] = 0x2080,

             /* Seq5: Write Register */
             /* CMD:    0x01 - Write Status Register, single pad */
             /* WRITE:  0x01 - Write 1 byte of data, single pad */
             [20] = 0x20010401,

             /* Seq6: Read Config Register */
             /* CMD:  0x05 - Read Config register, single pad */
             /* READ: 0x01 - Read 1 byte */
             [24] = 0x1c010405,

             /* Seq7: Erase Sector */
             /* CMD:  0x20 - Sector Erase, single pad */
             /* ADDR: 0x18 - 24 bit address, single pad */
             [28] = 0x08180420,

             /* Seq8: Dummy */
             /* CMD:    0xFF - Dummy command, used to force SPI flash to exit continuous read mode */
             [32] = 0x4FF,

             /* Seq9: Fast Single read */
             /* CMD:        0x0B - Fast Read, Single Pad */
             /* ADDR:       0x18 - 24bit address, Single Pad */
             /* DUMMY:      0x08 - 8 clock cyles, Single Pad */
             /* READ:       0x80 - Read 128 bytes, Single Pad */
             /* JUMP_ON_CS: 0 */
             [36] = 0x0818040B, [37] = 0x1C800C08, [38] = 0x2400,

             /* Seq10: Fast Dual read */
             /* CMD:        0x3B - Dual Read, Single Pad */
             /* ADDR:       0x18 - 24bit address, Single Pad */
             /* DUMMY:      0x08 - 8 clock cyles, Single Pad */
             /* READ:       0x80 - Read 128 bytes, Dual pads */
             /* JUMP_ON_CS: 0 */
             [40] = 0x0818043B, [41] = 0x1D800C08, [42] = 0x2400,

             /* Match MISRA rule */
             [63] = 0},
    .dataHoldTime = 0,
    .CSHoldTime = 0,
    .CSSetupTime = 0,
    .cloumnspace = 0,
    .dataLearnValue = 0,
    .enableWordAddress = false};


void QSPI_SetEnable(bool val)
{
    (val)?(QuadSPI0->MCR &= ~QuadSPI_MCR_MDIS_MASK):(QuadSPI0->MCR |= QuadSPI_MCR_MDIS_MASK);
}

void QSPI_ConfigFlash(QSPI_FlashConfig_t *config)
{
    uint32_t address = FSL_FEATURE_QSPI_AMBA_BASE + config->flashA1Size;
    uint32_t val = 0;
    uint32_t i = 0;
    
    QSPI_SetEnable(false);
    
    /* Config the serial flash size */
    QuadSPI0->SFA1AD = address;
    address += config->flashA2Size;
    QuadSPI0->SFA2AD = address;
    address += config->flashB1Size;
    QuadSPI0->SFB1AD = address;
    address += config->flashB2Size;
    QuadSPI0->SFB2AD = address;

    /* Set Word Addressable feature */
    val = QuadSPI_SFACR_WA(config->enableWordAddress) | QuadSPI_SFACR_CAS(config->cloumnspace);
    QuadSPI0->SFACR = val;

    /* Config look up table */
    QuadSPI0->LUTKEY = 0x5AF05AF0U;
    QuadSPI0->LCKCR = 0x2U;
    for (i = 0; i < 64; i++)
    {
        QuadSPI0->LUT[i] = config->lookuptable[i];
    }
    QuadSPI0->LUTKEY = 0x5AF05AF0U;
    QuadSPI0->LCKCR = 0x1U;

    /* Config flash timing */
    val = QuadSPI_FLSHCR_TCSS(config->CSHoldTime) | QuadSPI_FLSHCR_TDH(config->dataHoldTime) |
          QuadSPI_FLSHCR_TCSH(config->CSSetupTime);
    QuadSPI0->FLSHCR = val;

    /* Set flash endianness */
    QuadSPI0->MCR &= ~QuadSPI_MCR_END_CFG_MASK;
    QuadSPI0->MCR |= QuadSPI_MCR_END_CFG(3);

    /* Enable QSPI again */
    QSPI_SetEnable(true);
}

void QSPI_ExecuteIPCommand(uint32_t instance, uint32_t index)
{
    while(QuadSPI0->SR & (QuadSPI_SR_BUSY_MASK | QuadSPI_SR_IP_ACC_MASK));
    
    /* clear seq */
    QuadSPI0->SPTRCLR = QuadSPI_SPTRCLR_IPPTRC_MASK | QuadSPI_SPTRCLR_BFPTRC_MASK;
    /* Write the seqid bit */
    QuadSPI0->IPCR = ((QuadSPI0->IPCR & (~QuadSPI_IPCR_SEQID_MASK)) | QuadSPI_IPCR_SEQID(index / 4U));
}

/* Write enable command */
void QSPI_CmdWriteEnable(void)
{
    while(QuadSPI0->SR & QuadSPI_SR_BUSY_MASK);
    QSPI_ExecuteIPCommand(0, 4U);
}

void QSPI_ClearRxFIFO(void)
{
    QuadSPI0->MCR |= QuadSPI_MCR_CLR_RXF_MASK;
}

void QSPI_ClearTxFIFO(void)
{
    QuadSPI0->MCR |= QuadSPI_MCR_CLR_TXF_MASK;
}

void QSPI_ResetAHBBuf(void)
{
    /*
    * Follow K8x/KL8x QSPI BG:
    * The MCR[MDIS] should therefore be set to 0 when the software reset bits are asserted.
    * Also, before they can be de-asserted again (by setting MCR[SWRSTHD] to 0), it is recommended
    * to set the MCR[MDIS] bit to 1. Once the software resets have been de-asserted, the normal
    * operation can be started by setting the MCR[MDIS] bit to 0.
    */
    QuadSPI0->MCR &= (uint32_t)~QuadSPI_MCR_MDIS_MASK;
    QuadSPI0->MCR |= QuadSPI_MCR_SWRSTHD_MASK | QuadSPI_MCR_SWRSTSD_MASK;
    // Wait several ticks until the AHB domain and serial flash domain are reset
    DelayUs(10);
    // Disable module during the reset procedure
    QuadSPI0->MCR |= QuadSPI_MCR_MDIS_MASK;
    // Clear the reset bits.
    QuadSPI0->MCR &= (uint32_t) ~(QuadSPI_MCR_SWRSTHD_MASK | QuadSPI_MCR_SWRSTSD_MASK);
    // Re-enable QSPI module
    QuadSPI0->MCR &= (uint32_t)~QuadSPI_MCR_MDIS_MASK;
}

/* Check if serial flash erase or program finished. */
void QSPI_IsSpiFlashReady(void)
{
    uint32_t val = 0;
    /* Check WIP bit */
    do
    {
        while(QuadSPI0->SR & QuadSPI_SR_BUSY_MASK);
        QSPI_ClearRxFIFO();
        QSPI_ExecuteIPCommand(0, 12U);
        while(QuadSPI0->SR & QuadSPI_SR_BUSY_MASK);
        val = *(volatile uint32_t *)(FSL_FEATURE_QSPI_ARDB_BASE);
//        LIB_TRACE("Flash Reg:0x%X\r\n", val & 0xFF);
        
        /* Clear ARDB area */
        //QSPI_ClearErrorFlag(EXAMPLE_QSPI, kQSPI_RxBufferDrain);
    } while (val & 0x1);
}

/*Erase sector */
uint32_t QSPI_CmdEraseSector(uint32_t addr)
{
    while(QuadSPI0->SR & QuadSPI_SR_BUSY_MASK);
    QuadSPI0->SFAR = addr;
    QSPI_CmdWriteEnable();
    QSPI_ExecuteIPCommand(0, 28U);
    QSPI_IsSpiFlashReady();
    QSPI_ResetAHBBuf();
    return CH_OK;
}

void QSPI_WriteBlocking(uint32_t *buf, uint32_t size)
{
    uint32_t i = 0;

    for (i = 0; i < size / 4U; i++)
    {
        /* Check if the buffer is full */
        while(QuadSPI0->SR & QuadSPI_SR_TXFULL_MASK);
        QuadSPI0->TBDR = *buf;
        buf++;
    }
}

#define FSL_FEATURE_QSPI_TXFIFO_DEPTH (16)

/* Program page into serial flash using QSPI polling way */
uint32_t QSPI_CmdProgramPage(uint32_t addr, uint32_t *buf)
{
    uint32_t leftLongWords = 0;

    while(QuadSPI0->SR & QuadSPI_SR_BUSY_MASK);
    QuadSPI0->SFAR = addr;
    QSPI_CmdWriteEnable();
    while(QuadSPI0->SR & QuadSPI_SR_BUSY_MASK);
    
    /* First write some data into TXFIFO to prevent from underrun */
    QSPI_WriteBlocking(buf, FSL_FEATURE_QSPI_TXFIFO_DEPTH * 4);
    buf += FSL_FEATURE_QSPI_TXFIFO_DEPTH;

    /* Start the program */
    QuadSPI0->IPCR = ((QuadSPI0->IPCR & (~QuadSPI_IPCR_IDATSZ_MASK)) | QuadSPI_IPCR_IDATSZ(FLASH_PAGE_SIZE));
    QSPI_ExecuteIPCommand(0, 16U);

    leftLongWords = FLASH_PAGE_SIZE - FSL_FEATURE_QSPI_TXFIFO_DEPTH * sizeof(uint32_t);
    QSPI_WriteBlocking(buf, leftLongWords);
    QSPI_IsSpiFlashReady();
    QSPI_ResetAHBBuf();
    return CH_OK;
}

/* Enable Quad mode */
uint32_t QSPI_EnableQuadMode(void)
{
    uint32_t val[4] = {0x40, 0, 0, 0};
    while(QuadSPI0->SR & QuadSPI_SR_BUSY_MASK);
    QuadSPI0->SFAR = FSL_FEATURE_QSPI_AMBA_BASE;
    QSPI_CmdWriteEnable();
    while(QuadSPI0->SR & QuadSPI_SR_BUSY_MASK);
    
    QSPI_WriteBlocking(val, 16);
    
    /* Set seq id, write register */
    QSPI_ExecuteIPCommand(0, 20);

    /* Wait until finished */
    QSPI_IsSpiFlashReady();
    return CH_OK;
}

uint32_t QSPI_CmdWriteSector(uint32_t addr, uint32_t *buf)
{
    for(int i=0; i<(FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE); i++)
    {
        QSPI_CmdProgramPage(addr, buf);
        addr += FLASH_PAGE_SIZE;
        buf += (FLASH_PAGE_SIZE/sizeof(uint32_t));
    }
    return CH_OK;
}

uint32_t QSPI_Init(uint32_t MAP, uint32_t baud)
{
    uint32_t val;
    volatile uint32_t i;
    
    SIM->SCGC2 |= SIM_SCGC2_QSPI_MASK;
    
    /* Reset AHB domain and buffer domian */
    QuadSPI0->MCR |= (QuadSPI_MCR_SWRSTHD_MASK | QuadSPI_MCR_SWRSTSD_MASK);
    for (i = 0; i < 100; i++)
    {
        __ASM("nop");
    }

    /* Disable QSPI module */
    QSPI_SetEnable(false);
    QuadSPI0->MCR &= ~(QuadSPI_MCR_SWRSTHD_MASK | QuadSPI_MCR_SWRSTSD_MASK);

    /* config clock */
    QuadSPI0->SOCCR = 0;
    QuadSPI0->MCR &= ~QuadSPI_MCR_SCLKCFG_MASK;
    QuadSPI0->MCR |= QuadSPI_MCR_SCLKCFG(GetClock(kCoreClock) / baud - 1U);
    LIB_TRACE("QSPI SCLKCFG:%d\r\n", (GetClock(kCoreClock) / baud - 1U));
    
    /* Set AHB buffer size and buffer master */
    for (i = 0; i < 4; i++)
    {
        val = QuadSPI_BUF0CR_MSTRID(0x0E) | QuadSPI_BUF0CR_ADATSZ(256 / 8U);
        QSPI_AHB_BUFFER_REG(QuadSPI0, i) = val;
    }
    
    QuadSPI0->BUF3CR |= QuadSPI_BUF3CR_ALLMST_MASK;

    /* Set watermark */
//    QuadSPI0->RBCT &= ~QuadSPI_RBCT_WMRK_MASK;
//    QuadSPI0->RBCT |= QuadSPI_RBCT_WMRK(8 - 1);
//    QuadSPI0->TBCT &= ~QuadSPI_TBCT_WMRK_MASK;
//    QuadSPI0->TBCT |= QuadSPI_TBCT_WMRK(8 - 1);

    /* Enable QSPI module */
    QSPI_SetEnable(true);
    return CH_OK;
}

uint32_t QSPI_LoadImage(uint32_t addr, uint32_t* image, uint32_t len)
{
    uint32_t ofs = addr;
    
    while(ofs < (len + addr))
    {
        QSPI_CmdEraseSector(ofs);
        QSPI_CmdWriteSector(ofs, (uint32_t*)(image));
        ofs += FLASH_SECTOR_SIZE;
        image += FLASH_SECTOR_SIZE/sizeof(uint32_t);
    }
}


uint32_t QSPI_Test(uint32_t addr, uint32_t size)
{
    uint32_t sector, j;
    static uint8_t buf[FLASH_SECTOR_SIZE];

    for(sector=0; sector<((size)/FLASH_SECTOR_SIZE); sector++)
    {
        LIB_TRACE("verify addr:0x%X(%dKB)...\r\n", addr + sector*FLASH_SECTOR_SIZE, (addr + sector*FLASH_SECTOR_SIZE)/1024);
        
        for(j=0; j<sizeof(buf); j++)
        {
            buf[j] = j % 0xFF;
        }
        
        QSPI_CmdEraseSector(addr + sector*FLASH_SECTOR_SIZE);
        
        /* write data */
        QSPI_CmdWriteSector(addr + sector*FLASH_SECTOR_SIZE, (uint32_t*)(buf));

        memset(buf, 0, FLASH_SECTOR_SIZE);
        memcpy(buf, (void*)(addr + sector*FLASH_SECTOR_SIZE), FLASH_SECTOR_SIZE);

        /* varify */
        for(j=0; j<sizeof(buf); j++)
        {
            if(buf[j] != (j%0xFF))
            {
                LIB_TRACE("index: %d error\r\n", j);
                return CH_ERR;
            }
        }
    }
    LIB_TRACE("test complete\r\n");
    return CH_OK;
}
