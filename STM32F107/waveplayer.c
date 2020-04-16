
#include "includes.h"
#include "waveplayer.h"
#include "DMA.h"

#define FCC(c1,c2,c3,c4)	(((DWORD)c4<<24)+((DWORD)c3<<16)+((WORD)c2<<8)+(BYTE)c1)	/* FourCC */

//This struct will hold information related to the WAV file that's being read.

wave_format wave_info;
extern unsigned char Header[HEADER_SIZE];

void read_wav_fileinformation( char *path,char *filename,unsigned char * header)
{
	unsigned int count;
	char field[2];
	char field2[4];
	FIL fil;DIR dir;
	f_opendir(&dir, path);
	if (!(  f_open( &fil	, filename , FA_READ ) ))
	 {
		f_read( &fil , header , HEADER_SIZE, &count );  //Read the wav file header
		 
		//Extract the ChunkSize 
		sprintf(field2,"%x%x%x%x",header[7],header[6],header[5],header[4]);
		str2int(field2,&wave_info.chunk_size);
		
    //Extract the SubChunk2Size
    sprintf(field2,"%x%x%x%x",header[43],header[42],header[41],header[40]);
    str2int(field2,&wave_info.sub_chunk2_size);		 
		
		//Extract the Sample Rate field from the header
		sprintf(field,"%x%x",header[25],header[24]);
		str2int(field,&wave_info.sample_rate);

		//Extract the Byte Rate field from the header
		sprintf(field,"%x%x",header[29],header[28]);
		str2int(field,&wave_info.byte_rate);
		 
		//Extract the audio format from the header
		sprintf(field, "%x%x", header[21], header[20]);
		str2int(field, &wave_info.format);	
		 
		//Extract the bits per sample from the header
		sprintf(field, "%x%x", header[35], header[34]);
		str2int(field, &wave_info.bits_per_sample);	
		 
		//Extract the number of channels from the header
		sprintf(field, "%x%x", header[23],header[22]);
		str2int(field,&wave_info.num_channels);
		
		//Extract the block allign from the header
		sprintf(field, "%x%x", header[33],header[32]);
		str2int(field,&wave_info.block_align);
		f_close(&fil);
//		printf(" File Name = %s\r\n" , filename);
//		printf(" File Size = %d Bytes\r\n" , f_size(&fil));
//		printf(" Chunk Size = %d Bytes\r\n", wave_info.chunk_size);
//		printf(" Sub Chunk 2 Size = %d Bytes\r\n" , wave_info.sub_chunk2_size);
//		
//		printf(" Audio Format = %d\r\n", wave_info.format);
//		printf(" Number of Channels = %d\r\n", wave_info.num_channels);
//		printf(" Sample Rate = %d\r\n", wave_info.sample_rate);
//		printf(" Byte Rate = %d\r\n", wave_info.byte_rate);
//		printf(" Bits Per Sample = %d\r\n",wave_info.bits_per_sample);
//		printf(" Block Align = %d\r\n", wave_info.block_align);
		
	 }
	else
	{
		printf("Error");
	}
	
	return;
}

/* convert a hex number string into int */
int str2int(char * str,int * val_p)
{
  int i, len;
//  char c;

  *val_p = 0;
  len = strlen(str);
  for(i = 0; i < len; i++)
  {
    if(str[i]>='0'&&str[i]<='9')
    {
	(*val_p)+=((str[i]-'0')<<(4*(len-i-1)));
    }
    else if(str[i]>='a'&&str[i]<='f')
    {
	(*val_p)+=((str[i]-'a'+0xa)<<(4*(len-i-1)));
    }
    else
    {
	return 1;
    }
  }
  return 0;
}
/*
 Start Wave Player
*/
void DisableWavePlayerHardware(void)
{
	TIM_Cmd(TIM2, DISABLE);
	DMA_Cmd(DMA2_Channel3 , DISABLE ); DMA_Cmd(DMA2_Channel4 , DISABLE );
	DAC_DMACmd( DAC_Channel_1 , DISABLE ) ; DAC_DMACmd( DAC_Channel_2 , DISABLE ) ;
}

void Hardware_for_waveplayer_init(void)
{
	DAC_InitTypeDef            DAC_InitStructure;
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  GPIO_InitTypeDef GPIO_InitStructure;


	
	/* Enable peripheral clocks ------------------------------------------------*/
  /* GPIOA Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  /* TIM2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN ;// GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  /* TIM2 Configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period =( (uint16_t)(36000000/( wave_info.sample_rate )  ) )  ;//1632/2-1;   
 // TIM_TimeBaseStructure.TIM_Period =680;//1632/2-1;     
  TIM_TimeBaseStructure.TIM_Prescaler = 1;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down ;  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM2 TRGO selection */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
  DAC_InitStructure.DAC_WaveGeneration =  DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	DAC_DMACmd( DAC_Channel_1 , ENABLE ) ;
	  

		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN ;// GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
				
           
		DAC_Init(DAC_Channel_2,&DAC_InitStructure);
		DAC_DMACmd( DAC_Channel_2 , ENABLE ) ;
	

}

void TIM4_Start(void)
{	//20ms Heart Beat 
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	 TIM4->PSC = 72-1;
	 TIM4->ARR = 20000-1;
	 TIM4->CR1 |=1;        /* TIM4 ON */  
	 TIM4->DIER|= TIM_DIER_UIE;
   NVIC_EnableIRQ(TIM4_IRQn);
}
void GPIO_for_Push_Button_Start(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN | RCC_APB2ENR_AFIOEN; 

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 ;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_WriteBit(GPIOE,GPIO_Pin_0,Bit_SET);
	GPIO_WriteBit(GPIOE,GPIO_Pin_1,Bit_SET);
	

	
}
void StartWavePlayer(void)
{
	SystemInit();
	GPIO_for_Push_Button_Start();
	DispInitPort();    
	DispInit(2,16);
	DispConstStr(0,1,"Wave Player");	
	TIM4_Start();
	USART_Configuration();	
	
	f_mount(&fs,path,1);
	scan_files(path);

for(music_index = 0;music_index < NUMBER_OF_MUSIC_FILES ; music_index++)
	{
		DispConstStr(1,1,"                ");
		DispConstStr(1,1,&music_name[music_index][0]);
		play_wave_file(path,&music_name[music_index][0]);	
	}
  DisableWavePlayerHardware();
}

void play_wave_file( char *path,char *filename)
{
unsigned int count;
//	char field[2];
	FIL fil;DIR dir;
	uint32_t WAVE_counter = 0 ;
	long size;
	f_opendir(&dir, path);
	read_wav_fileinformation(path,filename,Header);
	
	if(wave_info.block_align == 1 ) //8 bit data
	{
		init_DMA2_8bit_mode((uint32_t)&Left_Channel,512);
	}
	if(wave_info.block_align == 2 ) //16 mono bit data
	{
		if( wave_info.num_channels == 1 )
		{
		init_DMA2_Channel3_12bit_mode((uint32_t)&Left_Channel_16bit[0],1024); //Mono
		init_DMA2_Channel4_12bit_mode((uint32_t)&Left_Channel_16bit[0],1024); //Mono
		}
		
	}
	if( wave_info.block_align == 4 ) //16bit stereo data
	{
		if( wave_info.num_channels == 2 )
		{
		init_DMA2_Channel4_12bit_mode((uint32_t)&Right_Channel_16bit,512);
		init_DMA2_Channel3_12bit_mode((uint32_t)&Left_Channel_16bit,512); //Stereo
		}
	}
	
	Hardware_for_waveplayer_init();		
	if (!(  f_open( &fil	, filename , FA_READ ) ))
		 {
			 size = f_size(&fil);
		   TIM_Cmd(TIM2, ENABLE);
	     DMA_Cmd(DMA2_Channel3 , ENABLE ); 
			 DMA_Cmd(DMA2_Channel4 , ENABLE );
			 DAC_Cmd( DAC_Channel_1 , ENABLE ) ;
			 DAC_Cmd( DAC_Channel_2 , ENABLE ) ;
			  
			 while( WAVE_counter < size )
			 { 
					  		
       if( wave_info.block_align == 1 )
			 {
					 f_read( &fil , buffer , 512 , &count );  
					 WAVE_counter += 512;
					 
						while(!Flag_1)
						 continue;
						Flag_1=  0;
						
						split_8bit_wavedata(buffer,Left_Channel);
			 }
			 
			 else if( wave_info.block_align == 2 | wave_info.block_align == 4 ) //16bit Mono or Stereo
			 {
				 static char mono_state = 0;
						
						 if(wave_info.num_channels == 1 )
						 {			 
						 switch( mono_state )
						 {
							 case 0:
										 f_read( &fil, buffer , 2048 , &count) ;
										 WAVE_counter+= 2048;
                     split_12bit_mono_wavedata(buffer,&Left_Channel_16bit[0]);
							       split_12bit_mono_wavedata(buffer,&Left_Channel_16bit[512]);
								     mono_state = 1;
								 break;
						   
							 case 1:
								     while(! ( HTC_1 & HTC_2 ) )
											 continue ;
										 HTC_1 = 0; HTC_2 = 0;
										 f_read(&fil, buffer , 1024 , &count);
										 WAVE_counter += 1024;
										 split_12bit_mono_wavedata(buffer,&Left_Channel_16bit[0]);
										 mono_state = 2;
								 break;
										 
							 case 2:
								      while(!(TC_1 & TC_2 ))
												continue;
											TC_1 = 0;
											TC_2 = 0;
											f_read(&fil, buffer , 1024 , &count);
										  WAVE_counter += 1024;
											split_12bit_mono_wavedata(buffer,&Left_Channel_16bit[512]);
											mono_state = 1;
								 break;
						 } 
						 }//if(wave_info.num_channels == 1 )
						 if(wave_info.num_channels == 2 )
						 {
							 
						 f_read( &fil , buffer , 2048 , &count );  
					   WAVE_counter += 2048;
						 while(!( Flag_1 & Flag_2 ))
						 continue;
						 Flag_1 = Flag_2 =  0;
						 split_12bit_stereo_wavedata(buffer,&Left_Channel_16bit[0],&Right_Channel_16bit[0]);
						 
						 }
			 }
       if( skip_music == 1 )
			 {
				skip_music = 0;
			 TIM_Cmd(TIM2, DISABLE);
	     DMA_Cmd(DMA2_Channel3 , DISABLE ); 
			 DMA_Cmd(DMA2_Channel4 , DISABLE );
			 DAC_Cmd(DAC_Channel_1 , DISABLE ) ;
			 DAC_Cmd(DAC_Channel_2 , DISABLE ) ;
			 break;		
			 }
         		 
		   }//end of while( counter < size )
			 
	  }
	else
		{
			printf("Error");
		}
	f_close(&fil);
}


///////////////////////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////////////////////

void split_8bit_wavedata(char* from,char* to)
{
	static char state = 0;

  int counter = 0;
	switch(state)
	{
		case 0:

					for(counter = 0 ; counter <= 512 ; counter++)//212
					{
						 *to = from[44+counter];
						to++;
						
					} 
			state = 1;
			break;
		
		case 1:
					for(counter = 0 ; counter <= 512 ; counter++) 
				{
					 *to = from[counter];
					to++;
				}
			break;
	}
}//end of void split_8bit_wavedata(char* from,char* to)

void split_12bit_mono_wavedata(char *from , unsigned short int  * to )
{
	static char state_16bit = 0;  
	static int counter = 0;
	static uint16_t temp1 = 0;
	static uint16_t temp2 = 0;
	switch(state_16bit)
	{
		case 0:
				for(counter = 0 ; counter <= 1020 ;)
					{
						 temp1 = from[44+counter];
					   counter++;
						 temp2 = from[44+counter];
						 counter++;					 	 
						 *to =  (( temp1 |(temp2)<<8 ) +32768 ) >> 4 ;		
						to++;
					} 
					state_16bit = 1;
			break;
		
		case 1:
			counter = 0;
		      while(counter<1024) // **************************************************************
					{
						temp1 = from[counter++];
						temp2 = from[counter++];
						*to =  (( temp1 |(temp2)<<8 ) +32768 ) >> 4 ;			
						to++;
					} 
			break;
	}
}
void split_12bit_stereo_wavedata(char *from,unsigned short int *left,unsigned short int *right)
{
	static char state_16bit = 0;  
	int counter = 0;
	uint16_t temp1 = 0;
	uint16_t temp2 = 0;
	switch(state_16bit)
	{
		case 0:
				for(counter = 0 ; counter <= 2040 ;)
					{
						 temp1 = from[44+counter];
					   counter++;
						 temp2 = from[44+counter];
						 counter++;
					 	 *left =  (( temp1 |(temp2)<<8 ) +32768 ) >> 4 ;					
             left++;//increase left channel address
						
						 temp1 = from[44+counter];
					   counter++;
						 temp2 = from[44+counter];
						 counter++;
					 	 *right =  (( temp1 |(temp2)<<8 ) +32768 ) >> 4 ;		
             right++;//increase right channel address
						
						
					} 
					state_16bit = 1;
			break;
		
		case 1:
				for(counter = 0 ; counter <= 2040 ;)
					{
		         temp1 = from[counter++];
						 temp2 = from[counter++];
					 	 *left =  (( temp1 |(temp2)<<8 ) +32768 ) >> 4 ;					 
             left++; //increase left channel address 
						
						 temp1 = from[counter++];
						 temp2 = from[counter++];
					 	 *right =  (( temp1 |(temp2)<<8 ) +32768 ) >> 4 ;	
             right++; //increase right channel address
					} 
			break;
	}
}

/*
SD Card 
*/
FRESULT scan_files (char* path)
{
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;
	char counter = 0;
#if _USE_LFN
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);		
            if (res != FR_OK || fno.fname[0] == 0) break;
            if (fno.fname[0] == '.') continue;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) {
                sprintf(&path[i], "/%s", fn);
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {
							sprintf(&music_name[counter++][0],"%s",fn);
              //  printf("%s/%s \r\n", path, fn);
            }
        }
    }
    return res;
}

int SD_TotalSize(void)
{
    FATFS *fs;
    DWORD fre_clust;        
    res = f_getfree("0:", &fre_clust, &fs); 
    if ( res==FR_OK ) 
    {
	  /* Print free space in unit of MB (assuming 512 bytes/sector) */
      printf("\r\n%d MB total drive space.\r\n"
             "%d MB available.\r\n",( (fs->n_fatent - 2) * fs->csize ) / 2 /1024 , (fre_clust * fs->csize) / 2 /1024 );		
	  return ENABLE;
	}
	else 
	  return DISABLE;   
}	 
void readFileData(char *path,char *filename)
{
   	FIL fil;
    DIR dir;
	  char buffer[511];
	  uint32_t counter = 0;
	  int a;
    unsigned int x;
	  uint32_t size;
    res = f_opendir(&dir, path);
	  
	 if (!(  f_open( &fil	, filename , FA_READ ) ))
	 {
		 printf("%s is open sucessfully \r\n", filename ) ;
		 printf("Size of File is %d \r\n", f_size(&fil));
		 size = f_size(&fil);	 
		 while( counter < size )
		 {
			 for(a = 0 ;a < 511; a++ ) 
			 buffer[a] = 0;
			 counter += 511;
			 f_read(&fil,buffer,sizeof(buffer),&x);
//			 put_wavedata(buffer,Left_Channel);
			 #ifndef PRINTASSTRING
			 for(a = 0 ;a<511;a++)
			 {
				 USART_SendData(USART1, buffer[a]);
				 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			 }
		   #else
			 for(a = 0 ;a < 511; a++ ) 
       printf("%d ",Left_Channel_16bit[a]);
       #endif			 
     }
	 }
	 else
	 {
		 printf("%s/%s Cannot be opened\r\n",path,filename);
	 }
}


void printFileDDMMYY(FILINFO file)
{	unsigned char Day;
	unsigned short Year;
	char  Month;
	char HH,MM,SS;
	Day = file.fdate & 0x001F;
	Month = (file.fdate & 0x1E0 ) >> 5;
	if(Month == 0 ) Month++; //To be January
	Year = 1980 + (( file.fdate & 0xFE00 ) >> 9 );
	
	SS = ( file.ftime & 0x001F ) * 2 ;
	MM = ( file.ftime & 0x7E0 ) >> 5 ;
	HH = ( file.ftime & 0xF800) >> 11;
	
	printf("File was modified on %d/%s/%d   at  %d:%d:%d\r\n\r\n" , Day , MonthArray[Month-1] ,Year , HH, MM , SS); 
}

