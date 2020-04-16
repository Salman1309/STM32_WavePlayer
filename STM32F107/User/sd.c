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
    SPI0_communication(0xff);//����д������ʱ
	SPI0_communication(0xff);     
 	SPI0_communication(0xff);  	 
    //Ƭѡ���õͣ�ѡ��SD��
    SD_CS_Low; 
    //����
    SPI0_communication(cmd | 0x40);//�ֱ�д������
    SPI0_communication(arg >> 24);
    SPI0_communication(arg >> 16);
    SPI0_communication(arg >> 8);
    SPI0_communication(arg);
    SPI0_communication(crc); 
    //�ȴ���Ӧ����ʱ�˳�
    while((r1=SPI0_communication(0xFF))==0xFF)
    {
        repeat++;	    
        if(repeat>200)break; 
    }   
    //�ر�Ƭѡ
    SD_CS_High;
    //�������϶�������8��ʱ�ӣ���SD�����ʣ�µĹ���
    SPI0_communication(0xFF);
    //����״ֵ̬
    return r1;
}		  																				 
												  
uint8_t SD_SendCommand_NoDeassert(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t repeat=0;	         
	uint8_t r1;			   
    SPI0_communication(0xff);//����д������ʱ
	SPI0_communication(0xff);  	 	 
    SD_CS_Low;//Ƭѡ���õͣ�ѡ��SD��	   
    //����
    SPI0_communication(cmd | 0x40); //�ֱ�д������
    SPI0_communication(arg >> 24);
    SPI0_communication(arg >> 16);
    SPI0_communication(arg >> 8);
    SPI0_communication(arg);
    SPI0_communication(crc);   
    //�ȴ���Ӧ����ʱ�˳�
    while((r1=SPI0_communication(0xFF))==0xFF)
    {
        repeat++;	    
        if(repeat>200)break; 
    }  	  
    //������Ӧֵ
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
    //�����Ƭ�汾��Ϣ��v1.0�汾�ģ���r1=0x05����������³�ʼ��
    if(r1 == 0x05)
    {
        //���ÿ�����ΪSDV1.0����������⵽ΪMMC�������޸�ΪMMC
        SD_Type = SD_TYPE_V1;	   
        //�����V1.0����CMD8ָ���û�к�������
        //Ƭѡ�øߣ�������������
        SD_CS_High;
        //�෢8��CLK����SD������������
        SPI0_communication(0xFF);	  

        repeat = 0;
        do
        {
            //�ȷ�CMD55��Ӧ����0x01���������
            r1 = SD_SendCommand(CMD55, 0, 0);
            if(r1 == 0XFF)return r1;//ֻҪ����0xff,�ͽ��ŷ���	  
            //�õ���ȷ��Ӧ�󣬷�ACMD41��Ӧ�õ�����ֵ0x00����������200��
            r1 = SD_SendCommand(ACMD41, 0, 0);
            repeat++;
        }while((r1!=0x00) && (repeat<400));
        // �ж��ǳ�ʱ���ǵõ���ȷ��Ӧ
        // ���л�Ӧ����SD����û�л�Ӧ����MMC��	  
        //----------MMC�������ʼ��������ʼ------------
        if(repeat==400)
        {
            repeat = 0;
            //����MMC����ʼ�����û�в��ԣ�
            do
            {
                r1 = SD_SendCommand(1,0,0);
                repeat++;
            }while((r1!=0x00)&& (repeat<400));
            if(repeat==400)return 1;   //MMC����ʼ����ʱ		    
            //д�뿨����
            SD_Type = SD_TYPE_MMC;
        }
        //----------MMC�������ʼ����������------------	    
        //����SPIΪ����ģʽ
        //LPC_SSP0->CPSR = 0x04;  //����SPI0Ϊ����ģʽ   
				SPI0_Init_HighSpeed();
		SPI0_communication(0xFF);	 
        //��ֹCRCУ��	   
		r1 = SD_SendCommand(CMD59, 0, 0x95);
        if(r1 != 0x00)return r1;  //������󣬷���r1   	   
        //����Sector Size
        r1 = SD_SendCommand(CMD16, 512, 0x95);
        if(r1 != 0x00)return r1;//������󣬷���r1		 
        //-----------------SD����MMC����ʼ������-----------------

    }//SD��ΪV1.0�汾�ĳ�ʼ������	 
    //������V2.0���ĳ�ʼ��
    //������Ҫ��ȡOCR���ݣ��ж���SD2.0����SD2.0HC��
    else if(r1 == 0x01)
    {
        //V2.0�Ŀ���CMD8�����ᴫ��4�ֽڵ����ݣ�Ҫ�����ٽ���������
        buff[0] = SPI0_communication(0xFF);  //should be 0x00
        buff[1] = SPI0_communication(0xFF);  //should be 0x00
        buff[2] = SPI0_communication(0xFF);  //should be 0x01
        buff[3] = SPI0_communication(0xFF);  //should be 0xAA	    
        SD_CS_High;	  
        SPI0_communication(0xFF);//the next 8 clocks			 
        //�жϸÿ��Ƿ�֧��2.7V-3.6V�ĵ�ѹ��Χ
        //if(buff[2]==0x01 && buff[3]==0xAA) //���жϣ�����֧�ֵĿ�����
        {	  
            repeat = 0;
            //������ʼ��ָ��CMD55+ACMD41
    		do
    		{
    			r1 = SD_SendCommand(CMD55, 0, 0);
    			if(r1!=0x01)return r1;	   
    			r1 = SD_SendCommand(ACMD41, 0x40000000, 0);
                if(repeat>200)return r1;  //��ʱ�򷵻�r1״̬  
            }while(r1!=0);		  
            //��ʼ��ָ�����ɣ���������ȡOCR��Ϣ		   
            //-----------����SD2.0���汾��ʼ-----------
            r1 = SD_SendCommand_NoDeassert(CMD58, 0, 0);
            if(r1!=0x00)
			{
				SD_CS_High;//�ͷ�SDƬѡ�ź�
				return r1;  //�������û�з�����ȷӦ��ֱ���˳�������Ӧ��	 
			}//��OCRָ����󣬽�������4�ֽڵ�OCR��Ϣ
            buff[0] = SPI0_communication(0xFF);
            buff[1] = SPI0_communication(0xFF); 
            buff[2] = SPI0_communication(0xFF);
            buff[3] = SPI0_communication(0xFF);		 
            //OCR������ɣ�Ƭѡ�ø�
            SD_CS_High;
            SPI0_communication(0xFF);	   
            //�����յ���OCR�е�bit30λ��CCS����ȷ����ΪSD2.0����SDHC
            //���CCS=1��SDHC   CCS=0��SD2.0
            if(buff[0]&0x40)SD_Type = SD_TYPE_V2HC;    //���CCS	 
            else SD_Type = SD_TYPE_V2;	    
            //-----------����SD2.0���汾����----------- 
            
            //LPC_SSP0->CPSR = 0x04;  //����SPI0Ϊ����ģʽ
						SPI0_Init_HighSpeed();
        }	    
    }
    return r1;
}	 																			   
//��SD���ж���ָ�����ȵ����ݣ������ڸ���λ��
//����: uint8_t *data(��Ŷ������ݵ��ڴ�>len)
//      uint16_t len(���ݳ��ȣ�
//      uint8_t release(������ɺ��Ƿ��ͷ�����CS�ø� 0�����ͷ� 1���ͷţ�	 
//����ֵ:0��NO_ERR
//  	 other��������Ϣ														  
uint8_t SD_ReceiveData(uint8_t *data, uint16_t len, uint8_t release)
{
    // ����һ�δ���
    SD_CS_Low;				  	  
	if(SD_GetResponse(0xFE))//�ȴ�SD������������ʼ����0xFE
	{	  
		SD_CS_High;
		return 1;
	}
    while(len--)//��ʼ��������
    {
        *data=SPI0_communication(0xFF);
        data++;
    }
    //������2��αCRC��dummy CRC��
    SPI0_communication(0xFF);
    SPI0_communication(0xFF);
    if(release==RELEASE)//�����ͷ����ߣ���CS�ø�
    {
        SD_CS_High;//�������
        SPI0_communication(0xFF);
    }											  					    
    return 0;
}																				  
//��ȡSD����CID��Ϣ��������������Ϣ
//����: uint8_t *cid_data(���CID���ڴ棬����16Byte��	  
//����ֵ:0��NO_ERR
//		 1��TIME_OUT
//       other��������Ϣ														   
uint8_t SD_GetCID(uint8_t *cid_data)
{
    uint8_t r1;	   
    //��CMD10�����CID
    r1 = SD_SendCommand(CMD10,0,0xFF);
    if(r1 != 0x00)return r1;  //û������ȷӦ�����˳�������  
    SD_ReceiveData(cid_data,16,RELEASE);//����16���ֽڵ�����	 
    return 0;
}																				  
//��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
//����:uint8_t *cid_data(���CID���ڴ棬����16Byte��	    
//����ֵ:0��NO_ERR
//       1��TIME_OUT
//       other��������Ϣ														   
uint8_t SD_GetCSD(uint8_t *csd_data)
{
    uint8_t r1;	 
    r1=SD_SendCommand(CMD9,0,0xFF);//��CMD9�����CSD
    if(r1)return r1;  //û������ȷӦ�����˳�������  
    SD_ReceiveData(csd_data, 16, RELEASE);//����16���ֽڵ����� 
    return 0;
}  
//��ȡSD�����������ֽڣ�   
//����ֵ:0�� ȡ�������� 
//       ����:SD��������(�ֽ�)														  
uint32_t SD_GetCapacity(void)
{
    uint8_t csd[16];
    uint32_t Capacity;
    uint16_t n;
	uint16_t csize;  					    
	//ȡCSD��Ϣ������ڼ��������0
    if(SD_GetCSD(csd)!=0) return 0;	    
    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)
    {									  
	    Capacity = ((uint32_t)csd[8])<<8;
		Capacity += (uint32_t)csd[9]+1;	 
        Capacity = (Capacity)*1024;//�õ�������
		Capacity *= 512;//�õ��ֽ���			   
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

	//LPC_SSP0->CPSR = 0x04;  //����SPIΪ����ģʽ24MHz  
	SPI0_Init_HighSpeed();	
    //�������SDHC����������sector��ַ������ת����byte��ַ
    if(SD_Type!=SD_TYPE_V2HC)
    {
        sector = sector<<9;
    } 
	r1 = SD_SendCommand(CMD17, sector, 0);//������	 		    
	if(r1 != 0x00)return r1; 		   							  
	r1 = SD_ReceiveData(buffer, 512, RELEASE);		 
	if(r1 != 0)return r1;   //�����ݳ���
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
    
    //�ȴ�SD��Ӧ��
    r1 = SPI0_communication(0xff);
    if((r1&0x1F)!=0x05)
    {
        SD_CS_High;
        return r1;
    }
    
    //�ȴ��������
    repeat = 0;
    while(!SPI0_communication(0xff))
    {
        repeat++;
        if(repeat>0xfffe)        //�����ʱ��д��û����ɣ������˳�
        {
            SD_CS_High;
            return 1;           //д�볬ʱ����1
        }
    }	    
    //д����ɣ�Ƭѡ��1
    SD_CS_High;
    SPI0_communication(0xff);

    return 0;
}				           
//��SD���Ķ��block(ʵ�ʲ��Թ�)										    
//����:uint32_t sector ������ַ��sectorֵ���������ַ�� 
//     uint8_t *buffer ���ݴ洢��ַ����С����512byte��
//     uint8_t count ������count��block 		   
//����ֵ:0�� �ɹ�
//       other��ʧ��															  
uint8_t SD_ReadMultiBlock(uint32_t sector, uint8_t *buffer, uint8_t count)
{
    uint8_t r1;	 			 
    //SPIx_SetSpeed(SPI_SPEED_HIGH);//����Ϊ����ģʽ  
 	//�������SDHC����sector��ַת��byte��ַ
    if(SD_Type!=SD_TYPE_V2HC)sector = sector<<9;  
    //SD_WaitDataReady();
    //�����������
	r1 = SD_SendCommand(CMD18, sector, 0);//������
	if(r1 != 0x00)return r1;	 
    do//��ʼ��������
    {
        if(SD_ReceiveData(buffer, 512, RELEASE) != 0x00)break; 
        buffer += 512;
    } while(--count);		 
    //ȫ��������ϣ�����ֹͣ����
    SD_SendCommand(CMD12, 0, 0);
    //�ͷ�����
    SD_CS_High;
    SPI0_communication(0xFF);    
    if(count != 0)return count;   //���û�д��꣬����ʣ�����	 
    else return 0;	 
}											  
//д��SD����N��block(δʵ�ʲ��Թ�)									    
//����:uint32_t sector ������ַ��sectorֵ���������ַ�� 
//     uint8_t *buffer ���ݴ洢��ַ����С����512byte��
//     uint8_t count д���block��Ŀ		   
//����ֵ:0�� �ɹ�
//       other��ʧ��															   
uint8_t SD_WriteMultiBlock(uint32_t sector, const uint8_t *data, uint8_t count)
{
    uint8_t r1;
    uint16_t i;
    uint16_t repeat;

    //����Ϊ����ģʽ
    //SPIx_SetSpeed(SPI_SPEED_HIGH);	   
    //�������SDHC����������sector��ַ������ת����byte��ַ
    if(SD_Type!=SD_TYPE_V2HC)
    {
        sector = sector<<9;
    }   
    r1 = SD_SendCommand(CMD25, sector, 0x00);
    if(r1 != 0x00)
    {
        return r1;  //Ӧ����ȷ��ֱ�ӷ���
    }
    
    //��ʼ׼�����ݴ���
    SD_CS_Low;
    //�ȷ�3�������ݣ��ȴ�SD��׼����
    SPI0_communication(0xff);
    SPI0_communication(0xff);
//    SPI0_communication(0xff);

	do{
	    //����ʼ����0xFE
	    SPI0_communication(0xFC);
	
	    //��һ��sector������
	    for(i=0;i<512;i++)
	    {
	        SPI0_communication(*data++);
	    }
	    //��2��Byte��dummy CRC
	    SPI0_communication(0xff);
	    SPI0_communication(0xff);
	    
	    //�ȴ�SD��Ӧ��
	    r1 = SPI0_communication(0xff);
		
	    if((r1&0x1F)!=0x05)
	    {
	        SD_CS_High;
	        return r1;
	    }
	   	
	    //�ȴ��������
	    repeat = 0;
	    while(!SPI0_communication(0xff))
	    {
	        repeat++;
	        if(repeat>0xfffe)        //�����ʱ��д��û����ɣ������˳�
	        {
	            SD_CS_High;
	            return 1;           //д�볬ʱ����1
	        }
	    }
	}while(--count);
		    
    //д����ɣ�Ƭѡ��1
	r1 = SPI0_communication(0xFD);
    SD_CS_High;
    SPI0_communication(0xff);
	delay_1ms(6);  // д�������Ժ���ʱ�ȶ��������ܴ�SD�������ݣ�
    return 0;
}						  					  


