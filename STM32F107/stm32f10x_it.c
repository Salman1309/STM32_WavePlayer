/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "User_systick.h"
#include "includes.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern int Flag_1,Flag_2 ;
extern char music_index;
extern char skip_music;
extern int HTC_1,TC_1,HTC_2,TC_2;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

TimingDelay_Decrement();
	
}

void Check_Pause_Button(void)
{static char Button_State;
 static char PauseKey=0;
 static char counter;
	switch(Button_State) //20ms long 
	{
		case 0:
			if( !GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0) ) //Check if the button is pressed for first time 
			{
				counter = 2; //40 ms delay
				Button_State = 1;
			}
			break;
			
		case 1:
			if( counter > 1 ) 
			{
				counter--;
			}
			else
			{
					if( !GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0) ) //Check if the button is still pressed after 20ms 
						{

							Button_State = 2;
						}
			}

				break;
			
		case 2:
			if( GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0) ) //Recheck the button is released to be ensure
			{
				Button_State = 0; //go to idle state 
				PauseKey ^= 1;
			}
			break;
			
	}
		if( PauseKey )
	{
		   TIM_Cmd(TIM2, DISABLE);
	     DMA_Cmd(DMA2_Channel3 , DISABLE ); 
			 DMA_Cmd(DMA2_Channel4 , DISABLE );
			 DAC_Cmd(DAC_Channel_1 , DISABLE ) ;
			 DAC_Cmd(DAC_Channel_2 , DISABLE ) ;
	}
	else
	{
		   TIM_Cmd(TIM2, ENABLE);
	     DMA_Cmd(DMA2_Channel3 , ENABLE ); 
			 DMA_Cmd(DMA2_Channel4 , ENABLE );
			 DAC_Cmd( DAC_Channel_1 , ENABLE ) ;
			 DAC_Cmd( DAC_Channel_2 , ENABLE ) ;
	}
}
void Check_Skip_Button(void)
{
 static char Button_State;
 static char skip_counter;
	

	
	
	switch(Button_State) //20ms long 
	{
		case 0:
			if( !GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1) ) //Check if the button is pressed for first time 
			{
				skip_counter = 2; //wait 40 ms 
				Button_State = 1;
			}
			break;
			
		case 1:
			if( skip_counter ) 
			{
				skip_counter--;
			}
			else
			{
					if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1) ) //Check if the button is released 
				{
					skip_counter = 10;
					Button_State = 2;
				}

			}
			
				break;
			
		case 2:
			if(skip_counter)
				skip_counter--;
			else
			{
			if( GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1) ) //Recheck the button is released to be ensure
			{
				    skip_music = 1;
				    Button_State = 0; //go to idle state
			}
		}
			break;
			
	}
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

void DMA2_Channel3_IRQHandler(void)
{
	//DAC Channel1 Transfer complete interrupt
	if(DMA_GetITStatus(DMA2_IT_TC3))
	{
		TC_1 = 1;
		Flag_1 = 1;
		DMA_ClearITPendingBit(DMA2_IT_TC3);
	}
	//DAC Channel1 Half Transfer complete interrupt
	if( DMA_GetITStatus(DMA2_IT_HT3) )
	{
		HTC_1 = 1;
		DMA_ClearITPendingBit(DMA2_IT_HT3);		
	}
}

void DMA2_Channel4_IRQHandler(void)
{
	//DAC Channel2 Transfer complete interrupt
	if(DMA_GetITStatus(DMA2_IT_TC4))
	{
		TC_2 = 1;
		Flag_2 = 1;
		DMA_ClearITPendingBit(DMA2_IT_TC4);
	}
	//DAC Channel2 Half Transfer complete interrupt
	if( DMA_GetITStatus(DMA2_IT_HT4) )
	{
		HTC_2 = 1;
		DMA_ClearITPendingBit(DMA2_IT_HT4);		
	}
}
/*
TIM4 Interrupt for checking buttons 
*/
void TIM4_IRQHandler (void)
{	void Check_Pause_Button(void);
	void Check_Skip_Button(void);
	extern uint32_t lcd_delay_counter ;
	if ( TIM4->SR & TIM_SR_UIF)
	{
				Check_Skip_Button();	
				Check_Pause_Button();
			 TIM4->SR &=~TIM_SR_UIF;
	}
	
	
}



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
