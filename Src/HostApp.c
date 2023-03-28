/*
 * HostApp.c
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */
 
#include <stddef.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "UARTDrv.h"
#include "TMRS.h"
#include "Common.h"
#include "HostApp.h"

u8 GetKey(void)
{
	u8 key = 0;

	while(1)
	{
		if (SerialKeyPressed((u8*)&key))
		{
			 break;
		}
	}

	return key;
}

u8 Get_Key_TimOut(void)
{
	u8 key = 0;

	while(1)
	{
		if(SerialKeyPressed((u8*)&key))
		{
 			break;
		}

		if(TMR16_Over() == 1)
		{
			SerialPutString("Password Time Out is Expired. BootLoader Not Burn New Application\n\r");

			Going_To_APP();
		}
	}

	return key;
}

void Serial_PutString(u8* s)
{
	while (*s != '\0')
	{
		SerialPutChar(*s);

		s++;
	}

	WaitForEndTx();
}


