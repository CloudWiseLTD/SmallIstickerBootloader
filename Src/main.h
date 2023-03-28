/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 


#define ENABLE_WATCHDOG

#define BLUE_BLINK_RATE_MINS			5
//#define SOLAR_FULL_TH					4.5
//#define CUTOFF_VOLTAGE					2.9 /// 2.7
#define START_OPEREATION_VOLTAGE		3.30 // 3.05 // 3.2 // 3.0
#define MAX_TEMPERATURE					45


#define SOLAR_MORE_CHARGER_TH			5.00
#define SOLAR_SAME_CHARGER_TH			4.50
#define SOLAR_REDUCE_CHARGER_TH			3.00


#define RTC_WAKEUP_INTERRUPT			20 //10
#define TICKS_IN_SECONDS				1000 //2500

#define BATTERY_FULL_CAPACITY			430   // 200mAH
#define MIN_CHARGING_PERCENTAGE			3  	// 3%
#define MIN_CHARGING_CAPACITY			(1.0f * MIN_CHARGING_PERCENTAGE * BATTERY_FULL_CAPACITY / 100.0f)
#define NUM_CHARGE_MODES				16

#define CHARGE_CURRENT_MODE_0			3
#define CHARGE_CURRENT_MODE_1			18
#define CHARGE_CURRENT_MODE_2			40
#define CHARGE_CURRENT_MODE_3			45
#define CHARGE_CURRENT_MODE_4			50
#define CHARGE_CURRENT_MODE_5			55
#define CHARGE_CURRENT_MODE_6			60
#define CHARGE_CURRENT_MODE_7			65
#define CHARGE_CURRENT_MODE_8			70
#define CHARGE_CURRENT_MODE_9			75
#define CHARGE_CURRENT_MODE_10			80
#define CHARGE_CURRENT_MODE_11			85
#define CHARGE_CURRENT_MODE_12			90
#define CHARGE_CURRENT_MODE_13			95
#define CHARGE_CURRENT_MODE_14			97
#define CHARGE_CURRENT_MODE_15			100




void SystemClock_Config(void);


#define FLASH_SECTOR_SIZE							4096

//#define EXTERNAL_FLASH_APP_START_ADDRESS			4096
#define EXTERNAL_FLASH_APP_START_ADDRESS			(16*FLASH_SECTOR_SIZE)

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
