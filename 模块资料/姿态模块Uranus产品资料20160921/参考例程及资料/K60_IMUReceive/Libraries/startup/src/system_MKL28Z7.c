/*
** ###################################################################
**     Processors:          MKL28Z512VDC7
**                          MKL28Z512VLH7
**                          MKL28Z512VLL7
**                          MKL28Z512VMP7
**
**     Compilers:           Keil ARM C/C++ Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          GNU C Compiler - CodeSourcery Sourcery G++
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    KL28SINGLERM, Rev. 0, June 1, 2015
**     Version:             rev. 1.12, 2015-06-03
**     Build:               b150604
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright (c) 2015 Freescale Semiconductor, Inc.
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2014-12-29)
**         Initial version.
**     - rev. 1.1 (2015-03-19)
**         Make array for registers in XRDC, PCC and TRGMUX
**     - rev. 1.2 (2015-03-20)
**         Add vector table for intmux0
**         Add IRQS for SCG, RCM, DAC, TSI
**     - rev. 1.3 (2015-03-26)
**         Update FGPIO and GPIO
**         Update IRQ for LPIT
**     - rev. 1.4 (2015-04-08)
**         Group channel registers for LPIT
**     - rev. 1.5 (2015-04-15)
**         Group channel registers for INTMUX
**     - rev. 1.6 (2015-05-06)
**         Correct FOPT reset value
**         Correct vector table size
**         Remove parts with 256K flash size
**     - rev. 1.7 (2015-05-08)
**         Reduce register for XRDC
**     - rev. 1.8 (2015-05-12)
**         Add CMP
**         DMAMUX channel count to 8
**         Add PIDR for GPIO/FGPIO
**         Rename PIT to LPIT in PCC
**         SCG, USB register update
**         SIM, TRGMUX1, TRNG, TSTMR0/1 base address update
**         Add KEY related macros for WDOG
**     - rev. 1.9 (2015-05-16)
**         Add IRQS
**     - rev. 1.10 (2015-05-30)
**         Remove MTB1, MCM1 and MMDVSQ1
**     - rev. 1.11 (2015-06-01)
**         Remove registers for modules which are not available on single in PCC and TRGMUX
**     - rev. 1.12 (2015-06-03)
**         Bit 31 of TRGMUX changed to RW from RO
**
** ###################################################################
*/

/*!
 * @file MKL28Z7
 * @version 1.12
 * @date 2015-06-03
 * @brief Device specific configuration file for MKL28Z7 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "MKL28Z7.h"



/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

void SystemInit (void) {

#if (DISABLE_WDOG)
  WDOG0->CNT = WDOG_UPDATE_KEY;
  WDOG0->TOVAL = 0xFFFF;
  WDOG0->CS = (uint32_t) ((WDOG0->CS) & ~WDOG_CS_EN_MASK) | WDOG_CS_UPDATE_MASK;
#endif /* (DISABLE_WDOG) */


#ifdef CLOCK_SETUP
  if(PMC->REGSC &  PMC_REGSC_ACKISO_MASK) {
    PMC->REGSC |= PMC_REGSC_ACKISO_MASK; /* VLLSx recovery */
  }

  /* Power mode protection initialization */
#ifdef SYSTEM_SMC_PMPROT_VALUE
  SMC->PMPROT = SYSTEM_SMC_PMPROT_VALUE;
#endif
  
    SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV1(1) | SCG_SPLLDIV_SPLLDIV2(1) | SCG_SPLLDIV_SPLLDIV3(1);
    SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1) | SCG_SOSCDIV_SOSCDIV2(1) | SCG_SOSCDIV_SOSCDIV3(1);
    SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV1(1) | SCG_SIRCDIV_SIRCDIV2(1) | SCG_SIRCDIV_SIRCDIV3(1);
    SCG->FIRCDIV = SCG_FIRCDIV_FIRCDIV1(1) | SCG_FIRCDIV_FIRCDIV2(1) | SCG_FIRCDIV_FIRCDIV3(1);
  
#if (CLOCK_SETUP == 0)
    /* Configure FIRC */
    SCG_FIRCDIV = SCG_FIRCDIV_FIRCDIV3(1UL) | \
                  SCG_FIRCDIV_FIRCDIV2(0UL) | \
                  SCG_FIRCDIV_FIRCDIV1(0UL);                                    /* FIRCDIV3 clock = 48MHz */
    SCG_FIRCCFG = 0x0UL;
    SCG_FIRCTCFG = 0x0UL;
    while(SCG_FIRCCSR & SCG_FIRCCSR_LK_MASK);
    SCG_FIRCCSR |= SCG_FIRCCSR_FIRCEN_MASK|SCG_FIRCCSR_FIRCSTEN_MASK| SCG_FIRCCSR_FIRCLPEN_MASK ;
    while(!(SCG_FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK));                           /* Enable FIRC 48M */

    /* Configure RUN clock */
    SCG_RCCR = SCG_RCCR_SCS(3UL) | \
               SCG_RCCR_DIVCORE(0UL) | \
               SCG_RCCR_DIVSLOW(1UL);                                           /* CoreClock: 48M/1 = 48M; SlowClock: 48/2 = 24M */

    SCG_SIRCCSR &= SCG_SIRCCSR_SIRCEN_MASK;                                     /* Disable SIRC */
#elif (CLOCK_SETUP == 1)
    /* Configure SOSC */
    SCG_SOSCDIV = SCG_SOSCDIV_SOSCDIV3(0UL) | \
                  SCG_SOSCDIV_SOSCDIV2(0UL) | \
                  SCG_SOSCDIV_SOSCDIV1(0UL);
    SCG_SOSCCFG = (SCG_SOSCCFG & SCG_SOSCCFG_RANGE_MASK | SCG_SOSCCFG_RANGE(2)) | \
                  SCG_SOSCCFG_EREFS_MASK;
    SCG_SOSCCSR |= SCG_SOSCCSR_SOSCEN_MASK;
    while(!(SCG_SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK));                           /* Enable SOSC */

    /* Configure PLL */
    SCG_SPLLDIV = SCG_SPLLDIV_SPLLDIV3(1UL) | \
                  SCG_SPLLDIV_SPLLDIV2(0UL) | \
                  SCG_SPLLDIV_SPLLDIV1(0UL);
    SCG_SPLLCFG = SCG_SPLLCFG_MULT(2UL) | \
                  SCG_SPLLCFG_PREDIV(0UL) | \
                  SCG_SPLLCFG_SOURCE(0UL);                                      /* (8M/1)*18 /2 = 72M */
    SCG_SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;                                     /* Enable SPLL */
    while(!(SCG_SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK));

    /* Configure RUN clock */
    SCG_RCCR = SCG_RCCR_SCS(6UL) | \
               SCG_RCCR_DIVCORE(0UL) | \
               SCG_RCCR_DIVSLOW(2UL);                                           /* CoreClock: 72M/1 = 72M; SlowClock: 72/3 = 24M */

    SCG_SIRCCSR &= SCG_SIRCCSR_SIRCEN_MASK;                                     /* Disable SIRC */

#elif (CLOCK_SETUP == 3)

    /* open FIRC */
    SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN_MASK | SCG_FIRCCSR_FIRCLPEN_MASK | SCG_FIRCCSR_FIRCSTEN_MASK;
    while(0 == (SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK));   
        
    SCG->SPLLCSR = 0;
    SCG->SPLLCFG = SCG_SPLLCFG_SOURCE(1) | SCG_SPLLCFG_MULT(24-16) | SCG_SPLLCFG_PREDIV(5); /* (48/(5+1)*24)/2 = 96M */
    SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK | SCG_SPLLCSR_SPLLSTEN_MASK;
    while(0 == (SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK));
    
    SCG->RCCR = SCG_RCCR_SCS(6UL) | SCG_RCCR_DIVCORE(0UL) | SCG_RCCR_DIVSLOW(3UL);
#endif

#endif
    /* open FIRC anyway! */
    if(!(SCG->FIRCCSR & SCG_FIRCCSR_FIRCEN_MASK))
    {
        SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN_MASK | SCG_FIRCCSR_FIRCLPEN_MASK | SCG_FIRCCSR_FIRCSTEN_MASK;
        while(0 == (SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK));   
    }
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {

  uint32_t SCGOUTClock;                                 /* Variable to store output clock frequency of the SCG module */
  uint16_t Divider, prediv, multi;
  Divider = ((SCG->CSR & SCG_CSR_DIVCORE_MASK) >> SCG_CSR_DIVCORE_SHIFT) + 1;

  switch ((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT) {
    case 0x1:
      /* System OSC */
      SCGOUTClock = CPU_XTAL_CLK_HZ;
      break;
    case 0x2:
      /* Slow IRC */
      SCGOUTClock = (((SCG->SIRCCFG & SCG_SIRCCFG_RANGE_MASK) >> SCG_SIRCCFG_RANGE_SHIFT) ? 8000000 : 2000000);
      break;
    case 0x3:
      /* Fast IRC */
      SCGOUTClock = 48000000 + ((SCG->FIRCCFG & SCG_FIRCCFG_RANGE_MASK) >> SCG_FIRCCFG_RANGE_SHIFT) * 4000000;
      break;
    case 0x6:
      /* System PLL */
      if ((SCG->SPLLCFG & SCG_SPLLCFG_SOURCE_MASK) >> SCG_SPLLCFG_SOURCE_SHIFT) {
        SCGOUTClock = 48000000 + ((SCG->FIRCCFG & SCG_FIRCCFG_RANGE_MASK) >> SCG_FIRCCFG_RANGE_SHIFT) * 4000000;
      }
      else {
        SCGOUTClock = CPU_XTAL_CLK_HZ;
      }
      prediv = ((SCG->SPLLCFG & SCG_SPLLCFG_PREDIV_MASK) >> SCG_SPLLCFG_PREDIV_SHIFT) + 1;
      multi = ((SCG->SPLLCFG & SCG_SPLLCFG_MULT_MASK) >> SCG_SPLLCFG_MULT_SHIFT) + 16;
      SCGOUTClock = SCGOUTClock * multi / (prediv * 2);
      break;
    default:
      return;
  }

    SystemCoreClock = (SCGOUTClock / Divider);

}
