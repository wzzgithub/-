#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern u16 Tick_Cnt;	//�ȴ���������
extern u16 Watch_Cnt;	//ͨ�ż���

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
 
#endif
