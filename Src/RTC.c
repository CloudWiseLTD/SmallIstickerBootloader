/*
 * RTC.c
 *
 *  Created on: Jul 16, 2015
 *      Author: Michael Nesher
 */

//Includes//
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "RTC.h"

RTC_HandleTypeDef I_RtcHandle 		= {'\0'};

unsigned rtc_count = 0;



HAL_StatusTypeDef RTC_WkUp_Init(void)
{
	HAL_StatusTypeDef status = HAL_OK;

	HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);	
	
	if (HAL_RTCEx_SetWakeUpTimer_IT(&I_RtcHandle, TICKS_IN_SECONDS*(RTC_WAKEUP_INTERRUPT), RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
	{
	  	status = Error_Handler();
	}
	
	HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
	
	return status;
}

void RTC_WKUP_IRQHandler(void)
{
	HAL_RTCEx_WakeUpTimerIRQHandler(&I_RtcHandle);
}


void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	rtc_count++;
}


HAL_StatusTypeDef  RTC_Init(void)
{
	HAL_StatusTypeDef status = HAL_OK;

	RTC_TimeTypeDef IL_RTCTime = {'\0'};
	RTC_DateTypeDef IL_RTCDate = {'\0'};

	__HAL_RCC_RTC_DISABLE();
	__HAL_RCC_RTC_ENABLE();


	RCC_OscInitTypeDef RCC_OscInitLSE;

	RCC_OscInitLSE.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitLSE.LSEState = RCC_LSE_ON;

	if (HAL_RCC_OscConfig(&RCC_OscInitLSE) != HAL_OK)
	{
		status = HAL_ERROR;
		Error_Handler();
	}

	HAL_PWR_EnableBkUpAccess();

	// pick LSE as source for RTC
	RCC->BDCR &= ~(RCC_BDCR_RTCSEL_0|RCC_BDCR_RTCSEL_1);
	RCC->BDCR |= (RCC_BDCR_RTCEN);
	RCC->BDCR |= (RCC_BDCR_RTCSEL_0);
	HAL_Delay(10);

	I_RtcHandle.Instance = RTC;

	 
	if ( HAL_RTC_DeInit(&I_RtcHandle) != HAL_OK)
	{
		status = HAL_ERROR;
		Error_Handler();
	}

	   
	I_RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
	I_RtcHandle.Init.AsynchPrediv = 127;
	I_RtcHandle.Init.SynchPrediv = 255;
	I_RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
	I_RtcHandle.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
	I_RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	I_RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	 
	if ( HAL_RTC_Init(&I_RtcHandle) != HAL_OK)
	{
		status = HAL_ERROR;
		Error_Handler();
	}
	 
	status = RTC_WkUp_Init();

	/*
	if (HAL_RTCEx_SetCalibrationOutPut(&I_RtcHandle, RTC_CALIBOUTPUT_512HZ) != HAL_OK )
	{
		status = HAL_ERROR;
		Error_Handler();
	}
	*/

	// HAL_PWR_DisableBkUpAccess();

	return status;
 }



