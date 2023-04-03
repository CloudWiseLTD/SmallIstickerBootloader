/*
 * TMRS.c
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */

#include <stddef.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "IO_Mapping.h"
#include "RST.h"
#include "TMRS.h"

TIM_HandleTypeDef    Tim7Handle = {'\0'};

u8 cntLed = 0;
u8 cntISR = 0;
u8 ledFinished = 0;

extern u8 success_code;

void TMR7_Set(u16 micro_seconds)
{
	TIM7->CR1 &= (u16)~TIM_CR1_CEN;

	TIM7->CNT = 0;

	TIM7->ARR = (micro_seconds);

	TIM7->EGR |= TIM_EGR_UG;

	TIM7->CR1 |= TIM_CR1_CEN;
}

void TMR7_Sleep (u16 micro_seconds)
{
	u32 inx = 0;

	TMR7_Set(micro_seconds);

	while((TIM7->CNT) < micro_seconds);
	{
		inx++;
	}
}

void TMR7_Init(void)
{
#ifndef REMOVE_TIMERS
	RCC_ClkInitTypeDef	clkconfig = {'\0'};
	u32 timclock = 0U, APB1Prescaler = 0U, prescalerValue = 0U,	pFLatency = 0U;

	__HAL_RCC_TIM7_CLK_DISABLE();
	__HAL_RCC_TIM7_CLK_ENABLE();

	HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

	APB1Prescaler = clkconfig.APB1CLKDivider;

	if (APB1Prescaler == RCC_HCLK_DIV1)
	{
	timclock = HAL_RCC_GetPCLK1Freq();
	}
	else
	{
	timclock = (2 * HAL_RCC_GetPCLK1Freq());
	}

	prescalerValue = (u32) ((timclock / 1000000U) - 1U);

	Tim7Handle.Instance = TIM7;
	Tim7Handle.Init.Prescaler = prescalerValue;
	Tim7Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	Tim7Handle.Init.Period = 0;

	if (HAL_TIM_Base_Init(&Tim7Handle) != HAL_OK)
	{
		Error_Handler();
	}
#endif
}


void TMR7_DeInit(void)
{
#ifndef REMOVE_TIMERS
	HAL_TIM_Base_DeInit(&Tim7Handle);
#endif
}

