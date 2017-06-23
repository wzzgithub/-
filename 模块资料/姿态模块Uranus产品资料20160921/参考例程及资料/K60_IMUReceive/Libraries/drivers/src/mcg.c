/**
  ******************************************************************************
  * @file    mcg.c
  * @author  YANDLD
  * @version V3.0
  * @date    2016.06.12
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "common.h"
#include "mcg.h"

#if defined(MCG) && defined(MCG_C6_PLLS_MASK)

#if !defined(SIM_CLKDIV1_OUTDIV3)
    #define SIM_CLKDIV1_OUTDIV3(x)  (x & 0x00)
#endif

#if !defined(SIM_CLKDIV1_OUTDIV2)
    #define SIM_CLKDIV1_OUTDIV2(x)  (x & 0x00)
#endif

/**
 * @brief  时钟设置
 * @note   None
 * @param  opt:时钟选择
 *         @arg IRC_96M : 内部时钟96M
 *         @arg IRC_48M : 内部时钟48M
 *         @arg EX12M_120M : 外部12M晶振120M
 *         @arg EX08M_120M : 外部8M晶振120M
 *         @arg EX50M_120M : 外部50M晶振120M
 *         @arg EX48M_96M : 外部48M晶振96M
 *         @arg EX48M_120M : 外部48M晶振120M
 * @retval 0：成功；其它：失败
 */
uint32_t ClockSetup(uint32_t opt)
{
    static bool isInitialized = false;
    
    if(isInitialized == true)
    {
        return CH_ERR;
    }
    
    SIM->CLKDIV1 = 0xFFFFFFFF;
    if((opt == IRC_96M) || (opt == IRC_48M))
    {
        SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
        #if defined(SIM_SOPT1_OSC32KSEL_MASK)
        SIM->SOPT1 |= SIM_SOPT1_OSC32KSEL(0x03);
        #endif
        
        /* Switch to FEI Mode */
        MCG->C1 = MCG_C1_CLKS(0x00) | MCG_C1_FRDIV(0x00) | MCG_C1_IREFS_MASK | MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK;
        MCG->C4 &= ~(MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK);
        switch(opt)
        {
            case IRC_48M:
                MCG->C4 |= (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01));
                SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) | SIM_CLKDIV1_OUTDIV2(0x00) | SIM_CLKDIV1_OUTDIV3(0x00) | SIM_CLKDIV1_OUTDIV4(0x01);
                SystemCoreClock = 48*1000*1000;
                break;
            case IRC_96M:
                MCG->C4 |= (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03));
                SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) | SIM_CLKDIV1_OUTDIV2(0x01) | SIM_CLKDIV1_OUTDIV3(0x01) | SIM_CLKDIV1_OUTDIV4(0x03);
                SystemCoreClock = 96*1000*1000;
                break;
            default:
                LIB_TRACE("Unsupported ClockSetup");
                break;
        }
        
        /* Check that the source of the FLL reference clock is the internal reference clock. */
        while((MCG->S & MCG_S_IREFST_MASK) == 0u);
        MCG->C6 &= ~MCG_C6_PLLS_MASK;
        /* Wait until output of the FLL is selected */
        while((MCG->S & 0x0Cu) != 0x00u); 
    }
    else
    {
        uint8_t div_core, div_bus, div_flexbus, div_flash;
        /* FEI - FBE - PBE - PEE */
        SIM->SOPT2 |= (SIM_SOPT2_PLLFLLSEL_MASK & 0x01);
        
        /* config OSC */
        #if defined(OSC)
        OSC->CR = OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK;
        #endif
        SIM->SOPT7 = 0; /* select OSC0 as MCG input clock */  
        #if defined(MCG_C2_RANGE0_MASK)
        MCG->C2 = MCG_C2_RANGE0(3) | MCG_C2_EREFS0_MASK;
        #else
        MCG->C2 = MCG_C2_RANGE(3) | MCG_C2_EREFS_MASK;
        #endif
        MCG->C1 = MCG_C1_CLKS(0x02); /* OSC as output clk */
        while((MCG->S & MCG_S_OSCINIT0_MASK) == 0u);
        
        /* if there is a PLL diver, set it to 1 */
        #if defined(SIM_CLKDIV3_PLLFLLFRAC_MASK)
        SIM->CLKDIV3 = 0;
        #endif
        
        /* config PLL */
        switch(opt)
        {
            case EX50M_120M:
                MCG->C5 = (uint8_t)MCG_C5_PRDIV0(14);               
                MCG->C6 = (uint8_t)(0x40u | MCG_C6_VDIV0(36-24)); 
                SystemCoreClock = 120*1000*1000;      
                div_core = 0; div_bus = 1; div_flexbus = 1;  div_flash = 4;            
                break;
            case EX50M_180M:
                MCG->C5 = (uint8_t)MCG_C5_PRDIV0(14);               
                MCG->C6 = (uint8_t)(0x40u | MCG_C6_VDIV0(54-24)); 
                SystemCoreClock = 180*1000*1000;      
                div_core = 0; div_bus = 1; div_flexbus = 1;  div_flash = 6;            
                break;
            case EX48M_96M:
                MCG->C5 = (uint8_t)MCG_C5_PRDIV0(11);               
                MCG->C6 = (uint8_t)(0x40u | MCG_C6_VDIV0(24-24)); 
                SystemCoreClock = 96*1000*1000;  
                div_core = 0; div_bus = 1; div_flexbus = 1;  div_flash = 4;            
                break;
            case EX48M_120M:
                MCG->C5 = (uint8_t)MCG_C5_PRDIV0(11);               
                MCG->C6 = (uint8_t)(0x40u | MCG_C6_VDIV0(30-24)); 
                SystemCoreClock = 120*1000*1000;
                div_core = 0; div_bus = 1; div_flexbus = 1;  div_flash = 4;
                break;
            case EX12M_192M:
                MCG->C5 = (uint8_t)MCG_C5_PRDIV0(0);               
                MCG->C6 = (uint8_t)(0x40u | MCG_C6_VDIV0(32-16)); 
                SystemCoreClock = 192*1000*1000;
                div_core = 0; div_bus = 1; div_flexbus = 1;  div_flash = 7;
                
                break;
        }
 
        while((MCG->S & MCG_S_PLLST_MASK) == 0u);   
        while((MCG->S & MCG_S_LOCK0_MASK) == 0u);
        
        /* select PLL as MCGOutClock */
        MCG->C1 = MCG_C1_CLKS(0x00);
        while((MCG->S & 0x0Cu) != 0x0Cu);                   /* PLL is selected */

        SIM->CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(div_core)|SIM_CLKDIV1_OUTDIV2(div_bus)|SIM_CLKDIV1_OUTDIV3(div_flexbus)|SIM_CLKDIV1_OUTDIV4(div_flash));
    }
    
    isInitialized = true;
    return CH_OK;  
}

#endif



