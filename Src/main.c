/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include <stddef.h>
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"

#include "Defines.h"
#include "IO_Mapping.h"
#include "RST.h"
#include "LED.h"
#include "SPI.h"
#include "FlashDrv.h"
#include "FlashApp.h"
#include "TMRS.h"
#include "UART.h"
#include "UARTDrv.h"
#include "DvcPrm.h"
#include "HostApp.h"
#include "FlashInnApp.h"
#include "Common.h"
#include "RTC.h"
#include "main.h"

#define B_FACTOR		3380
#define R16				10
#define R0				10
#define T25				295.0
#define T0				273.0


/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
#define DATA_BUFFER_SIZE			1024
#define TERMINAL_PASSWORD			0x2946
#define FLASH_ERASE_VALUE			0xFFFF

u8 glbDatBuf[DATA_BUFFER_SIZE + 2] = {'\0'};
u16 glbCrc16 = 0;

u8 general_str[256] = {'\0'};

void init_charge_tables(void);

#include "ADC.h"

extern u8 ledFinished;
extern unsigned rtc_count;
extern sDevice_Params I_DevicePrm;
extern sADC_Msrs measurements;

double charging_table[NUM_CHARGE_MODES];
double current_charging_capacity;
u8 charger_resistors_value = 0;
u8 success_code = 0;

// watch dog //
void WD_Init(void);
void WD_Refresh(void);
void CheckVoltages(void);
void AllLights(void);
void GPS_GPIO_Close(void);

IWDG_HandleTypeDef I_WD;
sADC_Rslt measurements_res;

static HAL_StatusTypeDef hal_status;

/*
 * byte 0: boot loader major version
 * byte 1: boot loader minor version
 * byte 2: Hardware type
 */

#ifndef  REMOVE_BOOT_CONSTANTS

static const unsigned char __attribute__((section (".boot_constants")))
	boot_constants[16] = { 4, 46, HARDWARE_TYPE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#endif

int measure_count = 0;

/*
void PrintVersion(void)
{
	SerialPutString("\n\rCar Sticker Boot Loader Version 4.46 (no reg)");
}
*/


int main(void)
{
 	uint32_t inx = 0; //,systemClock = 0;
	uint8_t tmp8 = 0;
	uint32_t param_address = 0;



	charger_resistors_value = 0;

	HAL_Init();

	hal_status = HAL_OK;

 	// DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_IWDG_STOP;
	DBGMCU->APB1FZR1 &= ~DBGMCU_APB1FZR1_DBG_IWDG_STOP;


	#ifdef ENABLE_WATCHDOG
		WD_Init();
		WD_Refresh();
	#endif

	HAL_Delay(3000); // ??????????


	Enable_GPIO_Clocks();

	GPIO_Init();

	TMR2_Init();

	//what the purpose of this line ?
	//TMR2_Sleep(100);
	//Regulator_Init();
	//TMR2_Sleep(100);
	Regulator_DeInit();
	TMR2_Sleep(100);

	#ifdef UART_DEBUG
	#ifndef REMOVE_UART
	USART1_Init();
	#endif
	#endif


	hal_status = RTC_Init();

#ifndef  REMOVE_ADC
	ADC1_Init();
#endif

	Led_GPIO_Init();
	GPS_GPIO_Close();

	// the test is moved to the application
#ifndef  REMOVE_ADC
	CheckVoltages();
#endif

	SystemClock_Config();

	HAL_FLASH_Lock();

	HAL_FLASH_Unlock();


	SPI2_Init();
	TMR7_Init();
	TMR6_Init();
	TMR16_Init();

	#ifndef UART_DEBUG
	USART1_Init();
	#endif
	
	Regulator_Init();

	memset(&I_DevicePrm,0 , sizeof(I_DevicePrm));
	
	// TMR2_Sleep(25);
	// TMR2_Sleep(1000);
	
	ErrorStatus error = Flash_Init();

	BlinkSequence();

	/*
	// SerialPutString("\n\rCar Sticker Boot Loader Version 4.44 (no reg)");
	PrintVersion();

#ifdef V4_REV2
	SerialPutString(" V4-R2 HW3\r\n");
#else
	#ifdef V5_REV9
		SerialPutString(" V5 HW5\r\n");
	#else
		#ifdef V6_REV0
			SerialPutString(" V6-R0 HW2\r\n");
		#else
			SerialPutString(" V3A-R4 HW2\r\n");
		#endif
	#endif
#endif
	*/

	// check voltage

	// SerialPutString("Reading Parameters...");

	if (DevParms_Read_Flash(&I_DevicePrm, (u32)param_address) == SUCCESS)
	{
		// SerialPutString(" - Success\r\n");

		BlinkSequence();

		if (I_DevicePrm.dpVersion == FLASH_ERASE_VALUE)
		{
			// SerialPutString("No Application Found\r\n");

			while(1)
			{
				TMR2_Sleep(3000);
			}

			/*
			SerialPutString("Start Terminal Burn New Application\n\r");

			while(1)
			{
				SerialPutString("Please Enter Password to Burn New Application\n\r");

				tmp8 = GetKey();
				tmp16|=(tmp8 - 0x30);
				tmp16 <<= 4;
				tmp8 = GetKey();
				tmp16|= (tmp8 - 0x30);
				tmp16 <<= 4;
				tmp8 = GetKey();
				tmp16 |= (tmp8 - 0x30);
				tmp16 <<= 4;
				tmp8 = GetKey();
				tmp16 |= (tmp8 - 0x30);
			
				if (tmp16 == TERMINAL_PASSWORD)
				{
					Main_Menu_NoApp();
				}
				else
				{
					tmp8 = 0;
					tmp16 = 0;
					
					SerialPutString("Password Entered is Wrong\n\r");
				}
			}
			*/
		}
		else
		{
			/*
			sprintf( general_str, "application version: %d.%d.%d\r\n" , (I_DevicePrm.dpVersion / 256), (I_DevicePrm.dpVersion % 256),  I_DevicePrm.dpBuild);
			SerialPutString(general_str);
			*/
			// SerialPutString("Application is in Inner Flash\n\r");
			
			// debugging only: force new version
			// I_DevicePrm.dpVerRdLn = 200000;

			if (I_DevicePrm.dpVerRdLn == 0)
			{
				// SerialPutString("No new firmware found. run the current firmware\r\n");

				// TMR2_Sleep(3000);
				Going_To_APP();

				/*
				SerialPutString("Start Local Burn New Application\n\r");
				
				//TMR16_Set(5000);
				TMR16_Set(500);
					
				while(1)
				{
					SerialPutString("Please Enter Password to Burn New Application\n\r");
				
					tmp8 = Get_Key_TimOut();
					tmp16|=(tmp8 - 0x30);
					tmp16 <<= 4;
					tmp8 = Get_Key_TimOut();
					tmp16|= (tmp8 - 0x30);
					tmp16 <<= 4;
					tmp8 = Get_Key_TimOut();
					tmp16 |= (tmp8 - 0x30);
					tmp16 <<= 4;
					tmp8 = Get_Key_TimOut();
					tmp16 |= (tmp8 - 0x30);
				
					if (tmp16 == TERMINAL_PASSWORD)
					{
						Main_Menu();
					}
					else
					{
						TMR16_Set(5000);	
						
						SerialPutString("Password Entered is Wrong\n\r");
					}
				}

				*/
			}
			else
			{
				// indicate trying to burn new firmware by blinking led
				success_code = 1;

				/*
				sprintf( general_str, "new firmware: %d.%d.%d - size: %d\r\n" , (I_DevicePrm.dpVersionR / 256), (I_DevicePrm.dpVersionR % 256),  I_DevicePrm.dpBuildR, I_DevicePrm.dpVerRdLn);
				SerialPutString(general_str);
				*/

				if ((I_DevicePrm.dpVerRdLn > 0) && (I_DevicePrm.dpVerRdLn  < (INNFLS_MAX_APP_SIZE + 1)))
				{
					/*
					SerialPutString("Found New firmware Update\r\n");
					SerialPutString("Calculating extern CRC...");
					*/
					
					if (Flash_Calc_Version_CRC16(I_DevicePrm.dpVerRdLn,  EXTERNAL_FLASH_APP_START_ADDRESS , glbDatBuf ,&glbCrc16) == SUCCESS)
					{
						// SerialPutString(" - Success\r\n");

						/*
						// SerialPutString("Calc CRC16 of Ext Flash New Application Success\n\r");
						sprintf( general_str, "Expected CRC=%d, Calculated CRC=%d\r\n" , I_DevicePrm.dpVerRcrc, glbCrc16);
						SerialPutString(general_str);
						*/
						
						// if (1) // dubug only: force upgrade new firmware.
						if(I_DevicePrm.dpVerRcrc == glbCrc16)
						{
							/*
							SerialPutString("External Flash CRC match\r\n");

							// SerialPutString("CRC16 of Ext Flash Compare Success\n\r");
							SerialPutString("Erasing current firmware...");
							*/
							
							__HAL_FLASH_CLEAR_FLAG(0xFFFF);

							if (Inner_Flash_Erase() == SUCCESS)
							{
								// SerialPutString(" - Success\r\n");
								
								// SerialPutString("Copying new firmware...\r\n");

								if (Transfer_Version(I_DevicePrm.dpVerRdLn, EXTERNAL_FLASH_APP_START_ADDRESS, glbDatBuf, INNFLS_STR_APP_ADD) == SUCCESS)
								{
									// SerialPutString(" - Success\r\n");
									
									glbCrc16 = 0;

									// SerialPutString("Calculating inner flash firmware CRC...\r\n");
									
									if (Inner_Flash_ClcCrc16(I_DevicePrm.dpVerRdLn, INNFLS_STR_APP_ADD , &glbCrc16, glbDatBuf, DATA_BUFFER_SIZE) == SUCCESS)
									{
										/*
										sprintf( general_str, "Expected CRC=%d, Calculated CRC=%d\r\n" , I_DevicePrm.dpVerRcrc, glbCrc16);
										SerialPutString(general_str);
										*/
										
										// if (1) // debug only: force copy firmware
										if (I_DevicePrm.dpVerRcrc == glbCrc16)
										{
											/*
											SerialPutString("Inner Flash CRC match\r\n");
											SerialPutString("The transfer succeeded\r\n");
											*/
											
											I_DevicePrm.dpVerRcrc = 0;
											I_DevicePrm.dpVerRdLn = 0;
											I_DevicePrm.dpVersion = I_DevicePrm.dpVersionR;
											I_DevicePrm.dpBuild = I_DevicePrm.dpBuildR;

											// SerialPutString("Burning the new version number...\r\n");
											
											if(DevParms_Burn_Flash(&I_DevicePrm) == SUCCESS)
											{
												// SerialPutString(" - Success\r\n");

												success_code = 2;
												// SerialPutString("Jumping To Application\r\n");
												Going_To_APP();
											}
											else
											{	
												// SerialPutString(" - Failed\r\n");
												// SerialPutString("Program Parameters into Ext Flash Failed\n\r");
												
												while(ledFinished == 0)
												{
													inx++;
												}
												
												// HAL_NVIC_SystemReset();
											}
										}
										else
										{
											// SerialPutString("Inner Flash CRC doesn't match\r\n");
											
											while(ledFinished == 0)
											{
												inx++;
											}
											
											// HAL_NVIC_SystemReset();
										}
									}
									else
									{
										// SerialPutString("Calculating inner CRC Failed\r\n");
										
										while(ledFinished == 0)
										{
											inx++;
										}

										// HAL_NVIC_SystemReset();
									}
								}
								else
								{
									// SerialPutString(" - Failed\r\n");
									//SerialPutString("Transfer Application from Ext Flash to Inner Flash Success\n\r");
									
									while (ledFinished == 0)
									{
										inx++;
									}
									
									// HAL_NVIC_SystemReset();
								}
							}
							else
							{
								// SerialPutString(" - Failed\r\n");
								// SerialPutString("Erase Inner Flash Sectors 1-5 Failed\n\r");
								
								while(ledFinished == 0)
								{
									inx++;
								}
								
								// HAL_NVIC_SystemReset();
							}
						}
						else
						{
							// SerialPutString("External Flash CRC doesn't match\r\n");
						}
					}
					else
					{
						// SerialPutString(" - Failed\r\n");	// reading the external flash failed
					}
				}
				else
				{
					/*
					SerialPutString(" - Failed\r\n");
					SerialPutString("New firmware is over-sized.\r\n");
					*/
				}
			}

			// SerialPutString("Jumping To Application\r\n");
			Going_To_APP();
		}
		
	}
	else
	{
		// SerialPutString(" - Failed\n\r");

		// SerialPutString("Setting manufacture defaults...\r\n");

#ifndef REMOVE_PARAMS
		DevParms_Set_Default(&I_DevicePrm);
		DevParms_Burn_Flash(&I_DevicePrm);
#endif
		
		while (ledFinished == 0)
		{
			inx++;
		}

		HAL_NVIC_SystemReset();
	}
	
	while(1)
	{
		inx++;	
	}
}

void init_charge_tables(void)
{
	charging_table[0] = CHARGE_CURRENT_MODE_0;
	charging_table[1] = CHARGE_CURRENT_MODE_1;
	charging_table[2] = CHARGE_CURRENT_MODE_2;
	charging_table[3] = CHARGE_CURRENT_MODE_3;
	charging_table[4] = CHARGE_CURRENT_MODE_4;
	charging_table[5] = CHARGE_CURRENT_MODE_5;
	charging_table[6] = CHARGE_CURRENT_MODE_6;
	charging_table[7] = CHARGE_CURRENT_MODE_7;
	charging_table[8] = CHARGE_CURRENT_MODE_8;
	charging_table[9] = CHARGE_CURRENT_MODE_9;
	charging_table[10] = CHARGE_CURRENT_MODE_10;
	charging_table[11] = CHARGE_CURRENT_MODE_11;
	charging_table[12] = CHARGE_CURRENT_MODE_12;
	charging_table[13] = CHARGE_CURRENT_MODE_13;
	charging_table[14] = CHARGE_CURRENT_MODE_14;
	charging_table[15] = CHARGE_CURRENT_MODE_15;
}


void ConvertMeasurementUnits(void)
{
	u16 *voltCalValPtr = NULL;
	float  fVoltCalVal = 0;
	u16 voltCalVal = 0;

	voltCalValPtr = (u16*)0x1FFF75AA;
	voltCalVal = (u16)(*voltCalValPtr);
	fVoltCalVal = (float)voltCalVal;

	measurements_res.adcVBatt = (float)(((((float)measurements.vBatt) * 3.0 * fVoltCalVal)/(((float)measurements.vRefernce) * 4095.0)) * 2.0);
	measurements_res.adcVBattPer = (float)((( measurements_res.adcVBatt) / 3.75)  * 100.0);

	if (measurements_res.adcVBattPer > 100.0)
	{
		measurements_res.adcVBattPer = 100.0;
	}

	fVoltCalVal = ((float)measurements.temperature)/4096;
	fVoltCalVal =  R16 * (1-fVoltCalVal) / fVoltCalVal;
	fVoltCalVal = log( fVoltCalVal / R0)/B_FACTOR + 1.0/T25;
	fVoltCalVal = (float)1.0f / fVoltCalVal - T0;
	measurements_res.adcTemp = (fVoltCalVal);


	// measurements_res.adcTemp = (float)((((float)measurements.temperature - 870.0)/150.0) + 25.0);
	measurements_res.adcSolarV = (float)(((((float)measurements.solar) * 3.0 * fVoltCalVal)/(((float)measurements.vRefernce) * 4095.0)) * 2.0);

	measurements_res.adcSolar = (float)((( (((float)measurements.solar) / 4095.0) * 1.0 ) /SOLAR_RESITOR_RATIO) * 100 );
	measurements_res.adcExtPw = 0.0;

	if (measurements_res.adcSolar > 100.0)
	{
		measurements_res.adcSolar = 100.0;
	}


	return measurements_res;
}

void SetResistor( GPIO_TypeDef* port , u16 pin, u8 is_set)
{
}

void SetChargingMode1(unsigned char value)
{
}



/*
u8 SetChargingMode(float solar_value)
{
	u8 charging_increased = 0;

	if (solar_value >= SOLAR_MORE_CHARGER_TH)
	{
		if (charger_resistors_value < (NUM_CHARGE_MODES-1) )
		{
			charging_increased = 1;
			charger_resistors_value++;

			SetChargingMode1(0);
		}

	}
	else if (solar_value >= SOLAR_SAME_CHARGER_TH)
	{
		// stay on the same charger
	}
	else if (solar_value >= SOLAR_REDUCE_CHARGER_TH)
	{

		if (charger_resistors_value > 0)
		{
			charger_resistors_value--;
			SetChargingMode1(1);
		}
	}

	else
	{
		if (charger_resistors_value > 0)
		{
			charger_resistors_value--;

			if (charger_resistors_value > 0)
				charger_resistors_value--;

			SetChargingMode1(1);
		}
	}


	return charging_increased;
}
*/


u8 TestVoltage(unsigned char enable_print)
{
	static u8 successive_voltage_count = 0;
	u8 ret = 0;

	// disable charger in order to read voltage correctly
	HAL_GPIO_WritePin(IO_PORT_CHARGE_EN, IO_PIN_CHARGE_EN, GPIO_PIN_SET);

	GPIO_Temperature(1);
	TMR2_Sleep(3);

	ActivateScan();
	ConvertMeasurementUnits();

	GPIO_Temperature(0);

	if ( measurements_res.adcVBatt >= START_OPEREATION_VOLTAGE )
	{
		successive_voltage_count++;

		if (successive_voltage_count >= (3*3))
			ret = 1;
	}
	else
	{
		successive_voltage_count = 0;
	}

	// enable charger
	unsigned char enable_solar_charger = 0;

	if (measurements_res.adcTemp <= MAX_TEMPERATURE)
		enable_solar_charger = 1;


	if (enable_solar_charger)
		HAL_GPIO_WritePin(IO_PORT_CHARGE_EN, IO_PIN_CHARGE_EN, GPIO_PIN_RESET);


/*
#ifdef UART_DEBUG
	if (enable_print)
	{
		float x = measurements_res.adcVBatt*1000;

		measure_count++;
		sprintf( general_str, "\r\n%d) Temperature=%d, Vbat=%dmV, solar charger enable=%d\r\n" , measure_count, (int)measurements_res.adcTemp, (int)x, enable_solar_charger );
		SerialPutString(general_str);

		PrintVersion();
	}
#endif
*/


	return ret;
}


void CheckVoltages(void)
{
	//u8 stay_in_bootloader;
	//u8 resitor_changed;
	//u8 successive_voltage_count = 0;
	u8 first_loop = 1;
	u8 i;

	//current_charging_capacity = 0.0f;
	// init_charge_tables();

	/*
	if (hal_status != HAL_OK)
	{
		HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_RED, GPIO_PIN_RESET);

		TMR2_Sleep(3000);
	}
	*/

	while (1)
	{
		// usb connected
		if (HAL_GPIO_ReadPin(IO_PORT_USB_BUS,IO_PIN_USB_BUS))
		{
			AllLights();
			break;
		}

		// if (first_loop)
		if (1)
		{
			// SetChargingMode1(1);

			HAL_GPIO_WritePin(IO_PORT_CLK_OUT, IO_PIN_CLK_OUT, GPIO_PIN_SET);
			HAL_GPIO_WritePin(IO_PORT_LED_GREEN, IO_PIN_LED_RED, GPIO_PIN_RESET);

			for (i=0 ; i<3 ; i++)
			{
				TMR2_Sleep(15);
				TestVoltage(0);
			}

			HAL_GPIO_WritePin(IO_PORT_LED_GREEN, IO_PIN_LED_RED, GPIO_PIN_SET);
			HAL_GPIO_WritePin(IO_PORT_CLK_OUT, IO_PIN_CLK_OUT, GPIO_PIN_RESET);
			TMR2_Sleep(10);
		}

		WD_Refresh();

		// first scan: check if charging resistor changed

		/*
		ActivateScan();
		ConvertMeasurementUnits();


		WD_Refresh();


		// when no light but battery is good, exit bootloader any way
		//if (measurements_res.adcSolarV < 3.0)
		{
			if ( measurements_res.adcVBatt >= START_OPEREATION_VOLTAGE )
			{
				successive_voltage_count++;

				if (successive_voltage_count>=5)
					break;
			}
			else
			{
				successive_voltage_count = 0;
			}
		}
		*/

		if (TestVoltage(1))
			break;

		/*
		// update charging capacity
		if ( measurements_res.adcSolarV >= SOLAR_FULL_TH )
		{
			current_charging_capacity += charging_table[charger_resistors_value] * RTC_WAKEUP_INTERRUPT / 3600;

			if ( current_charging_capacity > BATTERY_FULL_CAPACITY )
				current_charging_capacity = BATTERY_FULL_CAPACITY;
		}
		*/

		// resitor_changed = SetChargingMode(measurements_res.adcSolarV);

		/*
		if (resitor_changed)
		{
			TMR2_Sleep(50);

			// second scan in order to verify that the solar voltage is still stable
			ActivateScan();
			ConvertMeasurementUnits();

			// if the voltage is not stable, change charging resistors back
			if ( measurements_res.adcSolarV < SOLAR_SAME_CHARGER_TH)
				SetChargingMode(measurements_res.adcSolarV);
		}
		*/


		/*
		stay_in_bootloader = 0;

		// stay in boot-loader if voltage is low
		if (measurements_res.adcVBatt < CUTOFF_VOLTAGE)
			stay_in_bootloader = 1;

		// stay in boot-loader if not enough charging time
		if ( current_charging_capacity < MIN_CHARGING_CAPACITY)
			stay_in_bootloader = 1;
		else
			resitor_changed = 0;
		*/


		//if (stay_in_bootloader)
		if (1)
		{
			/*
			TMR2_Sleep(1000);
			TMR2_Sleep(1000);
			TMR2_Sleep(1000);
			*/

			WD_Refresh();

			// blue led

			//HAL_GPIO_WritePin(IO_PORT_LED_BLUE, IO_PIN_LED_BLUE, GPIO_PIN_RESET);
			//HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_RED, GPIO_PIN_RESET);

			/*
			if (first_loop)
				TMR2_Sleep(1500);
			else
				TMR2_Sleep(20);
			*/

			//HAL_GPIO_WritePin(IO_PORT_LED_BLUE, IO_PIN_LED_BLUE, GPIO_PIN_SET);
			//HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_RED, GPIO_PIN_SET);

			WD_Refresh();

			// close ports

			DeInit_Peripherals();

			HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);

			// open ports

			Enable_GPIO_Clocks();
			TMR2_Init();
			hal_status = RTC_Init();

			ADC1_Init();
			Led_GPIO_Init();
			// TMR6_Init();

			#ifdef UART_DEBUG
			#ifndef REMOVE_UART
			USART1_Init();
			#endif
			#endif


			// this delay for testing current consumption during measurement stage duty-cycle
			// TMR2_Sleep(3000);
		}
		else
		{
			break;
		}

		WD_Refresh();

		first_loop = 0;
	}
}

void BlinkSequence(void)
{
	BlinkLed(0);
	TMR2_Sleep(100*10);
	BlinkLed(0);
	TMR2_Sleep(100*10);
	BlinkLed(1);
	TMR2_Sleep(100*10);
	BlinkLed(2);
}

void AllLights(void)
{
#ifdef CIRCUIT_V4
	HAL_GPIO_WritePin(IO_PORT_LED_BLUE, IO_PIN_LED_BLUE, GPIO_PIN_RESET);
	//TMR2_Sleep(500);
	HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_RED, GPIO_PIN_RESET);
	//TMR2_Sleep(500);
	HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_GREEN, GPIO_PIN_RESET);
	TMR2_Sleep(2500);

	HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_GREEN, GPIO_PIN_SET);
	//TMR2_Sleep(500);
	HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_RED, GPIO_PIN_SET);
	//TMR2_Sleep(500);
	HAL_GPIO_WritePin(IO_PORT_LED_BLUE, IO_PIN_LED_BLUE, GPIO_PIN_SET);
	//TMR2_Sleep(500);

#else
	HAL_GPIO_WritePin(IO_PORT_LED_BLUE, IO_PIN_LED_BLUE, GPIO_PIN_SET);
	//TMR2_Sleep(500);
	HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_RED, GPIO_PIN_SET);
	//TMR2_Sleep(500);
	HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_GREEN, GPIO_PIN_SET);
	TMR2_Sleep(2500);

	HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_GREEN, GPIO_PIN_RESET);
	//TMR2_Sleep(250);
	HAL_GPIO_WritePin(IO_PORT_LED_RED, IO_PIN_LED_RED, GPIO_PIN_RESET);
	//TMR2_Sleep(250);
	HAL_GPIO_WritePin(IO_PORT_LED_BLUE, IO_PIN_LED_BLUE, GPIO_PIN_RESET);
	//TMR2_Sleep(250);

#endif
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
  
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  //HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_MSI, RCC_MCODIV_1);//
    /**Configure the main internal regulator output voltage
    */
    
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);//Michael

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//Michael

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);//Michael
}



void WD_Init(void)
{
	I_WD.Init.Prescaler = IWDG_PRESCALER_256;
	I_WD.Init.Reload = 4095;
	I_WD.Init.Window = 4095;
	I_WD.Instance = IWDG;

	HAL_IWDG_Init(&I_WD);

	DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_IWDG_STOP;
}

void WD_Refresh(void)
{
	HAL_IWDG_Refresh(&I_WD);
}



/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
