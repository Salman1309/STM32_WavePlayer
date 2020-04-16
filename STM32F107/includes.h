#include "stm32f10x.h"                  // Device header
#include "string.h"
#include "ff.h"
#include "diskio.h"
#include "stdio.h"
#include "LCD.h"

#include "sd.h"
#include "usart.h"
#include "User_systick.h"

#define NUMBER_OF_MUSIC_FILES 100

void StartWavePlayer(void);
