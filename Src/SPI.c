/*
 * SPI.c
 *
 *  Created on: May 7, 2017
 *      Author: Michael
 */

#include <stddef.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "IO_Mapping.h"
#include "RST.h"
#include "LED.h"
#include "SPI.h"

SPI_HandleTypeDef SPI2_Handle = {'\0'};

void SPI2_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct = {'\0'};

	__HAL_RCC_SPI2_CLK_DISABLE();
	__HAL_RCC_SPI2_CLK_ENABLE();
	__HAL_RCC_SPI2_FORCE_RESET();
	__HAL_RCC_SPI2_RELEASE_RESET();

	SPI2_Handle.Instance = SPI2;

	if(HAL_SPI_DeInit(&SPI2_Handle)  != HAL_OK)
	{
		Error_Handler();
	}

	SPI2_Handle.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2;
	SPI2_Handle.Init.CLKPhase=SPI_PHASE_2EDGE;
	SPI2_Handle.Init.CLKPolarity=SPI_POLARITY_HIGH;
	SPI2_Handle.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;
	SPI2_Handle.Init.CRCLength=SPI_CRC_LENGTH_8BIT;
	SPI2_Handle.Init.CRCPolynomial=7;
	SPI2_Handle.Init.DataSize=SPI_DATASIZE_8BIT;
	SPI2_Handle.Init.Direction=SPI_DIRECTION_2LINES;
	SPI2_Handle.Init.FirstBit=SPI_FIRSTBIT_MSB;
	SPI2_Handle.Init.Mode=SPI_MODE_MASTER;
	SPI2_Handle.Init.NSS=SPI_NSS_SOFT;
	SPI2_Handle.Init.NSSPMode=SPI_NSS_PULSE_DISABLE;
	SPI2_Handle.Init.TIMode=SPI_TIMODE_DISABLE;

	if(HAL_SPI_Init(&SPI2_Handle) != HAL_OK)
	{
	  	Error_Handler();
	}

	__HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_GPIO_DeInit(IO_PORT_FLASH_SPI_CS,IO_PIN_FLASH_SPI_CS);
	HAL_GPIO_DeInit(IO_PORT_FLASH_SPI_CLK,IO_PIN_FLASH_SPI_CLK);
	HAL_GPIO_DeInit(IO_PORT_FLASH_SPI_MISO,IO_PIN_FLASH_SPI_MISO);
	HAL_GPIO_DeInit(IO_PORT_FLASH_SPI_MOSI,IO_PIN_FLASH_SPI_MOSI);

	GPIO_Config_Params(&GPIO_InitStruct, GPIO_MODE_OUTPUT_PP, IO_PIN_FLASH_SPI_CS, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_FLASH_SPI_CS, &GPIO_InitStruct);

	 HAL_GPIO_WritePin(IO_PORT_FLASH_SPI_CS, IO_PIN_FLASH_SPI_CS, GPIO_PIN_SET);

	GPIO_Config_Params_AF(&GPIO_InitStruct, GPIO_MODE_AF_PP, IO_PIN_FLASH_SPI_CLK, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF5_SPI2);
	HAL_GPIO_Init(IO_PORT_FLASH_SPI_CLK, &GPIO_InitStruct);

	GPIO_Config_Params_AF(&GPIO_InitStruct, GPIO_MODE_AF_PP, IO_PIN_FLASH_SPI_MISO, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF5_SPI2);
	HAL_GPIO_Init(IO_PORT_FLASH_SPI_MISO, &GPIO_InitStruct);

	GPIO_Config_Params_AF(&GPIO_InitStruct, GPIO_MODE_AF_PP, IO_PIN_FLASH_SPI_MOSI, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF5_SPI2);
	HAL_GPIO_Init(IO_PORT_FLASH_SPI_MOSI, &GPIO_InitStruct);

	SPI2_Handle.Instance->CR1 |= (1<<6);
}

void SPI2_DeInit(void)
{
	HAL_SPI_DeInit(&SPI2_Handle);
}

