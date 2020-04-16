#ifndef __SD_H_
#define __SD_H_		 	 
#include "stm32f10x.h"
//SD�������ݽ������Ƿ��ͷ����ߺ궨��  
#define NO_RELEASE      0
#define RELEASE         1		 
// SD�����Ͷ���  
#define SD_TYPE_MMC     0
#define SD_TYPE_V1      1
#define SD_TYPE_V2      2
#define SD_TYPE_V2HC    4	   
// SD��ָ���  	   
#define CMD0    0       //����λ
#define CMD1    1
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����sector
#define CMD18   18      //����18����Multi sector
#define ACMD23  23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define ACMD41  41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00
//����д���Ӧ������
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF
//SD����Ӧ�����
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF
 							   						 	 
//OPEN103Cʹ�õ���PA4��ΪSD����CS��.
#define	SD_CS_High  GPIO_WriteBit(GPIOB, GPIO_Pin_1 , Bit_SET)    // SD_CS = 1;
#define SD_CS_Low   GPIO_WriteBit(GPIOB, GPIO_Pin_1 , Bit_RESET)  // SD_CS = 0;					    	  

extern uint8_t  SD_Type;//SD��������
//���������� 

uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t crc);                 //SD������һ������
uint8_t SD_SendCommand_NoDeassert(uint8_t cmd, uint32_t arg, uint8_t crc);		//SD������һ������(������Ƭѡ����)
uint8_t SD_Init(void);                                                    //SD����ʼ��

uint8_t SD_ReceiveData(uint8_t *data, uint16_t len, uint8_t release);           //SD��������
uint8_t SD_GetCID(uint8_t *cid_data);                                       //��SD��CID
uint8_t SD_GetCSD(uint8_t *csd_data);                                       //��SD��CSD
uint32_t SD_GetCapacity(void);                                            //ȡSD������

uint8_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buffer);                 //��һ��BLOCK
uint8_t SD_WriteSingleBlock(uint32_t sector, const uint8_t *buffer); 		    //дһ��BLOCK
uint8_t SD_ReadMultiBlock(uint32_t sector, uint8_t *buffer, uint8_t count); 	//�����BLOCK
uint8_t SD_WriteMultiBlock(uint32_t sector, const uint8_t *data, uint8_t count);//д���BLOCK

#endif






