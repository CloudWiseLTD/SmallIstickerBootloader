/*
 * RST.h
 *
 *  Created on: May 7, 2017
 *      Author: Michael
 */

#ifndef RST_H_
#define RST_H_

void Error_Handler(void);
void Disable_Interrupts(void);
void Disable_Per_Clocks(void);
void DeInit_Peripherals(void);
void Enable_GPIO_Clocks(void);
void Charger_GPIO_Init(void);

#endif /* RST_H_ */
