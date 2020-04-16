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
* Filename      : lcd.c
* Programmer(s) : JJL
* Version       : V3.00
*********************************************************************************************************
*                                              DESCRIPTION
*
*
* This module provides an interface to an alphanumeric display module.
*
* The current version of this driver supports any  alphanumeric LCD module based on the:
*     Hitachi HD44780 DOT MATRIX LCD controller.
*
* This driver supports LCD displays having the following configuration:
*
*     1 line  x 16 characters     2 lines x 16 characters     4 lines x 16 characters
*     1 line  x 20 characters     2 lines x 20 characters     4 lines x 20 characters
*     1 line  x 24 characters     2 lines x 24 characters
*     1 line  x 40 characters     2 lines x 40 characters
*********************************************************************************************************
*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/
#include "LCD.h"

/*
*********************************************************************************************************
*                                            LOCAL CONSTANTS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/
typedef uint8_t CPU_INT08U;
typedef uint32_t CPU_INT32U;

static  CPU_INT08U   DispMaxCols;      /* Maximum number of columns (i.e. characters per line)         */
static  CPU_INT08U   DispMaxRows;      /* Maximum number of rows for the display                       */


/*
*********************************************************************************************************
*                                        LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void         DispCursorSet(CPU_INT08U row, CPU_INT08U col);

/*$PAGE*/
/*
*********************************************************************************************************
*                                           DISPLAY A CHARACTER
*
* Description : This function is used to display a single character on the display device
* Arguments   : 'row'   is the row    position of the cursor in the LCD Display
*                       'row' can be a value from 0 to 'DispMaxRows - 1'
*               'col'   is the column position of the cursor in the LCD Display
*                       'col' can be a value from 0 to 'DispMaxCols - 1'
*               'c'     is the character to write to the display at the current ROW/COLUMN position.
* Returns     : none
*********************************************************************************************************
*/

void  DispChar (CPU_INT08U row, CPU_INT08U col, CPU_INT08U c)
{
    if (row < DispMaxRows && col < DispMaxCols) {
       
        DispCursorSet(row, col);            /* Position cursor at ROW/COL                              */
        DispSel(DISP_SEL_DATA_REG);
        DispDataWr(c);                      /* Send character to display                               */
       
    }
}



/*$PAGE*/
/*
*********************************************************************************************************
*                                     POSITION THE CURSOR (Internal)
*
* Description : This function positions the cursor into the LCD buffer
* Arguments   : 'row'   is the row    position of the cursor in the LCD Display
*                       'row' can be a value from 0 to 'DispMaxRows - 1'
*               'col'   is the column position of the cursor in the LCD Display
*                       'col' can be a value from 0 to 'DispMaxCols - 1'
* Returns     : none
*********************************************************************************************************
*/

static  void  DispCursorSet (CPU_INT08U row, CPU_INT08U col)
{
    DispSel(DISP_SEL_CMD_REG);                             /* Select LCD display command register      */
    switch (row) {
        case 0:
             if (DispMaxRows == 1) {                       /* Handle special case when only one line   */
                 if (col < (DispMaxCols >> 1)) {
                     DispDataWr(0x80 + col);               /* First  half of the line starts at 0x80   */
                 } else {                                  /* Second half of the line starts at 0xC0   */
                     DispDataWr(0xC0 + col - (DispMaxCols >> 1));
                 }
             } else {
                 DispDataWr(0x80 + col);                   /* Select LCD's display line 1              */
             }
             break;

        case 1:
             DispDataWr(0xC0 + col);                       /* Select LCD's display line 2              */
             break;

        case 2:
             DispDataWr(0x80 + DispMaxCols + col);         /* Select LCD's display line 3              */
             break;

        case 3:
             DispDataWr(0xC0 + DispMaxCols + col);         /* Select LCD's display line 4              */
             break;
    }
}

/*$PAGE*/



/*$PAGE*/
/*
*********************************************************************************************************
*                                      DISPLAY DRIVER INITIALIZATION
*
* Description : This function initializes the display driver.
* Arguments   : maxrows      specifies the number of lines on the display (1 to 4)
*               maxcols      specified the number of characters per line
* Returns     : None.
* Notes       : - DispInit() MUST be called only when multitasking has started.  This is because
*                 DispInit() requires time delay services from the operating system.
*               - DispInit() MUST only be called once during initialization.
*********************************************************************************************************
*/

void  DispInit (CPU_INT08U maxrows, CPU_INT08U maxcols)
{
  
    DispMaxRows = maxrows;
    DispMaxCols = maxcols;
               
    DispSel(DISP_SEL_CMD_REG);         /* Select command register.                                     */
   DispDly_uS(55000);
    DispDataWr( LCD_8bit_Interface|LCD_1line_Mode|LCD_5X7_Format);/* Function Set: 8 bit, Only writes upper nibble          */
   DispDly_uS(55000);
    DispDataWr( LCD_8bit_Interface|LCD_1line_Mode|LCD_5X7_Format);/* Function Set: 8 bit, Only writes upper nibble          */
DispDly_uS(55000);
    DispDataWr( LCD_8bit_Interface|LCD_1line_Mode|LCD_5X10_Format);/* Function Set: 8 bit, Only writes upper nibble          */
DispDly_uS(55000);
    DispDataWr(LCD_4bit_Interface|LCD_2line_Mode|LCD_5X7_Format);/* Function Set: 4 bit, Only writes upper nibble          */
DispDly_uS(55000);
    DispDataWr(Char_Disp_On|Char_Disp_Shift_Off);         /* Two lines, 1/16 duty cycle, 5x8 dots, Operation Mode         */
DispDly_uS(55000);
    DispDataWr(Disp_On|Cur_UnderLine_Off|Cur_Blink_Off);//  DispDataWr(DISP_CMD_ON_OFF);       /* Disp ON/OFF: Display ON, cursor OFF and no BLINK character   */
DispDly_uS(55000);
    DispDataWr(Disp_And_Cur_Home);          /* Send command to LCD display to clear the display             */
DispDly_uS(55000);
    DispDataWr(Clear_Display);          /* Entry mode: Inc. display data address when writing           */
DispDly_uS(55000);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         DISPLAY AN ASCII STRING
*
* Description : This function is used to display an ASCII string on a line of the LCD display
* Arguments   : 'row'   is the row    position of the cursor in the LCD Display
*                       'row' can be a value from 0 to 'DispMaxRows - 1'
*               'col'   is the column position of the cursor in the LCD Display
*                       'col' can be a value from 0 to 'DispMaxCols - 1'
*               's'     is a pointer to the string to write to the display at
*                       the desired row/col.
* Returns     : none
*********************************************************************************************************
*/

void  DispStr(CPU_INT08U row, CPU_INT08U col, CPU_INT08U *s)
{
    CPU_INT08U i;


    if (row < DispMaxRows && col < DispMaxCols) {
        
        DispCursorSet(row, col);            /* Position cursor at ROW/COL                              */
        DispSel(DISP_SEL_DATA_REG);
        i = col;                            /* Set counter to limit column to maximum allowable column */
        while (i < DispMaxCols && *s) {     /* Write all chars within str + limit to DispMaxCols       */
            DispDataWr(*s++);               /* Send character to LCD display                           */
            i++;                            /* Increment limit counter                                 */
        }
       
    }
}

void DispConstStr(CPU_INT08U row,CPU_INT08U col,const CPU_INT08U *s)
{
	 CPU_INT08U i;
	if (row < DispMaxRows && col < DispMaxCols) {
       
        DispCursorSet(row, col);            /* Position cursor at ROW/COL                              */
        DispSel(DISP_SEL_DATA_REG);
        i = col;                            /* Set counter to limit column to maximum allowable column */
        while (i < DispMaxCols && *s) {      /* Write all chars within str + limit to DispMaxCols       */
            DispDataWr(*s++);               /* Send character to LCD display                           */
            i++;                            /* Increment limit counter                                 */
        }
       
    }
}

void DispDataWr( CPU_INT08U data)
{

	DispDataWrOneNibble(data>>4);                     /* Write the UPPER nibble of ‘data’ to D7..D4 */
	GPIO_WriteBit(GPIOE,E_PIN , Bit_SET);							/* Set the E control line HIGH */
  DispDly_uS(4000);                                 /* Delay for about 100 µS */
	GPIO_WriteBit(GPIOE,E_PIN , Bit_RESET);						/* Set the E control line LOW */
	DispDataWrOneNibble(data);                        /* Write the LOWER nibble of ‘data’ to D7..D4 */
	GPIO_WriteBit(GPIOE,E_PIN , Bit_SET);							/* Set the E control line HIGH */
  DispDly_uS(4000);                                 /* Delay for about 100 µS */
	GPIO_WriteBit(GPIOE,E_PIN , Bit_RESET);						/* Set the E control line LOW */
DispDly_uS(4000); 
}

void DispDly_uS(uint16_t us)
{
	while(us--)
		continue;
}
void  DispDataWrOneNibble(CPU_INT08U data)
{
	GPIOE->ODR &= 0xFFFF0FFF;    //Clear PD12,13,14,15
	GPIOE->ODR |=  data << 12;   //Wirte One Nibble to GPIOA->ODR
}


void DispInitPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPEEN , ENABLE) ; //enable GPIOA Clock
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 
															 |GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOE,&GPIO_InitStructure);	
}

void DispSel( CPU_INT08U sel)
{
	if( sel == DISP_SEL_CMD_REG )
	{
	GPIO_WriteBit(GPIOE,RS_PIN , Bit_RESET);    /* Set the RS control line to LOW */
	}
	else
	{
		GPIO_WriteBit(GPIOE,RS_PIN , Bit_SET);     /* Set the RS control line to HIGH */
	}
}




