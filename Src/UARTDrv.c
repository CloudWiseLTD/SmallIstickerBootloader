/*
 * UARTDrv.c
 *
 *  Created on: May 7, 2017
 *      Author: Michael
 */
 
#include <stddef.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "LED.h"
#include "RST.h"
#include "UART.h"
#include "UARTDrv.h"

extern UART_HandleTypeDef I_Uart1Hndl;

 u32 SerialKeyPressedCh(u8* key)
 {
	 if ( __HAL_USART_GET_FLAG(&I_Uart1Hndl, USART_FLAG_RXNE) != RESET)
	 {
		 *key = (u8)((I_Uart1Hndl.Instance->RDR) & 0xFF);
		 
		 return 1;
	 }
	 else if (__HAL_USART_GET_FLAG(&I_Uart1Hndl, USART_FLAG_RXNE) != RESET)
	 {
		 *key = (u8)((I_Uart1Hndl.Instance->RDR) & 0xFF);
		 
		 return 2;
	 }
	 else
	 {
		 return 0;
	 }
 }
 
  u32 SerialKeyPressed(u8* key)
 {
	 if (__HAL_USART_GET_FLAG(&I_Uart1Hndl, USART_FLAG_RXNE) != RESET)
	 {
		*key = (u8)((I_Uart1Hndl.Instance->RDR) & 0xFF);
		 
		 return 1;
	 }
	 else
	 {
		 return 0;
	 }
 }

 void SerialPutChar(u8 c)
 {
	 I_Uart1Hndl.Instance->TDR = (c & (u16)0x01FF); 
	  
	 while (__HAL_USART_GET_FLAG(&I_Uart1Hndl, USART_FLAG_TXE) == RESET)
	 {
	 }
 }
 
 void WaitForEndTx(void)
 {
 	u16 data = 0;

	 while ((__HAL_USART_GET_FLAG(&I_Uart1Hndl, USART_FLAG_TC) == RESET));
	 
	I_Uart1Hndl.Instance->ICR |= (1<<6);
	 	 
	 data = (u8)((I_Uart1Hndl.Instance->RDR) & 0xFF);
 }

