/*
 * Common.c
 *
 *  Created on: May 8, 2017
 *      Author: Michael Nesher
 */

#include <stddef.h>
#include "string.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "FlashInnApp.h"
#include "RST.h"
#include "Download.h"
#include "HostApp.h"
#include "TMRS.h"
#include "Common.h"

pFunction Jump_To_Application;

uint32_t JumpAddress;

extern u8 ledFinished;
extern u8 success_code;

void BlinkLed(u8 led_code);

/**
  * @brief  Convert an Integer to a string
  * @param  str: The string
  * @param  intnum: The intger to be converted
  * @retval None
  */
void Int2Str(uint8_t* str, int32_t intnum)
{
	uint32_t i, Div = 1000000000, j = 0, Status = 0;

	for (i = 0; i < 10; i++)
	{
		str[j++] = (intnum / Div) + 48;

		intnum = intnum % Div;

		Div /= 10;

		if ((str[j-1] == '0') & (Status == 0))
		{
			j = 0;
		}
		else
		{
			Status++;
		}
	}
}

/**
  * @brief  Convert a string to an integer
  * @param  inputstr: The string to be converted
  * @param  intnum: The intger value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
	uint32_t i = 0, res = 0;
	uint32_t val = 0;

	if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
	{
		if (inputstr[2] == '\0')
		{
			return 0;
		}

		for (i = 2; i < 11; i++)
		{
			if(inputstr[i] == '\0')
			{
				*intnum = val;

				/* return 1; */

				res = 1;

				break;
			}
			if (ISVALIDHEX(inputstr[i]))
			{
				val = (val << 4) + CONVERTHEX(inputstr[i]);
			}
			else
			{
				/* return 0, Invalid input */

				res = 0;

				break;
			}
		}

		/* over 8 digit hex --invalid */
		if (i >= 11)
		{
			res = 0;
		}
	}

	else /* max 10-digit decimal input */
	{
		for (i = 0;i < 11;i++)
		{
			if(inputstr[i] == '\0')
			{
				*intnum = val;

				/* return 1 */

				res = 1;

				break;
			}
			else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
			{
				val = val << 10;

				*intnum = val;

				res = 1;

				break;
			}

			else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
			{
				val = val << 20;

				*intnum = val;

				res = 1;

				break;
			}
			else if (ISVALIDDEC(inputstr[i]))
			{
				val = val * 10 + CONVERTDEC(inputstr[i]);
			}
			else
			{
				/* return 0, Invalid input */

				res = 0;

				break;
			}
		}

		/* Over 10 digit decimal --invalid */
		if (i >= 11)
		{
			res = 0;
		}
	}

	return res;
}


void Going_To_APP(void)
{
	u32 inx = 0;

	WD_Refresh();
	ledFinished = 1;
	Led_GPIO_Init();

	TMR2_Sleep(2000*10);

	if (success_code==1)
		BlinkLed(0);	// error - red blink
	else if (success_code==2)
		BlinkLed(1);	// success - green blink

	TMR2_Sleep(1000*10);
	AllLights();
	TMR2_Sleep(2000*10);
	Led_GPIO_Init();

	HAL_FLASH_Lock();

	Disable_Interrupts();

	// DeInit_Peripherals();
	
	HAL_NVIC_DisableIRQ(SysTick_IRQn);
	SysTick->LOAD  = (u32)(0UL);						  				/* set reload register */
	SysTick->VAL   = 0UL;											  	/* Load the SysTick Counter Value */
	SysTick->CTRL  = 0;	
	
	Disable_Per_Clocks();

	JumpAddress = (uint32_t) *((__IO uint32_t*)INNFLS_STR_APP_ADD);

	/*Get the application entry point (Second entry in the application vector table)*/

	Jump_To_Application = (pFunction) *(__IO uint32_t*) (INNFLS_STR_APP_ADD + 4);

	/* Reconfigure vector table offset register to match the application location */

	SCB->VTOR = INNFLS_STR_APP_ADD;

	/*Set the application stack pointer */

	__set_MSP(JumpAddress);

	/*Start the application */

	Jump_To_Application();
}


