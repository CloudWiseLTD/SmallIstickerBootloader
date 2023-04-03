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


void Going_To_APP(void)
{
	WD_Refresh();
	Led_GPIO_Init();

	HAL_Delay(1000);

	AllLights();
	HAL_Delay(200);
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


