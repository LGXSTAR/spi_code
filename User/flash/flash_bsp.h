#ifndef __FLASH_BSP_H
#define __FLASH_BSP_H

#include "stm32f10x.h"
#include "spi_bsp.h"
#include "usart.h"

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

#define SPI_PerWritePageSize      256


#define DUMMY                     0x00
#define READ_JEDEC_ID             0x9f
#define SECTOR_ERASE              0x20
#define READ_STATUS               0x05
#define READ_DATA                 0x03
#define WRITE_ENABLE              0x06
#define WRITE_DATA                0x02

/*信息输出*/
#define FLASH_DEBUG_ON         0

#define FLASH_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define FLASH_DEBUG(fmt,arg...)          do{\
                                          if(FLASH_DEBUG_ON)\
                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

void FLASH_SPI_Init(void);
uint32_t SPI_FLASH_Read_ID(void);
void FLASH_SPI_Sector_Erase(uint32_t addr);
void SPI_WaitForWriteEnd(void);																					
void FLASH_SPI_Read_Data(uint32_t addr,uint8_t *readbuff,uint32_t numByteToRead);
void FLASH_SPI_Write_Data(uint32_t addr,uint8_t *writebuff,uint32_t numByteToWrite);
void SPI_Flash_Page_Write(uint32_t addr,uint8_t *writebuff,uint16_t numByteToWrite);
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
																					
#endif  /*FLASH_BSP_H*/

