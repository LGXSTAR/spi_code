#include "flash_bsp.h"


static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;     

static uint32_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);


void FLASH_SPI_Init(void)
{
	SPI_GPIO_Config();
	SPI_Mode_Config();
}

/**************���Ͳ�����һ���ֽ�******************/
uint8_t SPI_FLASH_Send_Byte(uint8_t data)
{
	SPITimeout = SPIT_LONG_TIMEOUT;
	
	//��鲢�ȴ���TX������Ϊ��
	while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) ==0) return SPI_TIMEOUT_UserCallback(0);
	}
	
  //����ִ�е��˴���TX�������ѿ�
	SPI_I2S_SendData(SPIx,data);
	
	SPITimeout = SPIT_LONG_TIMEOUT;

	//��鲢�ȴ���RXNE������Ϊ�ǿ�
	while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) ==0) return SPI_TIMEOUT_UserCallback(0);
	}
	
	//����ִ�е��˴���˵�����ݷ�����ϣ������յ�һ���ֽ�
	return SPI_I2S_ReceiveData(SPIx);
}

//��ȡFLASH����
uint8_t SPI_FLASH_Read(void)
{
	return SPI_FLASH_Send_Byte(DUMMY);
}

//��ȡID��
uint32_t SPI_FLASH_Read_ID(void)
{
	uint32_t flash_ID;
	
	//FLASH SPIƬѡʹ��
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(READ_JEDEC_ID);
	
	flash_ID = SPI_FLASH_Send_Byte(DUMMY);
	
	flash_ID <<= 8;
	
	flash_ID |= SPI_FLASH_Send_Byte(DUMMY);
	
	flash_ID <<= 8;
	
	flash_ID |= SPI_FLASH_Send_Byte(DUMMY);
	
	FLASH_SPI_CS_HIGH;
	
	return flash_ID;
	
}

//SPIдʹ��
void SPI_Write_Enable(void)
{
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(WRITE_ENABLE);
	FLASH_SPI_CS_HIGH;
}


/********************����FLASHָ������**********************/
void FLASH_SPI_Sector_Erase(uint32_t addr)
{
	addr*=4096;
	SPI_Write_Enable();
	SPI_WaitForWriteEnd();
	//FLASH SPIƬѡʹ��
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(SECTOR_ERASE);
	
	SPI_FLASH_Send_Byte((addr>>16)&0xff);
	
	SPI_FLASH_Send_Byte((addr>>8)&0xff);
	
	SPI_FLASH_Send_Byte(addr&0xff);
	
	FLASH_SPI_CS_HIGH;
	
	SPI_WaitForWriteEnd();
}
/**************************END*****************************/


/*********************��ȡFLASH����************************/
void FLASH_SPI_Read_Data(uint32_t addr,uint8_t *readbuff,uint32_t numByteToRead)
{
	
	//FLASH SPIƬѡʹ��
	FLASH_SPI_CS_LOW;
	
	SPI_FLASH_Send_Byte(READ_DATA);
	
	SPI_FLASH_Send_Byte((addr>>16)&0xff);
	
	SPI_FLASH_Send_Byte((addr>>8)&0xff);
	
	SPI_FLASH_Send_Byte(addr&0xff);
	
	while(numByteToRead--)
	{
		*readbuff = SPI_FLASH_Send_Byte(DUMMY);
		readbuff++;
	}
	
	FLASH_SPI_CS_HIGH;
	
}

/***************************END***************************/


/***********************����д��FLASH*********************/
void FLASH_SPI_Write_Data(uint32_t addr,uint8_t *writebuff,uint32_t numByteToWrite)
{
	SPI_Write_Enable();
	SPI_WaitForWriteEnd();
	//FLASH SPIƬѡʹ��
	FLASH_SPI_CS_LOW;
	
	SPI_FLASH_Send_Byte(WRITE_DATA);
	
	SPI_FLASH_Send_Byte((uint8_t)((addr>>16)&0xff));
	
	SPI_FLASH_Send_Byte((uint8_t)((addr>>8)&0xff));
	
	SPI_FLASH_Send_Byte((uint8_t)(addr&0xff));
	
	while(numByteToWrite--)
	{
		SPI_FLASH_Send_Byte(*writebuff);
		writebuff++;
	}
	
	FLASH_SPI_CS_HIGH;
	
	SPI_WaitForWriteEnd();
}
/***************************END**************************/

//SPIҳд��
void SPI_Flash_Page_Write(uint32_t addr,uint8_t *writebuff,uint16_t numByteToWrite)
{
	SPI_Write_Enable();
	SPI_WaitForWriteEnd();
	//FLASH SPIƬѡʹ��
	FLASH_SPI_CS_LOW;
	
	SPI_FLASH_Send_Byte(WRITE_DATA);
	
	SPI_FLASH_Send_Byte((uint8_t)((addr>>16)&0xff));
	
	SPI_FLASH_Send_Byte((uint8_t)((addr>>8)&0xff));
	
	SPI_FLASH_Send_Byte((uint8_t)(addr&0xff));
	
	if(numByteToWrite > SPI_PerWritePageSize)
	{
		numByteToWrite = SPI_PerWritePageSize;
		FLASH_ERROR("SPI_Flash_Page_Write too large!");
	}
	
	while(numByteToWrite--)
	{
		SPI_FLASH_Send_Byte(*writebuff);
		writebuff++;
	}
	
	FLASH_SPI_CS_HIGH;
	
	SPI_WaitForWriteEnd();
}

//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		SPI_Flash_Page_Write(WriteAddr,pBuffer,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 

/******************************************SPI�ֽڶ�������*********************************************************/
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)  		   
u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=WriteAddr/4096;//������ַ 0~511 for w25x16
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		FLASH_SPI_Read_Data(secpos*4096,SPI_FLASH_BUF,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			FLASH_SPI_Sector_Erase(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 	 
}
/*************************************************END*************************************************************/


//�ȴ�FLASH�ڲ�ʱ��������
void SPI_WaitForWriteEnd(void)
{
	uint8_t status_reg = 0;
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(READ_STATUS);
	
	do
	{
		status_reg = SPI_FLASH_Send_Byte(DUMMY);
	}while((status_reg&0x01) == 1);
	
	FLASH_SPI_CS_HIGH;
}


/**
  * @brief  Basic management of the timeout situation.
  * @param  errorCode��������룬����������λ���ĸ����ڳ���.
  * @retval ����0����ʾSPI��ȡʧ��.
  */
static  uint32_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  FLASH_ERROR("SPI �ȴ���ʱ!errorCode = %d",errorCode);
  
  return 0;
}







