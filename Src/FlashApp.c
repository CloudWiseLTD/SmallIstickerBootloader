/*
 * FlashApp.c
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */

#include <stddef.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "SPI.h"
#include "TMRS.h"
#include "FlashDrv.h"
#include "FlashApp.h"


ErrorStatus Flash_Init(void)
{
	u8 wrtEnbl = FLS_WRT_ENABLE_INST;
	u8 wrtDsbl = FLS_WRT_DISABLE_INST;
	u8 sttCmd = FLS_RD_STTREG1_INST;

	u8 idWBuf[4] = {FLS_RD_MNDV_ID_INST,0x00,0x00,0x00};
	u8 idRBuf[3] = {'\0'};
	u8 sttRg1Val = 0, writeEnBit = 0x00, busyBit = 0x00;

	if(Flash_Write_Data(&wrtEnbl, 1) == SUCCESS)
	{
		if(Flash_Read_Data(&sttCmd , 1 , &sttRg1Val , 1))
		{
			writeEnBit = ((sttRg1Val & 0x02) >> 1);

			busyBit = (sttRg1Val & 0x01);

			if((writeEnBit == 0x01) && (busyBit == 0x00))
			{
				if(Flash_Read_Data(idWBuf ,4 ,idRBuf , 2))
				{
					if((idRBuf[0] == MANUFACTURER_ID) && (idRBuf[1] == DEVICE_ID))
					{
						Flash_Write_Data(&wrtDsbl,1);

						return SUCCESS;
					}
				}
			}

		}
	}

	Flash_Write_Data(&wrtDsbl,1);

	return ERROR;
}

ErrorStatus Flash_Erase_Sector(u32 sctAddr)
{
	u8 wrtEnbl = FLS_WRT_ENABLE_INST;
	u8 wrtDsbl = FLS_WRT_DISABLE_INST;
	u8 sttCmd = FLS_RD_STTREG1_INST;
	u8 eraseBuf[4] = {'\0'};
	u8 sttRg1Val = 0, timOut = 0, writeEnBit = 0x00, busyBit = 0x00;

	eraseBuf[0] = FLS_ERASE_SECTOR_INST;
	eraseBuf[1] = (u8)((sctAddr & 0xFF0000) >> 16);
	eraseBuf[2] = (u8)((sctAddr & 0xFF00) >> 8);
	eraseBuf[3] = (u8)(sctAddr & 0xFF);

	if(Flash_Write_Data(&wrtEnbl, 1) == SUCCESS)
	{
		if(Flash_Read_Data(&sttCmd, 1, &sttRg1Val , 1) == SUCCESS)
		{
			writeEnBit = ((sttRg1Val & 0x02) >> 1);
			busyBit = (sttRg1Val & 0x01);

			if((writeEnBit == 0x01) && (busyBit == 0x00))
			{
				if(Flash_Write_Data(eraseBuf,4) == SUCCESS)
				{
					while(timOut < 24)
					{
						timOut++;

						HAL_Delay(100);
						
						if(Flash_Read_Data(&sttCmd, 1, &sttRg1Val , 1) == SUCCESS)
						{
							busyBit = (sttRg1Val & 0x01);

							if(busyBit == 0x00)
							{
								Flash_Write_Data(&wrtDsbl,1);

								return SUCCESS;
							}
						}
					}
				}
			}
		}
	}

	Flash_Write_Data(&wrtDsbl, 1);

	return ERROR;
}

ErrorStatus Flash_Check(void)
{
	u8 wrtEnbl = FLS_WRT_ENABLE_INST;
	u8 wrPBuff[8] = {FLS_PRG_PAGE_INST, 0x00, 0x00, 0x00, 0x05, 0x06, 0x07, 0x08};
	u8 wrBuff[4] = {FLS_RD_DATA_INST, 0x00, 0x0, 0x00};
	u8 rdBuff[5] = {'\0'};

	if(Flash_Erase_Sector(0x00000000))
	{
		if(Flash_Write_Data(&wrtEnbl,1))
		{
			if(Flash_Write_Data(wrPBuff,8))
			{
				if(Flash_Read_Data(wrBuff,4,rdBuff,5))
				{
					if((rdBuff[0] == wrPBuff[4]) && (rdBuff[1] == wrPBuff[5])
						&& (rdBuff[2] == wrPBuff[6]) && (rdBuff[3] == wrPBuff[7]))
						{
							return SUCCESS;
						}
				}
			}
		}
	}

	return ERROR;
}


