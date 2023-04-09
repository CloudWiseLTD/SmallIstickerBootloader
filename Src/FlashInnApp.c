/*
 * FlashInnApp.c
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */
#include <string.h>
#include <stddef.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "FlashDrv.h"
#include "FlashApp.h"
#include "FlashInnApp.h"

 u16 CRC16_Clc(u8 * ptrPct, u16 pctLen, u16  crc16)
 {
	 u8  OddParity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
 
	 u16  pwr = 0, data = 0;
	 
	 for(pwr = 0; pwr < pctLen; pwr++) 
	 {
			 data = (u16)ptrPct[pwr];
 
			 data = ((data ^ (crc16 & 0xFF)) & 0xFF);
 
			 crc16 >>= 8;
 
		  if(OddParity[data & 0xF] ^ OddParity[data >> 4])
		  {
				 crc16 ^= 0xC001;
		  }
  
			 data <<= 6;
 
			 crc16 ^= data;
 
			 data <<= 1;
 
			 crc16 ^= data;
 
	 }
 
		 return (crc16);
 
 }
 
 ErrorStatus Transfer_Version(u32 verByteLn, u32 extFlsAddr, u8 * dataBuff, u32 inFlsDstAdd)
 {
	 u8 wrBuff[4] = {0};
	 u8 count = 0;
	 
	 memset(dataBuff, '\0', 1024);
	
	 wrBuff[0] = FLS_RD_DATA_INST;
	 wrBuff[1] = (u8)((extFlsAddr & 0x00FF0000) >> 16);
	 wrBuff[2] = (u8)((extFlsAddr & 0x0000FF00) >> 8);
	 wrBuff[3] = (u8)((extFlsAddr & 0x000000FF));
 
	 while(verByteLn >= 1024)
	 {
		 if(Flash_Read_Data(wrBuff, 4, dataBuff , 1024))
		 {	 
			 if((((inFlsDstAdd) < INNFLS_STR_APP_ADD) || ((inFlsDstAdd) >= INNFLS_END_APP_ADD)))
			 {
				 return ERROR;
			 }
			 
			 if(Inner_Flash_Write_Version(dataBuff, 1024 , inFlsDstAdd) == ERROR)
			 {
				 return ERROR;
			 }
 
			 inFlsDstAdd += 1024;
			 extFlsAddr += 1024;
			 verByteLn -= 1024;
 
			 wrBuff[1] = (u8)((extFlsAddr & 0x00FF0000) >> 16);
			 wrBuff[2] = (u8)((extFlsAddr & 0x0000FF00) >> 8);
			 wrBuff[3] = (u8)((extFlsAddr & 0x000000FF));

			 count++;
			 if (count >= 16)
			 {
				 count=0;
				 BlinkLed(2);
				 WD_Refresh();
			 }
		 }
		 else
		 {
			 return ERROR;
		 }
	 }
	 
 	memset(dataBuff, '\0', 1024);
	
	 if(verByteLn)
	 {
		 if(Flash_Read_Data(wrBuff, 4, dataBuff , verByteLn))
		 {
			 if((((inFlsDstAdd) < INNFLS_STR_APP_ADD) || ((inFlsDstAdd) >= INNFLS_END_APP_ADD)))
			 {
				 return ERROR;
			 }
			 
			 if(Inner_Flash_Write_Version_Mod(dataBuff, verByteLn , inFlsDstAdd) == ERROR)
			 {
				 return ERROR;
			 }
		 }
		 else
		 {
			 return ERROR;
		 }
	 }
	 
	 return SUCCESS;
 }
 
 ErrorStatus Inner_Flash_Erase(void)
 {
	u32 PgErrPtr = 0;

	FLASH_EraseInitTypeDef I_FlsErase = {'\0'};

	I_FlsErase.Banks = FLASH_BANK_1;
	I_FlsErase.NbPages = 120;
	I_FlsErase.Page = 8;
	I_FlsErase.TypeErase=FLASH_TYPEERASE_PAGES;
	
	if(HAL_FLASHEx_Erase(&I_FlsErase, &PgErrPtr) != HAL_OK)
	{
		return ERROR;
	}

	return SUCCESS;
 }

 ErrorStatus Inner_Flash_Write_Version(u8 * inFlsBuff, u32 inFlsBLen, u32 inFlsDstAdd)
 {
	 u32* RamSourcel = (u32*)inFlsBuff;
 
	 u32 FlsDstAdd = inFlsDstAdd;
	 
	 while(inFlsBLen)
	 {		 
		 if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FlsDstAdd, *(u64*)RamSourcel) != HAL_OK)
		 {
			 return ERROR;
		 }
		 
		 if(*(u64*)FlsDstAdd != *(u64*)RamSourcel)
		 {
			 return ERROR;
		 }
		 
		 FlsDstAdd += 8;
		 
		 RamSourcel += 2;

		 inFlsBLen -= 8;
	 }
 
	 return SUCCESS;
 }

 ErrorStatus Inner_Flash_Write_Version_Mod(u8 * inFlsBuff, u32 inFlsBLen, u32 inFlsDstAdd)
  {
	  u32* RamSourcel = (u32*)inFlsBuff;
  
	  u32 FlsDstAdd = inFlsDstAdd;

	 u32 res = inFlsBLen % 8;

	 inFlsBLen = inFlsBLen - res;
	 
	  while(inFlsBLen)
	  { 	  
		  if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FlsDstAdd, *(u64*)RamSourcel) != HAL_OK)
		  {
			  return ERROR;
		  }
 
		  if(*(u64*)FlsDstAdd != *(u64*)RamSourcel)
		  {
			  return ERROR;
		  }
		  
		  FlsDstAdd += 8;
		  
		  RamSourcel += 2;
 
		  inFlsBLen -= 8;
	  }

	if(res)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FlsDstAdd, *(u64*)RamSourcel) != HAL_OK)
		{
			return ERROR;
		}

		if(*(u64*)FlsDstAdd != *(u64*)RamSourcel)
		{
		  	return ERROR;
		}
	}
	
	  return SUCCESS;
  }


 unsigned char is_flash_empty(void)
 {
	 u32 dataInbuf;
	 u32 innSrcAdd = INNFLS_STR_APP_ADD;
	 u16 i = 0;

	 while(i < 256)
	 {
		 dataInbuf = *(u32*)innSrcAdd;

		 if (dataInbuf != 0xFFFFFFFF)
			 return 0;

		 i += 4;
		 innSrcAdd += 4;
	 }

	 return 1;
 }


 ErrorStatus Inner_Flash_ClcCrc16(u32 verByteLn, u32 innFlsAdd, u16* pCrc16, u8* dataBuff, u32 buffLen)
 {
	 u32 innSrcAdd = innFlsAdd;
 
	 u32 inxI = 0, inxJ = 0 ,dataInbuf = 0, res = 0;
 
	 u16 lclCrc = 0;
	 
	 while(verByteLn >= 1024)
	 {
	 	inxJ = 0;
		
		 for(inxI = 0; inxI < 256 ; inxI++)
		 {
 			dataInbuf = *(u32*)innSrcAdd;

			 dataBuff[inxJ] = (u8)(dataInbuf & 0xFF);
			 
			 dataBuff[(inxJ + 1)] = (u8)((dataInbuf & 0xFF00) >> 8);
			 
			 dataBuff[(inxJ + 2)] = (u8)((dataInbuf & 0xFF0000) >> 16);
			 
			 dataBuff[(inxJ + 3)] = (u8)((dataInbuf & 0xFF000000) >> 24);

			 inxJ += 4;

			 innSrcAdd += 4;
		 }
		 
		 lclCrc  = CRC16_Clc(dataBuff, buffLen, *pCrc16);
 
		 *pCrc16 = lclCrc;
 
		 verByteLn -= 1024;
	 }

	 res = verByteLn % 4;

	 verByteLn -= res;
	 
	 inxJ = 0;
	 
	for(inxI = 0; inxI < (verByteLn/4) ; inxI++)
	{
	  dataInbuf = *(u32*)innSrcAdd;

	  dataBuff[inxJ] = (u8)(dataInbuf & 0xFF);
	  
	  dataBuff[(inxJ + 1)] = (u8)((dataInbuf & 0xFF00) >> 8);
	  
	  dataBuff[(inxJ + 2)] = (u8)((dataInbuf & 0xFF0000) >> 16);
	  
	  dataBuff[(inxJ + 3)] = (u8)((dataInbuf & 0xFF000000) >> 24);

	  inxJ += 4;

	  innSrcAdd += 4;
	}
	  
	lclCrc  = CRC16_Clc(dataBuff, verByteLn , *pCrc16);

	*pCrc16 = lclCrc;
	 
	if(res)
	{
		inxJ = 0;
		
		dataInbuf = *(u32*)innSrcAdd;

		dataBuff[inxJ] = (u8)(dataInbuf & 0xFF);

		dataBuff[(inxJ + 1)] = (u8)((dataInbuf & 0xFF00) >> 8);

		dataBuff[(inxJ + 2)] = (u8)((dataInbuf & 0xFF0000) >> 16);

		dataBuff[(inxJ + 3)] = (u8)((dataInbuf & 0xFF000000) >> 24);

		lclCrc  = CRC16_Clc(dataBuff, res , *pCrc16);

		*pCrc16 = lclCrc;
	}
	  
	return SUCCESS;
 }
 


