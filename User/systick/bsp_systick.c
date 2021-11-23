#include "bsp_systick.h"

static __IO u32 TimingDelay;

#if 0
static __INLINE uint32_t SysTick_Config(uint32_t ticks)
{ 
  //�����ܵ���װ��ֵ��������Χ
  if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);            
   
  // ������װ�ؼĴ���	 
  SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;

  // �����ж����ȼ�
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 

  // ���õ�ǰ��ֵ�Ĵ���
  SysTick->VAL   = 0;  

  // ����ϵͳ��ʱ����ʱ��ԴΪ AHBCLK=72M
  // ʹ��ϵͳ��ʱ���ж�
  // ʹ�ܶ�ʱ��
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk   | 
                   SysTick_CTRL_ENABLE_Msk;                    
  return (0);                                                  
}
#endif

void SysTick_Init(void)
{
	/* SystemFrequency / 1000 1ms �ж�һ��
	   SystemFrequency / 100000 10us �ж�һ��
     SystemFrequency / 1000000 1us �ж�һ�� */
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
	// ʹ�ܵδ�ʱ��  
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
	while (TimingDelay != 0);
}




