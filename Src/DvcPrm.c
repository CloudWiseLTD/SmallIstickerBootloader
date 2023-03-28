/*
 * DvcPrm.c
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */
 
#include <stddef.h>
#include "string.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "FlashDrv.h"
#include "FlashApp.h"
#include "FlashInnApp.h"
#include "DvcPrm.h"
#include "IO_Mapping.h"

sDevice_Params  I_DevicePrm;

ErrorStatus DevParms_Program_Flash(u8 * flsWrtBf, u16 flsWrtBfLn, u32 flsAddr)
{
    static u8 wrPrgBff[320];
	static u8 rdPrgBff[320];
	
	u8 sttCmd = FLS_RD_STTREG1_INST;
	u8 wrtEnbl = FLS_WRT_ENABLE_INST;
	u8 wrtDsbl = FLS_WRT_DISABLE_INST;
	
	u8 wrBuff[4] = {'\0'};
	
	u16 inxI = 0, inxJ = 0, inxZ = 0;

	u8 sttRg1Val = 0, writeEnBit = 0x00, busyBit = 0x00;
	
	wrPrgBff[0] = FLS_PRG_PAGE_INST;
	wrPrgBff[1] = (u8)((flsAddr & 0x00FF0000) >> 16);
	wrPrgBff[2] = (u8)((flsAddr & 0x0000FF00) >> 8);
	wrPrgBff[3] = (u8)((flsAddr & 0x000000FF));

	memcpy(&wrPrgBff[4] , flsWrtBf, flsWrtBfLn);

	wrBuff[0] = FLS_RD_DATA_INST;
	wrBuff[1] = (u8)((flsAddr & 0x00FF0000) >> 16);
	wrBuff[2] = (u8)((flsAddr & 0x0000FF00) >> 8);
	wrBuff[3] = (u8)((flsAddr & 0x000000FF));

	if(Flash_Write_Data(&wrtEnbl, 1) == SUCCESS)
	{
		if(Flash_Read_Data(&sttCmd, 1, &sttRg1Val , 1) == SUCCESS)
		{
			writeEnBit = ((sttRg1Val & 0x02) >> 1);
	
			busyBit = (sttRg1Val & 0x01);
	
			if((writeEnBit == 0x01) && (busyBit == 0x00))
			{
				if(Flash_Write_Data(wrPrgBff, (4 + flsWrtBfLn)) == SUCCESS)
				{
					if(Flash_Read_Data(&sttCmd, 1, &sttRg1Val , 1) == SUCCESS)
					{
						busyBit = (sttRg1Val & 0x01);
						
						for(inxJ=0; inxJ < 20; inxJ++)
						{
							for(inxZ=0;inxZ<1500;inxZ++);//Delay of 500 micro//
							
							if(busyBit == 1)
							{
								if(inxJ >= 19)
								{
									Flash_Write_Data(&wrtDsbl,1);
									
									return ERROR;
								}

							}
							else
							{
								break;
							}

							
							if(Flash_Read_Data(&sttCmd, 1, &sttRg1Val , 1) == SUCCESS)
							{
								busyBit = (sttRg1Val & 0x01);
							}
							else
							{
								Flash_Write_Data(&wrtDsbl,1);
						
								return ERROR;
							}

						}

						if(Flash_Read_Data(wrBuff,4, rdPrgBff, flsWrtBfLn) == SUCCESS)
						{
							for(inxI = 0; inxI < flsWrtBfLn; inxI++)
							{
								if(rdPrgBff[inxI]  !=  wrPrgBff[(inxI + 4)])
								{
									Flash_Write_Data(&wrtDsbl,1);
									
									return ERROR;
								}
							}
							
							Flash_Write_Data(&wrtDsbl,1);
							
							return SUCCESS;
						}
					}
				}
			}
		}
	}
} 
 
ErrorStatus DevParms_Read_Flash(sDevice_Params *P_HbDvcPrms, u32 dpFlsAddr)
{
  	u32	crcCalcFls = 0;

  	u8 wrBuff[4] = {0};

  	wrBuff[0] = FLS_RD_DATA_INST;
  	wrBuff[1] = (u8)((dpFlsAddr & 0x00FF0000) >> 16);
  	wrBuff[2] = (u8)((dpFlsAddr & 0x0000FF00) >> 8);
  	wrBuff[3] = (u8)((dpFlsAddr & 0x000000FF));

  	u32 data_size = sizeof(sDevice_Params);

  	memset(P_HbDvcPrms, 0, data_size);

  	if (Flash_Read_Data(wrBuff, 4, (u8*)P_HbDvcPrms , data_size ))
  	{
  		crcCalcFls = CRC_Calc_Flash_Data((u8*)P_HbDvcPrms, Get_Param_Size_Without_CRC() );

  		if (P_HbDvcPrms->dpCrc == crcCalcFls)
  		{
  			return SUCCESS;
  		}
  		else
  		{
  			return ERROR;
  		}

  	}
  	else
  	{
  		return ERROR;
  	}

  	return ERROR;
  }

  
  u32 CRC_Calc_Flash_Data(u8* pRdFlsbf, u16 dataBtSz)
  {
	  u16 inx = 0;
	  
	  u32 sum = 0;
	  
	  for(inx = 0; inx < dataBtSz ;inx++)
	  {
		  sum += (u32)pRdFlsbf[inx];
	  }
  
	  return sum;
  }
  


ErrorStatus DevParms_Burn_Flash(sDevice_Params* P_DevicePrms)
{
	u8* write_buffer;
	write_buffer = (u8*)P_DevicePrms;
	u32 address = 0;

	u32 data_size = sizeof(sDevice_Params);
	P_DevicePrms->dpCrc = CRC_Calc_Flash_Data((u8*)P_DevicePrms, Get_Param_Size_Without_CRC() );

	if (Flash_Erase_Sector(((u32)(address))))
	{
		if (DevParms_Program_Flash(write_buffer, 256 ,((u32)(address))))
		{
			data_size -= 256;
			if (DevParms_Program_Flash(&(write_buffer[256]), (data_size) ,((u32)(address+0x100))))
			{
				return SUCCESS;
			}
		}
	}

	return ERROR;
}
 

  
  ErrorStatus Flash_Calc_Version_CRC16(u32 verByteLn, u32 extFlsAdd,u8 * dataBuff, u16 * pCrc16)
  {
	  u8 wrBuff[4] = {0};
  
	  u16 crc16Lcl = 0;
		  
	  wrBuff[0] = FLS_RD_DATA_INST;
		  
	  wrBuff[1] = (u8)((extFlsAdd & 0x00FF0000) >> 16);
		  
	  wrBuff[2] = (u8)((extFlsAdd & 0x0000FF00) >> 8);
		  
	  wrBuff[3] = (u8)((extFlsAdd & 0x000000FF));
  
	  while(verByteLn >= 1024)
	  {
		  if(Flash_Read_Data(wrBuff, 4, dataBuff , 1024))
		  {
			  crc16Lcl = CRC16_Clc(dataBuff, 1024 , (*pCrc16));
  
			  *pCrc16 = crc16Lcl;
			  
			  extFlsAdd += 1024;
  
			  verByteLn -= 1024;
  
			  wrBuff[1] = (u8)((extFlsAdd & 0x00FF0000) >> 16);
				  
			  wrBuff[2] = (u8)((extFlsAdd & 0x0000FF00) >> 8);
				  
			  wrBuff[3] = (u8)((extFlsAdd & 0x000000FF));
		  }
		  else
		  {
			  return ERROR;
		  }
	  }
  
	  if(verByteLn)
	  {
		  if(Flash_Read_Data(wrBuff, 4, dataBuff , verByteLn))
		  {
			  crc16Lcl	= CRC16_Clc(dataBuff, verByteLn , (*pCrc16));
  
			  *pCrc16 = crc16Lcl;
		  }
		  else
		  {
			  return ERROR;
		  }
	  }
	  
	  return SUCCESS;
  }
 

 

 // This function and app function should be identical
  void DevParms_Set_Default(sDevice_Params * P_DevicePrms)
  {
  	u16 param_size = sizeof(sDevice_Params);

  	memset(P_DevicePrms, 0, param_size);

  	memcpy(P_DevicePrms->dpSrvIp,"www.cloud-wise.net",18);
  	//memcpy(P_DevicePrms->ssIdName4,"arkessa.com", 11);
  	memcpy(P_DevicePrms->ssIdName4,"data.mono", 9);
  	//memcpy(P_DevicePrms->ssIdName1,"arkessa", 7);
  	memcpy(P_DevicePrms->ssIdName1,"", 7);
  	//memcpy(P_DevicePrms->ssIdPswrd1,"arkessa", 7);
  	memcpy(P_DevicePrms->ssIdPswrd1,"", 7);

  	P_DevicePrms->dpSrvUport = 65;//2//
  	P_DevicePrms->dpSrvTDprt = 7777; // 90//42//

  	P_DevicePrms->dpVersion = 0;//45//
  	P_DevicePrms->dpBuild = 0;//46//
  	P_DevicePrms->dpVersionR = 0;//47//
  	P_DevicePrms->dpBuildR = 0;//48//

  	P_DevicePrms->dpVerRcrc = 0;//49//
  	P_DevicePrms->dpVerRdLn = 0;//50//

  	//P_DevicePrms->SecurityMode = 1;
  	P_DevicePrms->SecurityMode = 0;
  	P_DevicePrms->GsmMode = 2;

  	P_DevicePrms->HardwareType = HARDWARE_TYPE;

  	//memset(P_DevicePrms->ssIdName1,0x00,32);
  	//memset(P_DevicePrms->ssIdPswrd1,0x00,32);

  	memset(P_DevicePrms->ssIdName2,0x00,32);
  	memset(P_DevicePrms->ssIdPswrd2,0x00,32);

  	memset(P_DevicePrms->ssIdName3,0x00,32);
  	memset(P_DevicePrms->ssIdPswrd3,0x00,32);

  	//memset(P_DevicePrms->ssIdName4,0x00,32);
  	memset(P_DevicePrms->ssIdPswrd4,0x00,16);

  	memset(P_DevicePrms->dpDvcID,0x00,16);
}
 
 u32 Get_Param_Size_Without_CRC(void)
 {
 	char* p2;
 	char* p1;
 	u32 size;

 	p1 = (char*)( &(I_DevicePrm) );
 	p2 = (char*)( &(I_DevicePrm.dpCrc) );

 	size = (p2-p1);

 	return size;
 }
