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

TIM_HandleTypeDef    Tim2Handle = {'\0'};
TIM_HandleTypeDef    Tim7Handle = {'\0'};
TIM_HandleTypeDef	 Tim6Handle = {'\0'};
TIM_HandleTypeDef	 Tim16Handle = {'\0'};

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

 //TMR 7 counter of 1 micro second//
 void TMR7_Init(void)
{
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
}
 
 //TMR 2 counter of 1 milli second//
ErrorStatus TMR2_Init(void)
{
	RCC_ClkInitTypeDef	clkconfig = {'\0'};
	u32 timclock = 0U, APB1Prescaler = 0U, prescalerValue = 0U, pFLatency = 0U;

	__HAL_RCC_TIM2_CLK_DISABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();

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

	prescalerValue = (u32) ((timclock / 1000U) - 1U);

	Tim2Handle.Instance = TIM2;
	Tim2Handle.Init.Prescaler = prescalerValue;
	Tim2Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	Tim2Handle.Init.Period = 0;

	if (HAL_TIM_Base_Init(&Tim2Handle) != HAL_OK)
	{
		Error_Handler();
	}
}


void TMR2_Set(u16 milli_seconds)
{
	TIM2->CR1 &= (u16)~TIM_CR1_CEN;

	TIM2->CNT = 0;

	TIM2->ARR = (milli_seconds);

	TIM2->EGR |= TIM_EGR_UG;

	TIM2->CR1 |= TIM_CR1_CEN;
}

void TMR2_Sleep (u16 milli_seconds)
{
	u32 inx = 0;

	TMR2_Set(milli_seconds);

	while((TIM2->CNT) < milli_seconds);
	{
		inx++;
	}
}

//TMR 6 interrupt every 100 milli//
ErrorStatus TMR6_Init(void)
{
	RCC_ClkInitTypeDef	 clkconfig = {'\0'};
	u32	timclock = 0U, APB1Prescaler = 0U, prescalerValue = 0U,  pFLatency = 0U;

	__HAL_RCC_TIM6_CLK_DISABLE();
	__HAL_RCC_TIM6_CLK_ENABLE();

	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0U);

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

	prescalerValue = (u32) ((timclock / 100000U) - 1U);

	Tim6Handle.Instance = TIM6;
	Tim6Handle.Init.Period = (1000000U / 100U) - 1U;
	Tim6Handle.Init.Prescaler = prescalerValue;
	Tim6Handle.Init.ClockDivision = 0;
	Tim6Handle.Init.CounterMode = TIM_COUNTERMODE_UP;

	if(HAL_TIM_Base_Init(&Tim6Handle) != HAL_OK)
	{
		return ERROR;
	}

	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	
	if(HAL_TIM_Base_Start_IT(&Tim6Handle) != HAL_OK)
	{
		return ERROR;
	}

	return SUCCESS;
}
 
void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&Tim6Handle);
}

 void HAL_TIM_CallBack(TIM_HandleTypeDef *htim)
{
	 if(htim->Instance == TIM6)
	{
		TMR6_Process_Handler();
	}
}

void TMR6_Process_Handler(void)
{
	static u32 count = 0;

	/*
	count++;

	if (count%2)
		HAL_GPIO_WritePin(IO_PORT_CLK_OUT, IO_PIN_CLK_OUT, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(IO_PORT_CLK_OUT, IO_PIN_CLK_OUT, GPIO_PIN_RESET);
	*/

	if (!success_code)
		return;

	if (ledFinished)
	{
		// HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
		return;
	}
	cntLed++;
	
	if(cntLed == 5)
	{
		IO_PORT_LED_RED->BRR = (u32)IO_PIN_LED_RED;
	}
	if(cntLed == 10)
	{
		cntLed = 0;
	
		IO_PORT_LED_RED->BSRR = (u32)IO_PIN_LED_RED;
		 
		cntISR++;
	
		if(cntISR >= 20)
		{
			// HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
			
			ledFinished = 1;
		}
	}
}

//TMR 16 counter of 1 milli second//
ErrorStatus TMR16_Init(void)
{
	RCC_ClkInitTypeDef	clkconfig = {'\0'};
	u32 timclock = 0U, APB2Prescaler = 0U, prescalerValue = 0U, pFLatency = 0U;

	__HAL_RCC_TIM16_CLK_DISABLE();
	__HAL_RCC_TIM16_CLK_ENABLE();

	HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

	APB2Prescaler = clkconfig.APB2CLKDivider;

	if (APB2Prescaler == RCC_HCLK_DIV1)
	{
		timclock = HAL_RCC_GetPCLK2Freq();
	}
	else
	{
		timclock = (2 * HAL_RCC_GetPCLK2Freq());
	}

	prescalerValue = (u32) ((timclock / 1000U) - 1U);

	Tim16Handle.Instance = TIM16;
	Tim16Handle.Init.Prescaler = prescalerValue;
	Tim16Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	Tim16Handle.Init.Period = 0;

	if (HAL_TIM_Base_Init(&Tim16Handle) != HAL_OK)
	{
		Error_Handler();
	}
}

void TMR16_Set(u16 milli_seconds)
{
	TIM16->CR1 &= (u16)~TIM_CR1_CEN;

	TIM16->CNT = 0;

	TIM16->ARR = (milli_seconds);

	TIM16->EGR |= TIM_EGR_UG;

	TIM16->CR1 |= TIM_CR1_CEN;
}

void TMR16_Sleep (u16 milli_seconds)
{
	u32 inx = 0;

	TMR16_Set(milli_seconds);

	while((TIM16->CNT) < milli_seconds);
	{
		inx++;
	}
}

u8 TMR16_Over(void)
{
	if(TIM16->CNT)
	{
		return 0;
	}
	else
	{
		TIM16->CR1 &= (u16)~TIM_CR1_CEN;
		
		return 1;
	}
}

void TMR6_DeInit(void)
{
	HAL_TIM_Base_DeInit(&Tim6Handle);
}
void TMR2_DeInit(void)
{
	HAL_TIM_Base_DeInit(&Tim2Handle);
}
void TMR16_DeInit(void)
{
	HAL_TIM_Base_DeInit(&Tim16Handle);
}
void TMR7_DeInit(void)
{
	HAL_TIM_Base_DeInit(&Tim7Handle);
}

