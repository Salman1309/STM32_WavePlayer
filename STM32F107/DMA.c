#include "includes.h"
#include "DMA.h"

void init_DMA2_8bit_mode(unsigned int data_address,int size)
{
	  DMA_InitTypeDef  DMA_InitStructure;
	
		/* DMA Periph clock enable */
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA2 , ENABLE );

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR8R1;
	DMA_InitStructure.DMA_MemoryBaseAddr = data_address;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; 
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte ;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA2_Channel3,&DMA_InitStructure);
	
	
 	DMA_ITConfig( DMA2_Channel3 , DMA_IT_TC , ENABLE ); 
	DMA_Cmd(DMA2_Channel3 , DISABLE );
	NVIC_EnableIRQ( DMA2_Channel3_IRQn);

}

void init_DMA2_Channel3_12bit_mode(unsigned int data_address,int size)
{
 DMA_InitTypeDef  DMA_InitStructure;
	
		/* DMA Periph clock enable */
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA2 , ENABLE );

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1 ;
	DMA_InitStructure.DMA_MemoryBaseAddr = data_address;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; 
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA2_Channel3,&DMA_InitStructure);
	
	DMA_ITConfig( DMA2_Channel3 , DMA_IT_HT , ENABLE );
 	DMA_ITConfig( DMA2_Channel3 , DMA_IT_TC , ENABLE ); 
	DMA_Cmd(DMA2_Channel3 , DISABLE );
	NVIC_EnableIRQ( DMA2_Channel3_IRQn);
}

void init_DMA2_Channel4_12bit_mode(unsigned int data_address,int size)
{
	DMA_InitTypeDef  DMA_InitStructure;
		/* DMA Periph clock enable */
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA2 , ENABLE );

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R2;
	DMA_InitStructure.DMA_MemoryBaseAddr = data_address;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; 
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA2_Channel4,&DMA_InitStructure);
	
	DMA_ITConfig( DMA2_Channel4 , DMA_IT_HT , ENABLE );
 	DMA_ITConfig( DMA2_Channel4 , DMA_IT_TC , ENABLE ); 
	DMA_Cmd(DMA2_Channel4 , DISABLE );
	NVIC_EnableIRQ( DMA2_Channel4_IRQn);
}
