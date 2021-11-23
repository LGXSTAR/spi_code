#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f10x.h"

#define LED_0_GPIO_PIN              GPIO_Pin_8
#define LED_0_GPIO_PORT             GPIOA
#define LED_0_GPIO_CLK              RCC_APB2Periph_GPIOA

#define LED_1_GPIO_PIN              GPIO_Pin_2
#define LED_1_GPIO_PORT             GPIOD
#define LED_1_GPIO_CLK              RCC_APB2Periph_GPIOD

#define    ON        1
#define    OFF       0


#define LED0     (LED_0_GPIO_PORT->ODR ^= LED_0_GPIO_PIN)
#define LED1     (LED_1_GPIO_PORT->ODR ^= LED_1_GPIO_PIN)

// \  C语言里面叫续行符，后面不能有任何的东西

#define   LED_0(a)   if(a) \
	                       GPIO_ResetBits(LED_0_GPIO_PORT, LED_0_GPIO_PIN); \
                     else  GPIO_SetBits(LED_0_GPIO_PORT, LED_0_GPIO_PIN);
#define   LED_1(a)   if(a) \
	                       GPIO_ResetBits(LED_1_GPIO_PORT, LED_1_GPIO_PIN); \
                     else  GPIO_SetBits(LED_1_GPIO_PORT, LED_1_GPIO_PIN);


#define LED_RED_ON          LED_0(ON)
#define LED_RED_OFF         LED_0(OFF)
#define LED_GREE_ON         LED_1(ON)
#define LED_GREE_OFF        LED_1(OFF)
#define LED_(b)             if(b) \
                                { LED_0(ON);LED_1(ON); } \
														else \
														{ LED_0(OFF);LED_1(OFF); }

#define LED_RED_TOGGLE      LED0
#define LED_GREE_TOGGLE     LED1


void LED_GPIO_Config(void);

#endif /* __BSP_LED_H */

