#include "stm32f10x.h"
#include "SPI_SD.h"

void SPI0_Init_LowSpeed(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 ,ENABLE);	 
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
// 	SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
// 	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
// 	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
// 	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
// 	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
// 	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
// 	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
// 	SPI_InitStruct.SPI_CRCPolynomial = 7;
	
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
}

void SPI0_Init_HighSpeed(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);	
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
// 	SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
// 	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
// 	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
// 	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
// 	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
// 	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
// 	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
// 	SPI_InitStruct.SPI_CRCPolynomial = 7;
	
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
}

uint8_t SPI0_communication(uint8_t data)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(SPI2, data);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI2 bus */
  return SPI_I2S_ReceiveData(SPI2);
}
