#include "spi_bsp.h"

/*****************SPI I/O����***************/
void SPI_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	/*ʹ���й�SPI��ʱ��*/
	SPI_GPIO_APBxClock_Fun(SPI_GPIO_CLK, ENABLE);
	SPI_APBxClock_Fun(SPI_CLK,ENABLE);
	
	/*SCK,MOSI,MISO���ų�ʼ��*/
	GPIO_InitStruct.GPIO_Pin = SPI_SCK_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(SPI_SCK_PORT, &GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Pin = SPI_MISO_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(SPI_MISO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = SPI_MOSI_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStruct);
	
	/*��ʼ��CS���š�ʹ��������ƣ�����ֱ�����ó��������ģʽ*/
	GPIO_InitStruct.GPIO_Pin = SPI_FLASH_CS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(SPI_FLASH_CS_PORT, &GPIO_InitStruct);	
	
	FLASH_SPI_CS_HIGH;
}

/***********SPI ����ģʽ����********************/
void SPI_Mode_Config(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	/*SPIʹ��ģʽ3*/
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CRCPolynomial = 0;                                 //��ʹ��CRC����,��ֵ���д
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;      //˫��ȫ˫��
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                      //��λ����
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPIx,&SPI_InitStructure);                                      //д�����õ��Ĵ���
	                        
	SPI_Cmd(SPIx,ENABLE);                                                   //ʹ��SPI
}






