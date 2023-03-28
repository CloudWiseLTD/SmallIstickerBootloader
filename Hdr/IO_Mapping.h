/*
 * IO_Mapping.h
 *
 *  Created on: 11 בספט׳ 2017
 *      Author: alon
 */

#ifndef IO_MAPPING_H_
#define IO_MAPPING_H_


//#define V3A_REV4 // deprecated
//#define V4_REV2
//#define V5_REV9
#define V6_REV0

#ifdef V2E_REV3
	#define HARDWARE_TYPE	1
#endif

#ifdef V3A_REV4
	#define HARDWARE_TYPE	2
#endif

#ifdef V4_REV2
	#define HARDWARE_TYPE	3
	#define CIRCUIT_V4
#endif

#ifdef V5_REV9
	#define CIRCUIT_V5
	#define HARDWARE_TYPE	5	// skip the number 4 intentionly
#endif

#ifdef V6_REV0
	#define CIRCUIT_V5
	#define HARDWARE_TYPE	6
#endif



#define UART_DEBUG


///////////////
// ADC ports //
///////////////

#define IO_PORT_SOLAR_VOLTAGE		GPIOA
#define IO_PIN_SOLAR_VOLTAGE		GPIO_PIN_0

#define IO_PORT_BATTERY_VOLTAGE		GPIOA
#define IO_PIN_BATTERY_VOLTAGE		GPIO_PIN_1


///////////
// UARTS //
///////////


#define IO_PORT_GPS_TX				GPIOA
#define IO_PIN_GPS_TX				GPIO_PIN_2

#define IO_PORT_GPS_RX				GPIOA
#define IO_PIN_GPS_RX				GPIO_PIN_3


#define IO_PORT_DBG_TX				GPIOA
#define IO_PIN_DBG_TX				GPIO_PIN_9

#define IO_PORT_DBG_RX				GPIOA
#define IO_PIN_DBG_RX				GPIO_PIN_10

#define IO_PORT_WIFI_TX				GPIOB
#define IO_PIN_WIFI_TX				GPIO_PIN_10

#define IO_PORT_WIFI_RX				GPIOB
#define IO_PIN_WIFI_RX				GPIO_PIN_11


#define IO_PORT_GPSSIM_TX			GPIOC
#define IO_PIN_GPSSIN_TX			GPIO_PIN_1

#define IO_PORT_GPSSIM_RX			GPIOC
#define IO_PIN_GPSSIM_RX			GPIO_PIN_0


//////////
// WIFI //
//////////

#define IO_PORT_WIFI_SPI_CLK		GPIOA
#define IO_PIN_WIFI_SPI_CLK			GPIO_PIN_5

#define IO_PORT_WIFI_SPI_MISO		GPIOA
#define IO_PIN_WIFI_SPI_MISO		GPIO_PIN_6

#define IO_PORT_WIFI_SPI_MOSI		GPIOA
#define IO_PIN_WIFI_SPI_MOSI		GPIO_PIN_7

#define IO_PORT_WIFI_SPI_CS			GPIOA
#define IO_PIN_WIFI_SPI_CS			GPIO_PIN_15

#define IO_PORT_WIFI_RESET			GPIOB
#define IO_PIN_WIFI_RESET			GPIO_PIN_1

#define IO_PORT_WIFI_POWER			GPIOC
#define IO_PIN_WIFI_POWER			GPIO_PIN_4

#define IO_PORT_LNA					GPIOB
#define IO_PIN_LNA					GPIO_PIN_6


////////////////
// Blue-Tooth //
////////////////

#define IO_PORT_BT_RESET			GPIOA
#define IO_PIN_BT_RESET				GPIO_PIN_8


///////////////////
// Accelerometer //
///////////////////

#define IO_PORT_INT2				GPIOC
#define IO_PIN_INT2					GPIO_PIN_5

#define IO_PORT_INT1				GPIOC
#define IO_PIN_INT1					GPIO_PIN_13


/////////
// USB //
/////////

#define IO_PORT_USB_DM				GPIOA
#define IO_PIN_USB_DM				GPIO_PIN_11

#define IO_PORT_USB_DP				GPIOA
#define IO_PIN_USB_DP				GPIO_PIN_12

#define IO_PORT_USB_BUS				GPIOB
#define IO_PIN_USB_BUS				GPIO_PIN_7


//////////
// Misc //
//////////

#define IO_PORT_I2C_SCL				GPIOB
#define IO_PIN_I2C_SCL				GPIO_PIN_8

#define IO_PORT_I2C_SDA				GPIOB
#define IO_PIN_I2C_SDA				GPIO_PIN_9

#define IO_PORT_GPS_POWER			GPIOE
#define IO_PIN_GPS_POWER			GPIO_PIN_8

#define IO_PORT_GPS_RESET			GPIOC
#define IO_PIN_GPS_RESET			GPIO_PIN_8

#define IO_PORT_GPS_STANDBY			GPIOC
#define IO_PIN_GPS_STANDBY			GPIO_PIN_9


#ifdef CIRCUIT_V5
	#define IO_PORT_DC_REG_EN			GPIOC
	#define IO_PIN_DC_REG_EN			GPIO_PIN_10
#else
	#define IO_PORT_DC_REG_EN			GPIOB
	#define IO_PIN_DC_REG_EN			GPIO_PIN_2
#endif



/*
#define IO_PORT_BUZZER				GPIOE
#define IO_PIN_BUZZER				GPIO_PIN_11
*/


#define IO_PORT_EPIC_EN				GPIOD
#define IO_PIN_EPIC_EN				GPIO_PIN_14

#define IO_PORT_DISABLE_CHARGER		GPIOD
#define IO_PIN_DISABLE_CHARGER		GPIO_PIN_15


///////////////
// FLASH SPI //
///////////////

#define IO_PORT_FLASH_SPI_CS		GPIOB
#define IO_PIN_FLASH_SPI_CS			GPIO_PIN_12

#define IO_PORT_FLASH_SPI_CLK		GPIOB
#define IO_PIN_FLASH_SPI_CLK		GPIO_PIN_13

#define IO_PORT_FLASH_SPI_MISO		GPIOB
#define IO_PIN_FLASH_SPI_MISO		GPIO_PIN_14

#define IO_PORT_FLASH_SPI_MOSI		GPIOB
#define IO_PIN_FLASH_SPI_MOSI		GPIO_PIN_15

#define IO_PORT_FLASH_RESET			GPIOD
#define IO_PIN_FLASH_RESET			GPIO_PIN_9

#define IO_PORT_FLASH_WP			GPIOD
#define IO_PIN_FLASH_WP				GPIO_PIN_8



#define FLASH_PORT					GPIOB


//////////
// LEDS //
//////////

#define IO_PORT_LED_RED				GPIOD
#define IO_PIN_LED_RED				GPIO_PIN_3

#define IO_PORT_LED_GREEN			GPIOD
#define IO_PIN_LED_GREEN			GPIO_PIN_2

#define IO_PORT_LED_BLUE			GPIOD
#define IO_PIN_LED_BLUE				GPIO_PIN_4

#define IO_PORT_CLK_OUT				GPIOE
#define IO_PIN_CLK_OUT				GPIO_PIN_3


#define IO_PORT_CHARGE_EN			GPIOE
#define IO_PIN_CHARGE_EN			GPIO_PIN_12


#define IO_PORT_BATTERY_TEMP		GPIOA
#define IO_PIN_BATTERY_TEMP			GPIO_PIN_6

#define IO_PORT_BATTERY_TEMP_EN		GPIOD
#define IO_PIN_BATTERY_TEMP_EN		GPIO_PIN_12




#endif /* IO_MAPPING_H_ */



