/*
 * TMRS.h
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */

#include "IO_Mapping.h"

#ifndef TMRS_H_
#define TMRS_H_

void TMR7_Set(u16 micro_seconds);
void TMR7_Sleep (u16 micro_seconds);
void TMR7_Init(void);

ErrorStatus TMR2_Init(void);
void TMR2_Set(u16 milli_seconds);
void TMR2_Sleep (u16 milli_seconds);

ErrorStatus TMR6_Init(void);
void TMR6_Process_Handler(void);

ErrorStatus TMR16_Init(void);
void TMR16_Set(u16 milli_seconds);
void TMR16_Sleep (u16 milli_seconds);
u8 TMR16_Over(void);

#ifndef REMOVE_TIMERS
void HAL_TIM_CallBack(TIM_HandleTypeDef *htim);
#endif

void TMR6_DeInit(void);
void TMR2_DeInit(void);
void TMR16_DeInit(void);
void TMR7_DeInit(void);


#endif /* TMRS_H_ */
