/*
 * LED.c
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */

#include <stddef.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "IO_Mapping.h"
#include "LED.h"

 void Led_GPIO_Init(void)
 {
	GPIO_InitTypeDef GPIO_InitStructure = {'\0'};

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_LED_RED , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_LED_RED, &GPIO_InitStructure);
	HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_RED, GPIO_PIN_SET);

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_LED_GREEN , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_LED_GREEN, &GPIO_InitStructure);
	HAL_GPIO_WritePin(IO_PORT_LED_GREEN, IO_PIN_LED_GREEN, GPIO_PIN_SET);

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_LED_BLUE , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_LED_BLUE, &GPIO_InitStructure);
	HAL_GPIO_WritePin(IO_PORT_LED_BLUE, IO_PIN_LED_BLUE, GPIO_PIN_SET);

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_CLK_OUT , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_CLK_OUT, &GPIO_InitStructure);

	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_CHARGE_EN , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_CHARGE_EN, &GPIO_InitStructure);

	/*
	GPIO_Config_Params(&GPIO_InitStructure, GPIO_MODE_OUTPUT_PP, IO_PIN_BUZZER , GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	HAL_GPIO_Init(IO_PORT_BUZZER, &GPIO_InitStructure);
	HAL_GPIO_WritePin(IO_PORT_BUZZER, IO_PIN_BUZZER, GPIO_PIN_SET);
	*/
 }

 void BlinkLed(u8 led_code)
 {
	 // default: 0 - RED
	 u32 port = IO_PORT_LED_RED;
	 u32 pin = IO_PIN_LED_RED;

	 if (led_code == 1) // green
	 {
		 port = IO_PORT_LED_GREEN;
		 pin = IO_PIN_LED_GREEN;
	 }
	 else if (led_code == 2) // blue
	 {
		 port = IO_PORT_LED_BLUE;
		 pin = IO_PIN_LED_BLUE;
	 }

	 HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
	 HAL_Delay(100*3);
	 HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
 }


void GPIO_Config_Params(GPIO_InitTypeDef * P_gpio, u32 mode,u32 pin,u32 pull,u32 speed)
{
	P_gpio->Mode=mode;
	P_gpio->Pin=pin;
	P_gpio->Pull=pull;
	P_gpio->Speed=speed;
}

 void GPIO_Config_Params_AF(GPIO_InitTypeDef * P_gpio, u32 mode,u32 pin,u32 pull,u32 speed, u32 alternate)
 {
	 P_gpio->Mode=mode;
	 P_gpio->Pin=pin;
	 P_gpio->Pull=pull;
	 P_gpio->Speed=speed;
	 P_gpio->Alternate=alternate;
 }

