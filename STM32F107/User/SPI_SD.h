#ifndef __SPI_SD_H__
#define __SPI_SD_H__

extern void SPI0_Init_LowSpeed(void);
extern void SPI0_Init_HighSpeed(void);
extern uint8_t SPI0_communication(uint8_t data);

#endif

