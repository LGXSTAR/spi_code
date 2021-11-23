#include "bsp_systick.h"

static __IO u32 TimingDelay;

#if 0
static __INLINE uint32_t SysTick_Config(uint32_t ticks)
{ 
  //不可能的重装载值，超出范围
  if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);            
   
  // 设置重装载寄存器	 
  SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;

  // 设置中断优先级
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 

  // 设置当前数值寄存器
  SysTick->VAL   = 0;  

  // 设置系统定时器的时钟源为 AHBCLK=72M
  // 使能系统定时器中断
  // 使能定时器
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk   | 
                   SysTick_CTRL_ENABLE_Msk;                    
  return (0);                                                  
}
#endif

void SysTick_Init(void)
{
	/* SystemFrequency / 1000 1ms 中断一次
	   SystemFrequency / 100000 10us 中断一次
     SystemFrequency / 1000000 1us 中断一次 */
	if(SysTick_Config(SystemCoreClock / 1000000))
	{
		while(1);
	}
}

void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}

void Delay_us(__IO u32 nTime)
{
	TimingDelay = nTime;
	// 使能滴答定时器  
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
	while (TimingDelay != 0);
}




