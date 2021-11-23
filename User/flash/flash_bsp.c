#include "flash_bsp.h"


static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;     

static uint32_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);


void FLASH_SPI_Init(void)
{
	SPI_GPIO_Config();
	SPI_Mode_Config();
}

/**************发送并接收一个字节******************/
uint8_t SPI_FLASH_Send_Byte(uint8_t data)
{
	SPITimeout = SPIT_LONG_TIMEOUT;
	
	//检查并等待至TX缓冲区为空
	while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) ==0) return SPI_TIMEOUT_UserCallback(0);
	}
	
  //程序执行到此处，TX缓冲区已空
	SPI_I2S_SendData(SPIx,data);
	
	SPITimeout = SPIT_LONG_TIMEOUT;

	//检查并等待至RXNE缓冲区为非空
	while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) ==0) return SPI_TIMEOUT_UserCallback(0);
	}
	
	//程序执行到此处，说明数据发送完毕，并接收到一个字节
	return SPI_I2S_ReceiveData(SPIx);
}

//读取FLASH数据
uint8_t SPI_FLASH_Read(void)
{
	return SPI_FLASH_Send_Byte(DUMMY);
}

//读取ID号
uint32_t SPI_FLASH_Read_ID(void)
{
	uint32_t flash_ID;
	
	//FLASH SPI片选使能
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

//SPI写使能
void SPI_Write_Enable(void)
{
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(WRITE_ENABLE);
	FLASH_SPI_CS_HIGH;
}


/********************擦除FLASH指定扇区**********************/
void FLASH_SPI_Sector_Erase(uint32_t addr)
{
	addr*=4096;
	SPI_Write_Enable();
	SPI_WaitForWriteEnd();
	//FLASH SPI片选使能
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(SECTOR_ERASE);
	
	SPI_FLASH_Send_Byte((addr>>16)&0xff);
	
	SPI_FLASH_Send_Byte((addr>>8)&0xff);
	
	SPI_FLASH_Send_Byte(addr&0xff);
	
	FLASH_SPI_CS_HIGH;
	
	SPI_WaitForWriteEnd();
}
/**************************END*****************************/


/*********************读取FLASH内容************************/
void FLASH_SPI_Read_Data(uint32_t addr,uint8_t *readbuff,uint32_t numByteToRead)
{
	
	//FLASH SPI片选使能
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


/***********************数据写入FLASH*********************/
void FLASH_SPI_Write_Data(uint32_t addr,uint8_t *writebuff,uint32_t numByteToWrite)
{
	SPI_Write_Enable();
	SPI_WaitForWriteEnd();
	//FLASH SPI片选使能
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

//SPI页写入
void SPI_Flash_Page_Write(uint32_t addr,uint8_t *writebuff,uint16_t numByteToWrite)
{
	SPI_Write_Enable();
	SPI_WaitForWriteEnd();
	//FLASH SPI片选使能
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

//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		SPI_Flash_Page_Write(WriteAddr,pBuffer,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 

/******************************************SPI字节对齐输入*********************************************************/
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)  		   
u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=WriteAddr/4096;//扇区地址 0~511 for w25x16
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		FLASH_SPI_Read_Data(secpos*4096,SPI_FLASH_BUF,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FLASH_SPI_Sector_Erase(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//写入整个扇区  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 	 
}
/*************************************************END*************************************************************/


//等待FLASH内部时序操作完成
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
  * @param  errorCode：错误代码，可以用来定位是哪个环节出错.
  * @retval 返回0，表示SPI读取失败.
  */
static  uint32_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  FLASH_ERROR("SPI 等待超时!errorCode = %d",errorCode);
  
  return 0;
}







