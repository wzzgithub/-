#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern u16 Tick_Cnt;	//等待更新命令
extern u16 Watch_Cnt;	//通信监视

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
 
#endif
