
#include "bsp_iwdg.h"
/*                    ##### How to use this driver #####
 ===============================================================================
    [..]
      (#) Enable write access to IWDG_PR and IWDG_RLR registers using
          IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable) function
                 
      (#) Configure the IWDG prescaler using IWDG_SetPrescaler() function
              
      (#) Configure the IWDG counter value using IWDG_SetReload() function.
          This value will be loaded in the IWDG counter each time the counter
          is reloaded, then the IWDG will start counting down from this value.
              
      (#) Start the IWDG using IWDG_Enable() function, when the IWDG is used
          in software mode (no need to enable the LSI, it will be enabled
          by hardware)
               
      (#) Then the application program must reload the IWDG counter at regular
          intervals during normal operation to prevent an MCU reset, using
          IWDG_ReloadCounter() function.      
            
    @endverbatim   
	Min/max IWDG timeout period at 40 kHz (LSI)(1) (continued
  *******************************************************************************/
void IWDG_Init()
{
	//Enable write access to IWDG_PR and IWDG_RLR registers
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
	//Configure the IWDG prescaler
	IWDG_SetPrescaler(IWDG_Prescaler_16);  
	//Configure the IWDG counter value 
	IWDG_SetReload(2500);  	// Bits11:0 RL[11:0]: Watchdog counter reload value   ~ Only 12bit ~max value = 4096 
	IWDG_ReloadCounter();  
	IWDG_Enable();
}
