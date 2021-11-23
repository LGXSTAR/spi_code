// bsp ：board support package 板级支持包
#include "bsp_led.h"

void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(LED_0_GPIO_CLK|LED_1_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = LED_0_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(LED_0_GPIO_PORT, &GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Pin = LED_1_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(LED_1_GPIO_PORT, &GPIO_InitStruct);	
	
}

