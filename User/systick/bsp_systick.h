#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H

#include "stm32f10x.h"

#define Delay_ms(x)     Delay_us(1000*x)



void SysTick_Init(void);

void Delay_us(__IO u32 nTime);

#endif
