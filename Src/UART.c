/*
 * UART.c
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */
 
#include <stddef.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "IO_Mapping.h"
#include "LED.h"
#include "RST.h"
#include "UART.h"

#ifndef REMOVE_UART

UART_HandleTypeDef I_Uart1Hndl = {'\0'};
 
void USART1_Init(void)
 {
	 GPIO_InitTypeDef  GPIO_InitStruct = {'\0'};
	 
	 __HAL_RCC_USART1_CLK_DISABLE();
	 __HAL_RCC_USART1_CLK_ENABLE();
	 __HAL_RCC_USART1_FORCE_RESET();
	 __HAL_RCC_USART1_RELEASE_RESET();
 
	  HAL_GPIO_DeInit(IO_PORT_DBG_TX,IO_PIN_DBG_TX);//USART1_TX//
	  HAL_GPIO_DeInit(IO_PORT_DBG_RX,IO_PIN_DBG_RX);//USART1_RX//
	  
	  /*
	  HAL_GPIO_DeInit(GPIOC,GPIO_PIN_6);
	  HAL_GPIO_DeInit(GPIOC,GPIO_PIN_7);
	  */

	  GPIO_Config_Params_AF(&GPIO_InitStruct, GPIO_MODE_AF_PP, IO_PIN_DBG_TX, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF7_USART1);
	  HAL_GPIO_Init(IO_PORT_DBG_TX, &GPIO_InitStruct);//USART1_TX//
	  
	  GPIO_Config_Params_AF(&GPIO_InitStruct, GPIO_MODE_AF_PP, IO_PIN_DBG_RX, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF7_USART1);
	  HAL_GPIO_Init(IO_PORT_DBG_RX, &GPIO_InitStruct);//USART1_RX//
	  
	  /*
	  GPIO_Config_Params(&GPIO_InitStruct, GPIO_MODE_INPUT, GPIO_PIN_6, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	  
	  GPIO_Config_Params(&GPIO_InitStruct, GPIO_MODE_INPUT, GPIO_PIN_7, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	  */
	  
	  I_Uart1Hndl.Instance = USART1;
	  
	  if(HAL_UART_DeInit(&I_Uart1Hndl) != HAL_OK)
	  {
		  Error_Handler();
	  }   
	  
	  //I_Uart1Hndl.Init.BaudRate 	 		= 38400;
	  I_Uart1Hndl.Init.BaudRate 	 		= 9600;
	  I_Uart1Hndl.Init.HwFlowCtl		 	= UART_HWCONTROL_NONE;
	  I_Uart1Hndl.Init.Mode 		 		= UART_MODE_TX_RX;
	  I_Uart1Hndl.Init.OneBitSampling	 	= UART_ONE_BIT_SAMPLE_DISABLE;
	  I_Uart1Hndl.Init.OverSampling  		= UART_OVERSAMPLING_16;
	  I_Uart1Hndl.Init.Parity			 	= UART_PARITY_NONE;
	  I_Uart1Hndl.Init.StopBits 		 	= UART_STOPBITS_1;
	  I_Uart1Hndl.Init.WordLength		 	= UART_WORDLENGTH_8B;
	  
	  if(HAL_UART_Init(&I_Uart1Hndl) != HAL_OK)
	  {
		  Error_Handler();
	  }
 }

 void USART1_DeInit(void)
 {
	 HAL_UART_DeInit(&I_Uart1Hndl);
 }
 

#endif

