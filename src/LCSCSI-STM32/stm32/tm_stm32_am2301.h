/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   Basic library for AM2301 (DHT21) temperature and humidity sensor
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_AM2301_H
#define TM_AM2301_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_AM2301
 * @brief    Basic library for AM2301 (DHT21) temperature and humidity sensor
 * @{
 *
 * This library does not use any timers and interrupts to increase speed, only basic pooling mode.
 * 
 * \par Default pinout
 *
\verbatim
AM2301  STM32Fxxx      Description

VCC     3V3-5V         Supply voltage from 3V3 to 5V
GND     GND            Ground
DATA    User selected  Data line
\endverbatim
 *
 * \par Data output
 *
 * Temperature and humidity are returned from sensor in x10 multiplier, so like 55.5% humidity,
 * sensor will return 555 and for 27.3 degrees, sensor will return 273.
 *
 * @note This values are also returned from my library, you have to manually convert them (divide by 10) if needed.
 *
 * \par Changelog
 *
\verbatim
 Version 1.0
  - First release
\endverbatim
 *
 * \par Dependencies
 *
\verbatim
 - STM32Fxxx HAL
 - defines.h
 - TM DELAY
 - TM GPIO
\endverbatim
 */
#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_gpio.h"

/**
 * @defgroup TM_AM2301_Macros
 * @brief    Library defines
 * @{
 */

/**
 * @brief  Default data pin, overwrite it in defines.h file 
 */
#ifndef AM2301_PIN
#define AM2301_PORT				GPIOD
#define AM2301_PIN				GPIO_PIN_1
#endif

/**
 * @}
 */
 
/**
 * @defgroup TM_AM2301_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * Enumerations
 *
 * There are several different possible results.
 * If TM_AM2301_OK is returned from read function then you have valid data.
 */
typedef enum {
	TM_AM2301_Result_Ok = 0x00,           /*!< Everything OK */
	TM_AM2301_Result_Error,               /*!< An error occurred */
	TM_AM2301_Result_CONNECTION_ERROR,    /*!< Device is not connected */
	TM_AM2301_Result_WAITHIGH_ERROR,      /*!< Wait high pulse timeout */
	TM_AM2301_Result_WAITLOW_ERROR,       /*!< Wait low pulse timeout */
	TM_AM2301_Result_WAITHIGH_LOOP_ERROR, /*!< Loop error for high pulse */
 	TM_AM2301_Result_WAITLOW_LOOP_ERROR,  /*!< Loop error for low pulse */
	TM_AM2301_Result_PARITY_ERROR         /*!< Data read fail */
} TM_AM2301_Result_t;

/**
 * @brief  AM2301 working structure		
 */
typedef struct {
	int16_t Temp;        /*!< Temperature in tenths of degrees.
	                          If real temperature is 27.3�C, this variable's value is 273 */
	uint16_t Hum;        /*!< Humidity in tenths of percent.
	                          If real humidity is 55.5%, this variable's value is 555 */
	GPIO_TypeDef* GPIOx; /*!< Pointer to GPIOx PORT where pin for sensor is located */
	uint16_t GPIO_Pin;   /*!< GPIO pin used for sensor */
} TM_AM2301_t;

/**
 * @}
 */

/**
 * @defgroup TM_AM2301_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes AM2301 sensor
 * @param  *AMStruct: Pointer to empty @ref TM_AM2301_Data_t data structure
 * @param  *GPIOx: Pointer to GPIOx port you will use for your sensor
 * @param  GPIO_Pin: GPIO pin used in your port for sensor pin
 * @retval TM_AM2301_OK
 */
TM_AM2301_Result_t TM_AM2301_Init(TM_AM2301_t* AMStruct, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  Reads data from sensor
 * @param  *AMStruct: Pointer to @ref TM_AM2301_Data_t data structure to store data into
 * @retval Data valid:
 *            - TM_AM2301_OK: Data valid
 *            - Else: Data not valid
 */
TM_AM2301_Result_t TM_AM2301_Read(TM_AM2301_t* AMStruct);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
