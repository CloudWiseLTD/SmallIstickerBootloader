/*
 * ADC.c
 *
 *  Created on: Mar 4, 2015
 *      Author: Michael Nesher
 */


#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "IO_Mapping.h"
#include "ADC.h"

//ADC structs//
ADC_HandleTypeDef			AdcHandle = {'\0'};
ADC_ChannelConfTypeDef    	sConfig = {'\0'};
DMA_HandleTypeDef		 	I_dma1Cnl1 = {'\0'};

sADC_Msrs measurements;


//Global Arrays//
unsigned short  ADC1CnvDat[(ADC_CONV_BUFF_SIZE + 4)] = {'\0'};

GPIO_InitTypeDef GPIO_InitStructure = {'\0'};
RCC_PeriphCLKInitTypeDef PeriphClkInit = {'\0'};

#define EVENT_SAMPLE_COMPLETED		(1<<0)


static char adc_Sample_completed = 0;

sADC_Msrs* ActivateScan(void)
{
	unsigned count = 10000;

	sADC_Msrs* ptr = NULL;

	adc_Sample_completed = 0;

	// get sample
	if (HAL_ADC_Start_DMA(&AdcHandle, (unsigned*)ADC1CnvDat, ADC_CONV_BUFF_SIZE) != HAL_OK)
	{
		//Error_Handler();//
	}

	while (count > 0)
	{
		if (adc_Sample_completed)
		{
			ptr = &measurements;
			break;
		}
		count--;
	}

	return ptr;
}



void ADC1_Init(void)
{
	__HAL_RCC_ADC_CLK_DISABLE();
	__HAL_RCC_ADC_CLK_ENABLE();
	__HAL_RCC_ADC_FORCE_RESET();
	__HAL_RCC_ADC_RELEASE_RESET();
		
	__HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_SYSCLK);
	
	HAL_GPIO_DeInit(IO_PORT_SOLAR_VOLTAGE, IO_PIN_SOLAR_VOLTAGE);
	HAL_GPIO_DeInit(IO_PORT_BATTERY_VOLTAGE, IO_PIN_BATTERY_VOLTAGE);
	HAL_GPIO_DeInit(IO_PORT_USB_BUS, IO_PIN_USB_BUS);
	
	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_ANALOG_ADC_CONTROL, IO_PIN_SOLAR_VOLTAGE , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_SOLAR_VOLTAGE, &GPIO_InitStructure);

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_ANALOG_ADC_CONTROL, IO_PIN_BATTERY_VOLTAGE , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_BATTERY_VOLTAGE, &GPIO_InitStructure);
	
	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_ANALOG_ADC_CONTROL, IO_PIN_BATTERY_TEMP , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PIN_BATTERY_TEMP, &GPIO_InitStructure);

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_INPUT, IO_PIN_USB_BUS , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_USB_BUS, &GPIO_InitStructure);

	I_dma1Cnl1.Instance = DMA1_Channel1;
	
	if(HAL_DMA_DeInit(&I_dma1Cnl1) != HAL_OK)
	{
		Error_Handler();
	}
	
	I_dma1Cnl1.Init.Request 			    	= DMA_REQUEST_0;
	I_dma1Cnl1.Init.Direction			   		= DMA_PERIPH_TO_MEMORY;
	I_dma1Cnl1.Init.PeriphInc		   			= DMA_PINC_DISABLE;
	I_dma1Cnl1.Init.MemInc			   			= DMA_MINC_ENABLE;
	I_dma1Cnl1.Init.PeriphDataAlignment  		= DMA_PDATAALIGN_HALFWORD;
	I_dma1Cnl1.Init.MemDataAlignment	   		= DMA_MDATAALIGN_HALFWORD;
	I_dma1Cnl1.Init.Mode			   			= DMA_CIRCULAR;
	I_dma1Cnl1.Init.Priority					= DMA_PRIORITY_LOW;
	
	if (HAL_DMA_Init(&I_dma1Cnl1) != HAL_OK)
	{
		Error_Handler();
	}
			
	AdcHandle.Instance	= ADC1;
	
	if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
	{
		Error_Handler();
	}
	
	// AdcHandle.Init.ClockPrescaler			= ADC_CLOCK_SYNC_PCLK_DIV4;		/* Synchronous clock mode, input ADC clock divided by 2*/

	AdcHandle.Init.ClockPrescaler			= ADC_CLOCK_SYNC_PCLK_DIV1;
	
	AdcHandle.Init.Resolution				= ADC_RESOLUTION_12B;		  	/* 12-bit resolution for converted data */
	
	AdcHandle.Init.DataAlign				= ADC_DATAALIGN_RIGHT;		  	/* Right-alignment for converted data */
	
	AdcHandle.Init.ScanConvMode 			= ADC_SCAN_ENABLE; 			 	/* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	
	AdcHandle.Init.EOCSelection 			= ADC_EOC_SINGLE_CONV;		  	/* EOC flag picked-up to indicate conversion end */
	
	AdcHandle.Init.LowPowerAutoWait 		= DISABLE;					  	/* Auto-delayed conversion feature disabled */
	
	AdcHandle.Init.ContinuousConvMode		= DISABLE;					  	/* Continuous mode enabled (automatic conversion restart after each conversion) */
	
	AdcHandle.Init.NbrOfConversion		 	= 4;							/* Parameter discarded because sequencer is disabled */
	
	AdcHandle.Init.DiscontinuousConvMode	= ENABLE;					  	/* Parameter discarded because sequencer is disabled */
	
	AdcHandle.Init.NbrOfDiscConversion		= 4;							/* Parameter discarded because sequencer is disabled */
	
	AdcHandle.Init.ExternalTrigConv 		= ADC_SOFTWARE_START;			/* Software start to trig the 1st conversion manually, without external event */
	
	AdcHandle.Init.ExternalTrigConvEdge 	= ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
	
	AdcHandle.Init.DMAContinuousRequests	= DISABLE;					  	/* ADC DMA continuous request to match with DMA circular mode */
	
	AdcHandle.Init.Overrun					=  ADC_OVR_DATA_PRESERVED;	  	/* DR register is overwritten with the last conversion result in case of overrun */
	
	AdcHandle.Init.OversamplingMode 		= DISABLE;					  	/* No oversampling */

	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
	{
		Error_Handler();
	}
	
	if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) !=  HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Rank			= ADC_REGULAR_RANK_1; 	 		/* Rank of sampled channel number ADCx_CHANNEL */
	// sConfig.SamplingTime 	= ADC_SAMPLETIME_12CYCLES_5; 	/* Sampling time (number of clock cycles unit) */
	sConfig.SamplingTime 	= ADC_SAMPLETIME_92CYCLES_5;
	sConfig.SingleDiff	  	= ADC_SINGLE_ENDED;				/* Single-ended input channel */
	sConfig.OffsetNumber 	= ADC_OFFSET_NONE; 				/* No offset subtraction */
	sConfig.Offset		  	= 0;							/* Parameter discarded because offset correction is disabled */
	sConfig.Channel 	 	= ADC_CHANNEL_5;
	
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	
	sConfig.Rank		= ADC_REGULAR_RANK_2;
	sConfig.Channel 	= ADC_CHANNEL_6;
	
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	// ALON: 04-01-2023 external temperature instead of CPU temperature
	sConfig.Rank		  	= ADC_REGULAR_RANK_3;
	//sConfig.Channel 	 	= ADC_CHANNEL_17;
	sConfig.Channel 	 	= ADC_CHANNEL_11;
	
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	
	sConfig.Rank			= ADC_REGULAR_RANK_4;
	sConfig.Channel 	 	= ADC_CHANNEL_VREFINT;
	
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	
	//Associate the DMA handle//
	AdcHandle.DMA_Handle = &I_dma1Cnl1;
	I_dma1Cnl1.Parent = &AdcHandle;
	
	/* NVIC configuration for DMA Input data interrupt */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}


void DMA1_Channel1_IRQHandler(void)
{	
	HAL_DMA_IRQHandler(&I_dma1Cnl1);

	// measurement received
}




void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	static unsigned count = 0;
	if (hadc->Instance == ADC1)
	{
		count++;

		measurements.solar = (int)ADC1CnvDat[0];
		measurements.vBatt = (int)ADC1CnvDat[1];
		measurements.temperature = (int)ADC1CnvDat[2];
		measurements.vRefernce = (int)ADC1CnvDat[3];

		adc_Sample_completed = 1;
	}
}


void ADC1_DeInit(void)
{
	HAL_ADC_DeInit(&AdcHandle);
}

void DMA1_CH1_DeInit(void)
{
	HAL_DMA_DeInit(&I_dma1Cnl1); 
}
