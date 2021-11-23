#include "stm32f10x.h"   // �൱��51��Ƭ���е�  #include <reg51.h>
#include "bsp_led.h"
#include "usart.h"
#include "bsp_systick.h"
#include "flash_bsp.h"

//uint8_t readBuff[4096];
//uint8_t writeBuff[4096];

#define DOUBLE_ADDR     10
#define INT_ADDR        180

uint8_t cn_flag=0;
long double double_buffer[7];
int int_buffer[7];

long double double0_buffer[7];
int int0_buffer[7];

int main(void)
{
	uint32_t id,k;
	
	
	//���������ʱ��ϵͳ��ʱ���Ѿ������ó�72M��
	LED_GPIO_Config();
	USART_Config();
	SysTick_Init();
	FLASH_SPI_Init();
  
	FLASH_SPI_Read_Data(0,&cn_flag,1);
	
	printf("\r\n����һ��SPI_FLASH��д��������\r\n");
	
	id = SPI_FLASH_Read_ID();
	
	printf("id = 0x%x\r\n",id);
//	cn_flag=0x01;
//  FLASH_SPI_Sector_Erase(0);
//	SPI_Flash_Write(&cn_flag,0,1);
	
	if(cn_flag != 0xCD)
	{
		printf("\r\nû�м�⵽���ݱ�־��FLASHû�д洢���ݣ�������С��д��ʵ��\r\n");
		cn_flag=0xCD;
		FLASH_SPI_Sector_Erase(0);
		FLASH_SPI_Write_Data(0,&cn_flag,1);
		//����д������
		
		for(k=0;k<7;k++)
		{
			double_buffer[k]=k+0.1;
			int_buffer[k]=k*500+1;
		}
		
		/*д��С�����ݵ���ַ10*/
		SPI_Flash_Write((void*)double_buffer,DOUBLE_ADDR,sizeof(double_buffer));
		Delay_ms(2);
		/*д���������ݵ���ַ80*/
		SPI_Flash_Write((void*)int_buffer,INT_ADDR,sizeof(int_buffer));
		Delay_ms(2);
		printf("��оƬд�����ݣ�\r\n");
		
		for(k=0;k<7;k++)
		{
			printf("С��tx= %LF\r\n",double_buffer[k]);
			printf("����tx= %d\r\n",int_buffer[k]);
		}
		printf("\r\n�븴λ�����壬�Զ�ȡ���ݽ��м���\r\n");
	}
	else
	{
		printf("\r\n��⵽���ݱ�־\r\n");
		/*��ȡС������*/
		FLASH_SPI_Read_Data(DOUBLE_ADDR,(void*)double0_buffer,sizeof(double0_buffer));
		Delay_ms(2);
		/*��ȡ��������*/
		FLASH_SPI_Read_Data(INT_ADDR,(void*)int0_buffer,sizeof(int0_buffer));
		Delay_ms(2);
		
		printf("\r\n��оƬ��ȡ�������ݣ�\r\n");
		for(k=0;k<7;k++)
		{
			printf("С��tx= %LF\r\n",double0_buffer[k]);
			printf("����tx= %d\r\n",int0_buffer[k]);
		}
	}
	printf("double_buffer=%d,int_buffer=%d\r\n",sizeof(double_buffer),sizeof(int_buffer));
	
#if 0	
	for(i=0;i<580;i++)
	{
		writeBuff[i]=i+100;
	}
	
	FLASH_SPI_Write_Data(0,writeBuff,25);
	
	FLASH_SPI_Read_Data(0,readBuff,25);
	
	for(i=0;i<25;i++)
	{
		printf("%d ",readBuff[i]);
		if((i%10==0))printf("\r\n");
	}
	printf("\r\n");
#endif	

	while(1)
	{
		LED_RED_ON;LED_GREE_OFF;
		Delay_ms(1000);
		LED_RED_OFF;LED_GREE_ON;
		Delay_ms(1000);
	}
}

