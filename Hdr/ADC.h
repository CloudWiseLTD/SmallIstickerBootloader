
/*
 * ADC.h
 *
 *  Created on: Mar 4, 2015
 *      Author: Michael Nesher
 */

#ifndef ADC_H_
#define ADC_H_

#define ADC_CONV_BUFF_SIZE			4
#define SOLAR_RESITOR_RATIO			0.5


typedef enum

{
	ADC_STOP_OFF = 0,
	ADC_STOP_ON 
}
eADC_StopStt;

typedef struct ADC_Msrs
{
	 int solar;
	 int vBatt;
	 int vBattPrec;
	 int vRefernce;
	 int temperature;
	 int extrnlPwr;
}
sADC_Msrs;

typedef struct ADC_Rslt
{
	float adcVBattPer;
	float adcSolar;
	float adcExtPw;
	float adcTemp;
	float adcVBatt;
	float adcSolarV;
}
sADC_Rslt;


void ADC1_Init(void);
void ADC1_DeInit(void);
void DMA1_CH1_DeInit(void);
sADC_Msrs* ActivateScan(void);


#endif /* ADC_H_ */
