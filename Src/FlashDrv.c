/*
 * FlashDrv.c
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */

#include <stddef.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "IO_Mapping.h"
#include "Defines.h"
#include "TMRS.h"
#include "FlashDrv.h"

extern SPI_HandleTypeDef SPI2_Handle;

ErrorStatus Flash_Write_Data(u8* wrtBuf,u16 wrtBufLen)
{
	u16 inx = 0;
	u8 rdData;
	u8* rdDataPtr = &rdData;

	IO_PORT_FLASH_SPI_CS->BRR = IO_PIN_FLASH_SPI_CS;
	TMR7_Sleep (30);

	for(inx= 0;inx<wrtBufLen;inx++)
	{
		if(SPI_Flash_SendByte(wrtBuf[inx], rdDataPtr) == ERROR)
		{
			IO_PORT_FLASH_SPI_CS->BSRR = IO_PIN_FLASH_SPI_CS;
			TMR7_Sleep (30);
			return ERROR;
		}
	}

	GPIOB->BSRR = GPIO_PIN_12;
	TMR7_Sleep (30);
	return SUCCESS;
}

ErrorStatus Flash_Read_Data(u8* instBuf,u16 instBufLen,u8* readBuf,u16 readBufLen)
{
	u16 inx = 0;
	u8 rdData;
	u8* rdDataPtr = &rdData;

	IO_PORT_FLASH_SPI_CS->BRR = IO_PIN_FLASH_SPI_CS;
	TMR7_Sleep (30);

	for(inx= 0;inx<instBufLen;inx++)
	{
		if(SPI_Flash_SendByte(instBuf[inx], rdDataPtr) == ERROR)
		{
			IO_PORT_FLASH_SPI_CS->BSRR = IO_PIN_FLASH_SPI_CS;
			TMR7_Sleep (30);
			return ERROR;
		}
	}

	for(inx= 0;inx<readBufLen;inx++)
	{
		if(SPI_Flash_SendByte((u8)0x00, (u8*)&readBuf[inx]) == ERROR)
		{
			IO_PORT_FLASH_SPI_CS->BSRR = IO_PIN_FLASH_SPI_CS;
			TMR7_Sleep (30);
			return ERROR;
		}
	}

	IO_PORT_FLASH_SPI_CS->BSRR = IO_PIN_FLASH_SPI_CS;
	TMR7_Sleep (30);
	return SUCCESS;
}

ErrorStatus SPI_Flash_SendByte(u8 wrtByte, u8* rdByte)
{
	u8 dlyCnt = 0;
	static u8* dRegPtr = NULL;

	dRegPtr = (u8*)0x4000380C;

	while((__HAL_SPI_GET_FLAG(&SPI2_Handle,SPI_FLAG_TXE) == 0)&&(dlyCnt < 10))
	{
		dlyCnt++;
		TMR7_Sleep (30);
	}

	if(dlyCnt >= 10)
	{
		return ERROR;
	}

	*dRegPtr = wrtByte;

	dlyCnt = 0;

	while(( __HAL_SPI_GET_FLAG(&SPI2_Handle,SPI_FLAG_RXNE)== 0)&&(dlyCnt < 10))
	{
		dlyCnt++;
		TMR7_Sleep (30);
	}

	if(dlyCnt >= 10)
	{
		return ERROR;
	}

	*rdByte = *dRegPtr;

	return SUCCESS;
}
