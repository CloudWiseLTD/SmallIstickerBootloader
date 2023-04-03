/*
 * RST.c
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */

#include <stddef.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "IO_Mapping.h"
#include "TMRS.h"
#include "SPI.h"
#include "UART.h"
#include "RST.h"


 void Error_Handler(void)
 {
	u32 inx = 0;

 	//while(1)
 	//{
 		inx++;
 	//}
 }

 void  Disable_Interrupts(void)
 {
	HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);//TMR6//
 }

void Disable_Per_Clocks(void)
{
	__HAL_RCC_DMA1_CLK_DISABLE();

	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();

	__HAL_RCC_USART1_CLK_DISABLE();

	 __HAL_RCC_TIM2_CLK_DISABLE();
	 __HAL_RCC_TIM6_CLK_DISABLE();
	 __HAL_RCC_TIM7_CLK_DISABLE();
	 __HAL_RCC_TIM16_CLK_DISABLE();

	 __HAL_RCC_SPI2_CLK_DISABLE();

	 __HAL_RCC_SYSCFG_CLK_DISABLE();
	 __HAL_RCC_PWR_CLK_DISABLE();
}

void GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {'\0'};

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_FLASH_RESET , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_FLASH_RESET, &GPIO_InitStructure);
	HAL_GPIO_WritePin(IO_PORT_FLASH_RESET, IO_PIN_FLASH_RESET, GPIO_PIN_SET);

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_FLASH_WP , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_FLASH_WP, &GPIO_InitStructure);
	HAL_GPIO_WritePin(IO_PORT_FLASH_WP, IO_PIN_FLASH_WP, GPIO_PIN_SET);

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_FLASH_SPI_CLK , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_FLASH_SPI_CLK, &GPIO_InitStructure);
	HAL_GPIO_WritePin(IO_PORT_FLASH_SPI_CLK, IO_PIN_FLASH_SPI_CLK, GPIO_PIN_SET);

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_FLASH_SPI_MOSI , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_FLASH_SPI_MOSI, &GPIO_InitStructure);
	HAL_GPIO_WritePin(IO_PORT_FLASH_SPI_MOSI, IO_PIN_FLASH_SPI_MOSI, GPIO_PIN_SET);

	// ???????????????
	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_LNA , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_LNA, &GPIO_InitStructure);
	HAL_GPIO_WritePin(IO_PORT_LNA, IO_PIN_LNA, GPIO_PIN_RESET);


}

void Enable_GPIO_Clocks(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_RCC_DMA1_CLK_DISABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_DMA1_FORCE_RESET();
	__HAL_RCC_DMA1_RELEASE_RESET();

	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
}

void Regulator_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct = {'\0'};

	// regulator is closed on default
	GPIO_Config_Params(&GPIO_InitStruct, GPIO_MODE_OUTPUT_PP, IO_PIN_DC_REG_EN , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_DC_REG_EN, &GPIO_InitStruct);
	HAL_GPIO_WritePin(IO_PORT_DC_REG_EN, IO_PIN_DC_REG_EN, GPIO_PIN_SET);
}

void Regulator_DeInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct = {'\0'};

	// regulator is closed on default
	GPIO_Config_Params(&GPIO_InitStruct, GPIO_MODE_OUTPUT_PP, IO_PIN_DC_REG_EN , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_DC_REG_EN, &GPIO_InitStruct);
	HAL_GPIO_WritePin(IO_PORT_DC_REG_EN, IO_PIN_DC_REG_EN, GPIO_PIN_RESET);
}

void GPS_GPIO_Close(void)
 {
	GPIO_InitTypeDef GPIO_InitStructure = {'\0'};

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_GPS_STANDBY , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_GPS_STANDBY, &GPIO_InitStructure);
	HAL_GPIO_WritePin(IO_PORT_GPS_STANDBY, IO_PIN_GPS_STANDBY, GPIO_PIN_SET);
 }


void  DeInit_Peripherals(void)
{
	SPI2_DeInit();
	
	TMR7_DeInit();
	TMR6_DeInit();
	TMR2_DeInit();
	TMR16_DeInit();

	GPIOB->MODER = 0xFFFFFFDF;
	GPIOC->MODER = 0xFFFFFFFF;
	GPIOD->MODER = 0xFFFFFFFF;
	//GPIOE->MODER = 0xFFFFFFFF;
	GPIOE->MODER = 0xFDFFFFFF;	// charger enable need to be output PE12

	// This line disconnect jtag
	// GPIOA->MODER = 0xFFFFFFFF;

	GPIO_Init();

	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();
	__HAL_RCC_GPIOA_CLK_DISABLE();

	__HAL_RCC_ADC_CLK_DISABLE();
	__HAL_RCC_DMA1_CLK_DISABLE();
}

void GPIO_Temperature(unsigned char enabled)
{
	GPIO_InitTypeDef  GPIO_InitStruct = {'\0'};

	// regulator is closed on default
	GPIO_Config_Params(&GPIO_InitStruct, GPIO_MODE_OUTPUT_PP, IO_PIN_BATTERY_TEMP_EN , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_BATTERY_TEMP_EN, &GPIO_InitStruct);

	if (enabled)
		HAL_GPIO_WritePin(IO_PORT_BATTERY_TEMP_EN, IO_PIN_BATTERY_TEMP_EN, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(IO_PORT_BATTERY_TEMP_EN, IO_PIN_BATTERY_TEMP_EN, GPIO_PIN_RESET);
}



