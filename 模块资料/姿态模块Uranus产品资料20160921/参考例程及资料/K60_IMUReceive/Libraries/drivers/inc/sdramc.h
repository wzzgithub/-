/**
  ******************************************************************************
  * @file    sdramc.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.2.16
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#ifndef __CH_LIB_SDRAMC_H__
#define __CH_LIB_SDRAMC_H__

#include <stdint.h>
#include <stdbool.h>


#define SDRAM_START_ADDRESS (0x88000000)

/*! @brief SDRAM controller auto-refresh timing. */
typedef enum
{
    kSDRAMC_RefreshThreeClocks = 0x0U, /*!< The refresh timing with three bus clocks. */
    kSDRAMC_RefreshSixClocks,          /*!< The refresh timing with six bus clocks. */
    kSDRAMC_RefreshNineClocks          /*!< The refresh timing with nine bus clocks. */
} sdramc_refresh_time_t;

/*!
 * @brief Setting latency for SDRAM controller timing specifications.
 *
 * The latency setting will affects the following SDRAM timing specifications:
 *       - trcd: SRAS assertion to SCAS assertion \n
 *       - tcasl: SCAS assertion to data out \n
 *       - tras: ACTV command to Precharge command \n
 *       - trp: Precharge command to ACTV command \n
 *       - trwl, trdl: Last data input to Precharge command \n
 *       - tep: Last data out to Precharge command \n
 * the details of the latency setting and timing specifications are shown on the following table list: \n
 *   latency      trcd:          tcasl         tras           trp        trwl,trdl        tep   \n
 *    0       1 bus clock    1 bus clock   2 bus clocks   1 bus clock   1 bus clock   1 bus clock  \n
 *    1       2 bus clock    2 bus clock   4 bus clocks   2 bus clock   1 bus clock   1 bus clock  \n
 *    2       3 bus clock    3 bus clock   6 bus clocks   3 bus clock   1 bus clock   1 bus clock  \n
 *    3       3 bus clock    3 bus clock   6 bus clocks   3 bus clock   1 bus clock   1 bus clock  \n
 */
typedef enum
{
    kSDRAMC_LatencyZero = 0x0U, /*!< Latency  0. */
    kSDRAMC_LatencyOne,         /*!< Latency  1. */
    kSDRAMC_LatencyTwo,         /*!< Latency  2. */
    kSDRAMC_LatencyThree,       /*!< Latency  3. */
} sdramc_latency_t;

/*! @brief SDRAM controller command bit location. */
typedef enum
{
    kSDRAMC_Commandbit17 = 0x0U, /*!< Command bit location is bit 17. */
    kSDRAMC_Commandbit18,        /*!< Command bit location is bit 18. */
    kSDRAMC_Commandbit19,        /*!< Command bit location is bit 19. */
    kSDRAMC_Commandbit20,        /*!< Command bit location is bit 20. */
    kSDRAMC_Commandbit21,        /*!< Command bit location is bit 21. */
    kSDRAMC_Commandbit22,        /*!< Command bit location is bit 22. */
    kSDRAMC_Commandbit23,        /*!< Command bit location is bit 23. */
    kSDRAMC_Commandbit24         /*!< Command bit location is bit 24. */
} sdramc_command_bit_location_t;

/*! @brief SDRAM controller command. */
typedef enum
{
    kSDRAMC_ImrsCommand = 0x0U,        /*!< Initiate MRS command. */
    kSDRAMC_PrechargeCommand,          /*!< Initiate precharge command.  */
    kSDRAMC_SelfrefreshEnterCommand,   /*!< Enter self-refresh command. */
    kSDRAMC_SelfrefreshExitCommand,    /*!< Exit self-refresh command. */
    kSDRAMC_AutoRefreshEnableCommand,  /*!< Enable Auto refresh command. */
    kSDRAMC_AutoRefreshDisableCommand, /*!< Disable Auto refresh command.  */
} SDRAMC_Cmd_t;

/*! @brief SDRAM port size. */
typedef enum
{
    kSDRAMC_PortSize32Bit = 0x0U, /*!< 32-Bit port size. */
    kSDRAMC_PortSize8Bit,         /*!< 8-Bit port size. */
    kSDRAMC_PortSize16Bit         /*!< 16-Bit port size. */
} sdramc_port_size_t;

enum
{
    kSDRAM_addressMask_512K =   0x00040000,             /* size in byte */
    kSDRAM_addressMask_1M =     0x000C0000,
    kSDRAM_addressMask_2M =     0x001C0000,
    kSDRAM_addressMask_4M =     0x003C0000,
    kSDRAM_addressMask_8M =     0x007C0000,
    kSDRAM_addressMask_16M =    0x00FC0000,
    kSDRAM_addressMask_32M =    0x01FC0000,
};

/*!
 * @brief SDRAM controller configuration structure.
 *
 * Defines a configure structure and uses the SDRAMC_Configure() function to make necessary
 * initializations.
 */
typedef struct
{
    sdramc_port_size_t              portSize;                   /*!< The port size of the associated SDRAM block. */
    sdramc_command_bit_location_t   location;           /*!< The command bit location. */
    sdramc_latency_t                latency;                    /*!< The latency for some timing specifications. */       
    uint32_t                        baseAddress;                /*!< The base address of the SDRAM block. */
    uint32_t                        addressMask;                /*!< The base address mask of the SDRAM block. */
    sdramc_refresh_time_t           refreshTime;                /*!< Trc:The number of bus clocks inserted between a REF and next ACTIVE command. */
} SDRAMC_Init_t;

/* API  */
void SDRAMC_Init(SDRAMC_Init_t *Init);
uint32_t SDRAMC_SendCommand(uint32_t block, SDRAMC_Cmd_t command);

/* SDRAM API */
uint32_t SDRAM_Init(void);



#endif

