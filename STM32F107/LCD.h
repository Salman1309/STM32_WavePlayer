/*
*********************************************************************************************************
*                                               uC/LCD
*                                         LCD Module Driver
*
*                              (c) Copyright 2005; Micrium; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        LCD MODULE SOURCE FILE
*
* Filename      : lcd.h
* Programmer(s) : JJL
* Version       : V3.00
*********************************************************************************************************
*/
#include "stm32f10x.h"   
/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/
#define  DISP_BUS_WIDTH           4        /* Data bus width: 4 or 8                               */
#define  DISP_MODULE_PRESENT        1

#define  DISP_SEL_CMD_REG           0
#define  DISP_SEL_DATA_REG          1

#define RS_PIN  GPIO_Pin_10
#define E_PIN   GPIO_Pin_11
/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
typedef uint8_t CPU_INT08U;
typedef uint32_t CPU_INT32U;
void  DispChar(CPU_INT08U row, CPU_INT08U col, CPU_INT08U c);
void  DispClrScr(void);
void  DispDefChar(CPU_INT08U id, CPU_INT08U *pat);
void  DispDly_uS(uint16_t us);
void  DispInit(CPU_INT08U maxrows, CPU_INT08U maxcols);
void  DispStr(CPU_INT08U row, CPU_INT08U col, CPU_INT08U *s);
void DispConstStr(CPU_INT08U row,CPU_INT08U col,const CPU_INT08U *s);

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*                                            HARDWARE SPECIFIC
*********************************************************************************************************
*/


void  DispDataWrOneNibble(CPU_INT08U data);
void  DispDataWr(CPU_INT08U data);
void  DispInitPort(void);
void  DispSel(CPU_INT08U sel);
/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*                                               OS SPECIFIC
*********************************************************************************************************
*/



#define First_Line		0x80
#define Second_Line		0xC0
#define Third_Line		0x94
#define Fourth_Line 	0xD4
///////////////////////////////////////////////////////////////////////////////////
#define Clear_Display	0x01
///////////////////////////////////////////////////////////////////////////////////
#define Disp_And_Cur_Home 0x02
///////////////////////////////////////////////////////////////////////////////////
//Character Entry Mode Command/////////////////////////////////////////////////////
#define Char_Disp_On	0x06
#define Char_Disp_Off	0x04
#define Char_Disp_Shift_On	0x05
#define Char_Disp_Shift_Off	0x04
///////////////////////////////////////////////////////////////////////////////////
//Display ON/OFF And Cursor////////////////////////////////////////////////////////
#define Disp_On					0x0C
#define Disp_Off				0x08
#define Cur_UnderLine_On		0x0A
#define Cur_UnderLine_Off		0x08
#define Cur_Blink_On			0x09
#define Cur_Blink_Off			0x08
///////////////////////////////////////////////////////////////////////////////////
//Display And Cursor Shift/////////////////////////////////////////////////////////
#define Disp_Shift				0x18//Display_Shift OR Cur_Move
#define Cur_Move				0x10//Display_Shift OR Cur_Move
#define R_Shift					0x14
#define L_Shift					0x10
///////////////////////////////////////////////////////////////////////////////////
//Function Set Command/////////////////////////////////////////////////////////////
#define LCD_8bit_Interface		0x30//LCD_8bit_Interface,LCD_1line_Mode and  
#define LCD_4bit_Interface		0x23//LCD_5X7_Format Command Must be send First
#define LCD_2line_Mode			0x2C
#define LCD_1line_Mode			0x20
#define LCD_5X10_Format			0x22
#define LCD_5X7_Format			0x23
