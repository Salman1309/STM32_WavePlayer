#include "stm32f10x.h"
#include "sd.h"
#include "SPI_SD.h"
#include "User_systick.h"
							   
uint8_t  SD_Type=0;
						   


uint8_t SD_GetResponse(uint8_t Response)
{
	uint16_t Count=0xFFF; 						  
	while ((SPI0_communication(0XFF)!=Response)&&Count)Count--;  
	if (Count==0)return MSD_RESPONSE_FAILURE;  
	else return MSD_RESPONSE_NO_ERROR;
}

										  
uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t r1;	
	uint8_t repeat=0;	         
	SD_CS_High;
    SPI0_communication(0xff);//高速写命令延时
	SPI0_communication(0xff);     
 	SPI0_communication(0xff);  	 
    //片选端置低，选中SD卡
    SD_CS_Low; 
    //发送
    SPI0_communication(cmd | 0x40);//分别写入命令
    SPI0_communication(arg >> 24);
    SPI0_communication(arg >> 16);
    SPI0_communication(arg >> 8);
    SPI0_communication(arg);
    SPI0_communication(crc); 
    //等待响应，或超时退出
    while((r1=SPI0_communication(0xFF))==0xFF)
    {
        repeat++;	    
        if(repeat>200)break; 
    }   
    //关闭片选
    SD_CS_High;
    //在总线上额外增加8个时钟，让SD卡完成剩下的工作
    SPI0_communication(0xFF);
    //返回状态值
    return r1;
}		  																				 
												  
uint8_t SD_SendCommand_NoDeassert(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t repeat=0;	         
	uint8_t r1;			   
    SPI0_communication(0xff);//高速写命令延时
	SPI0_communication(0xff);  	 	 
    SD_CS_Low;//片选端置低，选中SD卡	   
    //发送
    SPI0_communication(cmd | 0x40); //分别写入命令
    SPI0_communication(arg >> 24);
    SPI0_communication(arg >> 16);
    SPI0_communication(arg >> 8);
    SPI0_communication(arg);
    SPI0_communication(crc);   
    //等待响应，或超时退出
    while((r1=SPI0_communication(0xFF))==0xFF)
    {
        repeat++;	    
        if(repeat>200)break; 
    }  	  
    //返回响应值
    return r1;
}
									 
uint8_t SD_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	uint8_t r1,i;      
  uint16_t repeat=0;  
  uint8_t buff[6];

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);

	/*-------  CS Configuartion ----------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	SD_CS_High;	// SD_CS = 1; 
	
//#ifndef SSP0INIT 		   
	SPI0_Init_LowSpeed();
//#define SSP0INIT		   
//#endif

	SD_CS_High;
	

    for(i=0;i<12;i++)SPI0_communication(0xFF);
	do
    {	   
        i = SD_SendCommand(CMD0, 0, 0x95);
        repeat++;
    }while((i!=0x01)&&(repeat<200)); 
    if(repeat==200)return 1;
	repeat = 0;				 
	  
	
 	SD_CS_Low;	
	r1 = SD_SendCommand_NoDeassert(8, 0x1aa,0x87);	     
    //如果卡片版本信息是v1.0版本的，即r1=0x05，则进行以下初始化
    if(r1 == 0x05)
    {
        //设置卡类型为SDV1.0，如果后面检测到为MMC卡，再修改为MMC
        SD_Type = SD_TYPE_V1;	   
        //如果是V1.0卡，CMD8指令后没有后续数据
        //片选置高，结束本次命令
        SD_CS_High;
        //多发8个CLK，让SD结束后续操作
        SPI0_communication(0xFF);	  

        repeat = 0;
        do
        {
            //先发CMD55，应返回0x01；否则出错
            r1 = SD_SendCommand(CMD55, 0, 0);
            if(r1 == 0XFF)return r1;//只要不是0xff,就接着发送	  
            //得到正确响应后，发ACMD41，应得到返回值0x00，否则重试200次
            r1 = SD_SendCommand(ACMD41, 0, 0);
            repeat++;
        }while((r1!=0x00) && (repeat<400));
        // 判断是超时还是得到正确回应
        // 若有回应：是SD卡；没有回应：是MMC卡	  
        //----------MMC卡额外初始化操作开始------------
        if(repeat==400)
        {
            repeat = 0;
            //发送MMC卡初始化命令（没有测试）
            do
            {
                r1 = SD_SendCommand(1,0,0);
                repeat++;
            }while((r1!=0x00)&& (repeat<400));
            if(repeat==400)return 1;   //MMC卡初始化超时		    
            //写入卡类型
            SD_Type = SD_TYPE_MMC;
        }
        //----------MMC卡额外初始化操作结束------------	    
        //设置SPI为高速模式
        //LPC_SSP0->CPSR = 0x04;  //设置SPI0为高速模式   
				SPI0_Init_HighSpeed();
		SPI0_communication(0xFF);	 
        //禁止CRC校验	   
		r1 = SD_SendCommand(CMD59, 0, 0x95);
        if(r1 != 0x00)return r1;  //命令错误，返回r1   	   
        //设置Sector Size
        r1 = SD_SendCommand(CMD16, 512, 0x95);
        if(r1 != 0x00)return r1;//命令错误，返回r1		 
        //-----------------SD卡、MMC卡初始化结束-----------------

    }//SD卡为V1.0版本的初始化结束	 
    //下面是V2.0卡的初始化
    //其中需要读取OCR数据，判断是SD2.0还是SD2.0HC卡
    else if(r1 == 0x01)
    {
        //V2.0的卡，CMD8命令后会传回4字节的数据，要跳过再结束本命令
        buff[0] = SPI0_communication(0xFF);  //should be 0x00
        buff[1] = SPI0_communication(0xFF);  //should be 0x00
        buff[2] = SPI0_communication(0xFF);  //should be 0x01
        buff[3] = SPI0_communication(0xFF);  //should be 0xAA	    
        SD_CS_High;	  
        SPI0_communication(0xFF);//the next 8 clocks			 
        //判断该卡是否支持2.7V-3.6V的电压范围
        //if(buff[2]==0x01 && buff[3]==0xAA) //不判断，让其支持的卡更多
        {	  
            repeat = 0;
            //发卡初始化指令CMD55+ACMD41
    		do
    		{
    			r1 = SD_SendCommand(CMD55, 0, 0);
    			if(r1!=0x01)return r1;	   
    			r1 = SD_SendCommand(ACMD41, 0x40000000, 0);
                if(repeat>200)return r1;  //超时则返回r1状态  
            }while(r1!=0);		  
            //初始化指令发送完成，接下来获取OCR信息		   
            //-----------鉴别SD2.0卡版本开始-----------
            r1 = SD_SendCommand_NoDeassert(CMD58, 0, 0);
            if(r1!=0x00)
			{
				SD_CS_High;//释放SD片选信号
				return r1;  //如果命令没有返回正确应答，直接退出，返回应答	 
			}//读OCR指令发出后，紧接着是4字节的OCR信息
            buff[0] = SPI0_communication(0xFF);
            buff[1] = SPI0_communication(0xFF); 
            buff[2] = SPI0_communication(0xFF);
            buff[3] = SPI0_communication(0xFF);		 
            //OCR接收完成，片选置高
            SD_CS_High;
            SPI0_communication(0xFF);	   
            //检查接收到的OCR中的bit30位（CCS），确定其为SD2.0还是SDHC
            //如果CCS=1：SDHC   CCS=0：SD2.0
            if(buff[0]&0x40)SD_Type = SD_TYPE_V2HC;    //检查CCS	 
            else SD_Type = SD_TYPE_V2;	    
            //-----------鉴别SD2.0卡版本结束----------- 
            
            //LPC_SSP0->CPSR = 0x04;  //设置SPI0为高速模式
						SPI0_Init_HighSpeed();
        }	    
    }
    return r1;
}	 																			   
//从SD卡中读回指定长度的数据，放置在给定位置
//输入: uint8_t *data(存放读回数据的内存>len)
//      uint16_t len(数据长度）
//      uint8_t release(传输完成后是否释放总线CS置高 0：不释放 1：释放）	 
//返回值:0：NO_ERR
//  	 other：错误信息														  
uint8_t SD_ReceiveData(uint8_t *data, uint16_t len, uint8_t release)
{
    // 启动一次传输
    SD_CS_Low;				  	  
	if(SD_GetResponse(0xFE))//等待SD卡发回数据起始令牌0xFE
	{	  
		SD_CS_High;
		return 1;
	}
    while(len--)//开始接收数据
    {
        *data=SPI0_communication(0xFF);
        data++;
    }
    //下面是2个伪CRC（dummy CRC）
    SPI0_communication(0xFF);
    SPI0_communication(0xFF);
    if(release==RELEASE)//按需释放总线，将CS置高
    {
        SD_CS_High;//传输结束
        SPI0_communication(0xFF);
    }											  					    
    return 0;
}																				  
//获取SD卡的CID信息，包括制造商信息
//输入: uint8_t *cid_data(存放CID的内存，至少16Byte）	  
//返回值:0：NO_ERR
//		 1：TIME_OUT
//       other：错误信息														   
uint8_t SD_GetCID(uint8_t *cid_data)
{
    uint8_t r1;	   
    //发CMD10命令，读CID
    r1 = SD_SendCommand(CMD10,0,0xFF);
    if(r1 != 0x00)return r1;  //没返回正确应答，则退出，报错  
    SD_ReceiveData(cid_data,16,RELEASE);//接收16个字节的数据	 
    return 0;
}																				  
//获取SD卡的CSD信息，包括容量和速度信息
//输入:uint8_t *cid_data(存放CID的内存，至少16Byte）	    
//返回值:0：NO_ERR
//       1：TIME_OUT
//       other：错误信息														   
uint8_t SD_GetCSD(uint8_t *csd_data)
{
    uint8_t r1;	 
    r1=SD_SendCommand(CMD9,0,0xFF);//发CMD9命令，读CSD
    if(r1)return r1;  //没返回正确应答，则退出，报错  
    SD_ReceiveData(csd_data, 16, RELEASE);//接收16个字节的数据 
    return 0;
}  
//获取SD卡的容量（字节）   
//返回值:0： 取容量出错 
//       其他:SD卡的容量(字节)														  
uint32_t SD_GetCapacity(void)
{
    uint8_t csd[16];
    uint32_t Capacity;
    uint16_t n;
	uint16_t csize;  					    
	//取CSD信息，如果期间出错，返回0
    if(SD_GetCSD(csd)!=0) return 0;	    
    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0)==0x40)
    {									  
	    Capacity = ((uint32_t)csd[8])<<8;
		Capacity += (uint32_t)csd[9]+1;	 
        Capacity = (Capacity)*1024;//得到扇区数
		Capacity *= 512;//得到字节数			   
    }
    else
    {		    
		n = (csd[5] & 0x0F) + ((csd[10] & 0x80) >> 7) + ((csd[9] & 0x03) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 0x03) << 10) + 1;
    	Capacity = (uint32_t)csize << (n - 9);	
		Capacity *= 512;  
    }
    return (uint32_t)Capacity;
}	    																			    
													  
uint8_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buffer)
{
	uint8_t r1;	    

	//LPC_SSP0->CPSR = 0x04;  //设置SPI为高速模式24MHz  
	SPI0_Init_HighSpeed();	
    //如果不是SDHC，给定的是sector地址，将其转换成byte地址
    if(SD_Type!=SD_TYPE_V2HC)
    {
        sector = sector<<9;
    } 
	r1 = SD_SendCommand(CMD17, sector, 0);//读命令	 		    
	if(r1 != 0x00)return r1; 		   							  
	r1 = SD_ReceiveData(buffer, 512, RELEASE);		 
	if(r1 != 0)return r1;   //读数据出错！
    else return 0; 
}

													  
uint8_t SD_WriteSingleBlock(uint32_t sector, const uint8_t *data)
{
    uint8_t r1;
    uint16_t i;
    uint16_t repeat;


    if(SD_Type!=SD_TYPE_V2HC)
    {
        sector = sector<<9;
    }   
    r1 = SD_SendCommand(CMD24, sector, 0x00);
    if(r1 != 0x00)
    {
        return r1;  
    }
    

    SD_CS_Low;

    SPI0_communication(0xff);
    SPI0_communication(0xff);
    SPI0_communication(0xff);

    SPI0_communication(0xFE);


    for(i=0;i<512;i++)
    {
        SPI0_communication(*data++);
    }

    SPI0_communication(0xff);
    SPI0_communication(0xff);
    
    //等待SD卡应答
    r1 = SPI0_communication(0xff);
    if((r1&0x1F)!=0x05)
    {
        SD_CS_High;
        return r1;
    }
    
    //等待操作完成
    repeat = 0;
    while(!SPI0_communication(0xff))
    {
        repeat++;
        if(repeat>0xfffe)        //如果长时间写入没有完成，报错退出
        {
            SD_CS_High;
            return 1;           //写入超时返回1
        }
    }	    
    //写入完成，片选置1
    SD_CS_High;
    SPI0_communication(0xff);

    return 0;
}				           
//读SD卡的多个block(实际测试过)										    
//输入:uint32_t sector 扇区地址（sector值，非物理地址） 
//     uint8_t *buffer 数据存储地址（大小至少512byte）
//     uint8_t count 连续读count个block 		   
//返回值:0： 成功
//       other：失败															  
uint8_t SD_ReadMultiBlock(uint32_t sector, uint8_t *buffer, uint8_t count)
{
    uint8_t r1;	 			 
    //SPIx_SetSpeed(SPI_SPEED_HIGH);//设置为高速模式  
 	//如果不是SDHC，将sector地址转成byte地址
    if(SD_Type!=SD_TYPE_V2HC)sector = sector<<9;  
    //SD_WaitDataReady();
    //发读多块命令
	r1 = SD_SendCommand(CMD18, sector, 0);//读命令
	if(r1 != 0x00)return r1;	 
    do//开始接收数据
    {
        if(SD_ReceiveData(buffer, 512, RELEASE) != 0x00)break; 
        buffer += 512;
    } while(--count);		 
    //全部传输完毕，发送停止命令
    SD_SendCommand(CMD12, 0, 0);
    //释放总线
    SD_CS_High;
    SPI0_communication(0xFF);    
    if(count != 0)return count;   //如果没有传完，返回剩余个数	 
    else return 0;	 
}											  
//写入SD卡的N个block(未实际测试过)									    
//输入:uint32_t sector 扇区地址（sector值，非物理地址） 
//     uint8_t *buffer 数据存储地址（大小至少512byte）
//     uint8_t count 写入的block数目		   
//返回值:0： 成功
//       other：失败															   
uint8_t SD_WriteMultiBlock(uint32_t sector, const uint8_t *data, uint8_t count)
{
    uint8_t r1;
    uint16_t i;
    uint16_t repeat;

    //设置为高速模式
    //SPIx_SetSpeed(SPI_SPEED_HIGH);	   
    //如果不是SDHC，给定的是sector地址，将其转换成byte地址
    if(SD_Type!=SD_TYPE_V2HC)
    {
        sector = sector<<9;
    }   
    r1 = SD_SendCommand(CMD25, sector, 0x00);
    if(r1 != 0x00)
    {
        return r1;  //应答不正确，直接返回
    }
    
    //开始准备数据传输
    SD_CS_Low;
    //先放3个空数据，等待SD卡准备好
    SPI0_communication(0xff);
    SPI0_communication(0xff);
//    SPI0_communication(0xff);

	do{
	    //放起始令牌0xFE
	    SPI0_communication(0xFC);
	
	    //放一个sector的数据
	    for(i=0;i<512;i++)
	    {
	        SPI0_communication(*data++);
	    }
	    //发2个Byte的dummy CRC
	    SPI0_communication(0xff);
	    SPI0_communication(0xff);
	    
	    //等待SD卡应答
	    r1 = SPI0_communication(0xff);
		
	    if((r1&0x1F)!=0x05)
	    {
	        SD_CS_High;
	        return r1;
	    }
	   	
	    //等待操作完成
	    repeat = 0;
	    while(!SPI0_communication(0xff))
	    {
	        repeat++;
	        if(repeat>0xfffe)        //如果长时间写入没有完成，报错退出
	        {
	            SD_CS_High;
	            return 1;           //写入超时返回1
	        }
	    }
	}while(--count);
		    
    //写入完成，片选置1
	r1 = SPI0_communication(0xFD);
    SD_CS_High;
    SPI0_communication(0xff);
	delay_1ms(6);  // 写完多个块以后，延时稳定！否则不能从SD卡读数据！
    return 0;
}						  					  


