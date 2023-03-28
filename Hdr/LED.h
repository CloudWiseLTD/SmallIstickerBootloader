/*
 * LED.h
 *
 *  Created on: May 7, 2017
 *      Author: Michael
 */

#ifndef LED_H_
#define LED_H_

void Led_GPIO_Init(void);
void BlinkLed(u8 led_code);
void GPIO_Config_Params(GPIO_InitTypeDef * P_gpio, u32 mode,u32 pin,u32 pull,u32 speed);
void GPIO_Config_Params_AF(GPIO_InitTypeDef * P_gpio, u32 mode,u32 pin,u32 pull,u32 speed, u32 alternate);

#endif /* LED_H_ */
