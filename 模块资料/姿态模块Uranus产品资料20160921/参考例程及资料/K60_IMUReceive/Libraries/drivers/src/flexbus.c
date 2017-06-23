/**
  ******************************************************************************
  * @file    flexbus.c
  * @author  YANDLD
  * @version V3.0
  * @date    2016.2.20
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "flexbus.h"
#include "common.h"

#if (defined(FB))

/**
 * @brief  初始化FlexBus模块
 * @note   具体的配置应用详见关于FlexBus的使用例程  
 * @param[in]  FLEXBUS_InitStruct 指向FlexBus初始化配置结构体的指针，详见FlexBus.h
 * @retval None
 */
void FLEXBUS_Init(FLEXBUS_Init_t* Init)
{
    /* enable clock gate enable seruriy mode */
    SIM->SOPT2 |= SIM_SOPT2_FBSL(3);
    SIM->SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
     
    /* we must set V_MASK in CS0, because CS0.CSMR.V_MASK act as a global CS */
    FB->CS[0].CSMR |= FB_CSMR_V_MASK; 
    /* clear registers */
    FB->CS[Init->CSn].CSCR = 0;
    /* base address */
    FB->CS[Init->CSn].CSAR = Init->baseAddress;
    /* address space */
    FB->CS[Init->CSn].CSMR = FB_CSMR_BAM(Init->ADSpaceMask) | FB_CSMR_V_MASK;
    /* port size */
    FB->CS[Init->CSn].CSCR &= FB_CSCR_PS_MASK;
    FB->CS[Init->CSn].CSCR |= FB_CSCR_PS(Init->dataWidth);
    /* AutoAcknogement(AA) Config */
    if(Init->autoAckMode == kFLEXBUS_AutoAckEnable)
    {
        FB->CS[Init->CSn].CSCR |= FB_CSCR_AA_MASK;
    }
    else
    {
        FB->CS[Init->CSn].CSCR &= ~FB_CSCR_AA_MASK;
    }
    /* data align */
    if(Init->dataAlignMode == kFLEXBUS_DataLeftAligned)
    {
        FB->CS[Init->CSn].CSCR &= ~FB_CSCR_BLS_MASK;
    }
    else
    {
        FB->CS[Init->CSn].CSCR |= FB_CSCR_BLS_MASK;
    }
    /* byte enable mode */
    if(Init->ByteEnableMode == kFLEXBUS_BE_AssertedWrite)
    {
        FB->CS[Init->CSn].CSCR &= ~FB_CSCR_BEM_MASK;
    }
    else
    {
        FB->CS[Init->CSn].CSCR |= FB_CSCR_BEM_MASK;
    }
    
    /* gourp control */
    FB->CSPMCR = FB_CSPMCR_GROUP1(Init->PinGroup1 & 0x07) | FB_CSPMCR_GROUP2(Init->PinGroup2 & 0x07) | FB_CSPMCR_GROUP3(Init->PinGroup3 & 0x07) | FB_CSPMCR_GROUP4(Init->PinGroup4 & 0x07) | FB_CSPMCR_GROUP5(Init->PinGroup5 & 0x07);
}

/**
 * @brief  高级Flexbus 配置选项
 * @note   具体的配置应用详见关于FlexBus的使用例程
 * \param[in] CS 片选通道信号
 * @param[in] FLEXBUS_AdvancedConfigStruct
 * @retval None
 */
void FLEXBUS_TimingConfig(uint32_t CS, FLEXBUS_TimingConfig_t* config)
{
    /* Wait States
    Specifies the number of wait states inserted after FlexBus asserts the associated chip-select and before
    an internal transfer acknowledge is generated (WS = 00h inserts 0 wait states, ..., WS = 3Fh inserts 63
    wait states).
     */
    FB->CS[CS].CSCR &= ~FB_CSCR_WS_MASK;
    FB->CS[CS].CSCR |= FB_CSCR_WS(config->kFLEXBUS_WS);
    
    /* Address Setup
    Controls when the chip-select is asserted with respect to assertion of a valid address and attributes.
    */
    FB->CS[CS].CSCR &= ~FB_CSCR_ASET_MASK;
    FB->CS[CS].CSCR |= FB_CSCR_ASET(config->kFLEXBUS_ASET);
    
    /* Read Address Hold or Deselect
    Controls the address and attribute hold time after the termination during a read cycle that hits in the
    associated chip-select's address space.
    */
    FB->CS[CS].CSCR &= ~FB_CSCR_RDAH_MASK;
    FB->CS[CS].CSCR |= FB_CSCR_RDAH(config->kFLEXBUS_RDAH);
    
    /* Write Address Hold or Deselect
    Controls the address, data, and attribute hold time after the termination of a write cycle that hits in the
    associated chip-select's address space.
    */
    FB->CS[CS].CSCR &= ~FB_CSCR_WRAH_MASK;
    FB->CS[CS].CSCR |= FB_CSCR_WRAH(config->kFLEXBUS_WRAH);
    
    /* brust read enable */
    (config->kFLEXBUS_brustReadEnable)?
    (FB->CS[CS].CSCR |= FB_CSCR_BSTR_MASK):
    (FB->CS[CS].CSCR &= ~FB_CSCR_BSTR_MASK);
   
    /* brust write enable */
    (config->kFLEXBUS_brustWriteEnable)?
    (FB->CS[CS].CSCR |= FB_CSCR_BSTW_MASK):
    (FB->CS[CS].CSCR &= ~FB_CSCR_BSTW_MASK);
    
    /* brust write enable */
    (config->kFLEXBUS_SWSEN)?
    (FB->CS[CS].CSCR |= FB_CSCR_SWSEN_MASK):
    (FB->CS[CS].CSCR &= ~FB_CSCR_SWSEN_MASK);
    
    FB->CS[CS].CSCR &= ~FB_CSCR_SWS_MASK;
    FB->CS[CS].CSCR |= FB_CSCR_SWS(config->kFLEXBUS_SWS);
}

#endif

