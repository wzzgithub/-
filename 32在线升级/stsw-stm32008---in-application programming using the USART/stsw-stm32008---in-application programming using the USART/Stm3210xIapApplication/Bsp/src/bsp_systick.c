#include "bsp_systick.h"

static volatile unsigned int TimingDelay;

//SysTick_Config(72);1MHZ

void Delay_us(volatile unsigned int nTime)
{ 
	TimingDelay = nTime;	
	while(TimingDelay != 0);
}
static void TimingDelay_Decrement(void)
{
	TimingDelay--;
}
void SysTick_Handler(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay_Decrement();
	}
	//TimingDelay_Decrement();	
	//TickCount ++;
}






