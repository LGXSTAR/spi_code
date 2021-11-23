#ifndef __SPI_BSP_H
#define __SPI_BSP_H

#include "stm32f10x.h"

/*******************************SPI参数定义****************************************/

#define SPIx                         SPI1
#define SPI_APBxClock_Fun            RCC_APB2PeriphClockCmd
#define SPI_CLK                      RCC_APB2Periph_SPI1
#define SPI_GPIO_APBxClock_Fun       RCC_APB2PeriphClockCmd
#define SPI_GPIO_CLK                 RCC_APB2Periph_GPIOA

#define SPI_SCK_PORT                 GPIOA
#define SPI_SCK_PIN                  GPIO_Pin_5
#define SPI_MISO_PORT                GPIOA
#define SPI_MISO_Pin                 GPIO_Pin_6
#define SPI_MOSI_PORT                GPIOA
#define SPI_MOSI_PIN                 GPIO_Pin_7

#define SPI_FLASH_CS_PORT            GPIOA
#define SPI_FLASH_CS_PIN             GPIO_Pin_2

/*片选引脚的拉高或拉低*/
#define FLASH_SPI_CS_LOW             GPIO_ResetBits( SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN )
#define FLASH_SPI_CS_HIGH            GPIO_SetBits( SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN )

void SPI_GPIO_Config(void);
void SPI_Mode_Config(void);

#endif  /*SPI_BSP_H*/

