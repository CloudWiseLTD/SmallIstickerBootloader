/*
 * Download.c
 *
 *  Created on: May 8, 2017
 *      Author: Michael Nesher
 */
 
#include <stddef.h>
#include <string.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "Ymodem.h"
#include "HostApp.h"
#include "DvcPrm.h"
#include "FlashInnApp.h"
#include "Common.h"
#include "Download.h"
#include "IO_Mapping.h"

#ifndef REMOVE_DOWNLOAD

 extern uint8_t file_name[FILE_NAME_LENGTH];
 
 extern uint16_t appVerNum;
 extern uint16_t appBldNum;
 
 uint8_t tab_1024[1024] = {0};
 
 extern sDevice_Params	 I_DevicePrm;
 
 int32_t SerialDownload(void)
 {
	 uint8_t Number[10] = " 	   \0";
	 
	 int32_t Size = 0;
	 
	 uint32_t Address=0, FlashChecksum=0;
 
	 SerialPutString("Waiting for the file to be sent ... (press 'a' to abort)\n\r");
	 WD_Refresh();
	 
	 Size = Ymodem_Receive(&tab_1024[0]);
 
	 if (Size > 0)
	 {
		 SerialPutString("\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
		 
		 SerialPutString(file_name);
		 
		 Int2Str(Number, Size);
		 
		 Number[9] =  '\0';
		 
		 SerialPutString("\n\r Size: ");
		 
		 SerialPutString(Number);
		 
		 SerialPutString(" Bytes\r\n");
		 
		 for (Address = INNFLS_STR_APP_ADD; Address <= INNFLS_STR_APP_ADD+Size ; Address += 4)
		 {
			 FlashChecksum+=(*(vu32*) Address);
		 }
		 
		 memset(Number,' ',sizeof(uint8_t)*10);
		 
		 Int2Str(Number, (FlashChecksum&0xFFFF));
		 
		 Number[9]= '\0';
		 
		 SerialPutString(" Checksum: ");
		 
		 SerialPutString(Number);
		 
		 SerialPutString("\r\n-------------------\n");
 
		 SerialPutString("Update Version and Build Parameters\n\r");
		 
		 I_DevicePrm.dpVersion = appVerNum;
		 I_DevicePrm.dpVersionR = appVerNum;
		 I_DevicePrm.dpBuild = appBldNum;
		 I_DevicePrm.dpBuildR = appBldNum;
 
		 SerialPutString("Program Parameters into Ext Flash\n\r");
		 
		 if(DevParms_Burn_Flash(&I_DevicePrm) == SUCCESS)
		 {
			 SerialPutString("Program Parameters into Ext Flash Success\n\r");
		 }
		 else
		 {
			 SerialPutString("Program Parameters into Ext Flash Failed\n\r");
			 
			 Size = -4;
		 }
	 }
	 
	 else if (Size == -1)
	 {
		 SerialPutString("\n\n\rThe image size is higher than the allowed space memory!\n\r");
	 }
	 else if (Size == -2)
	 {
		 SerialPutString("\n\n\rVerification failed!\n\r");
	 }
	 else if (Size == -3)
	 {
		 SerialPutString("\r\n\nAborted by user.\n\r");
	 }
	 else
	 {
		 SerialPutString("\n\rFailed to receive the file!\n\r");
	 }
 
	 return Size;
  }

#endif
