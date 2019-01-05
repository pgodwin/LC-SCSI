/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/08/hal-library-23-touch-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   TS3510 low level library
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
#ifndef TM_TOUCH_TS3510_H
#define TM_TOUCH_TS3510_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_TOUCH_TS3510
 * @brief    TS3510 low level library - http://stm32f4-discovery.net/2015/08/hal-library-23-touch-for-stm32fxxx/
 * @{
 *
 * This is a low-level driver for TS3510 touch screen controller.
 *
 * To use this library, check @ref TM_TOUCH library on how to use it properly.
 *
 * \par Select custom I2C
 *
 * This touch controller is used on STM32F39-EVAL board and settings for I2C are for this board by default.
 * If you have own board and you need custom pins for I2C, open defines.h file and add/edit following lines.
 *
\code
//Select I2C for TS3510 conroller
#define TOUCH_TS3510_I2C      I2C1
#define TOUCH_TS3510_I2C_PP   TM_I2C_PinsPack_3
\endcode
 *
 * @note  Check @ref TM_I2C library for corresponding pins for selected I2C peripheral and pinspack
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
 - TM TOUCH
 - TM I2C
 - TM GPIO
\endverbatim
 */
#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_i2c.h"
#include "tm_stm32_touch.h"

/**
 * @defgroup TM_TOUCH_TS3510_Macros
 * @brief    Library defines
 * @{
 */

/* Touch I2C settings */
#ifndef TOUCH_TS3510_I2C
#define TOUCH_TS3510_I2C        I2C1
#define TOUCH_TS3510_I2C_PP     TM_I2C_PinsPack_3
#endif

/* I2C device address */
#define TOUCH_TS3510_I2C_DEV    0x80

/**
 * @}
 */
 
/**
 * @defgroup TM_TOUCH_TS3510_Typedefs
 * @brief    Library Typedefs
 * @{
 */
/* Typedefs here */
/**
 * @}
 */

/**
 * @defgroup TM_TOUCH_TS3510_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes TS3510 touch controller
 * @param  *TS: Pointer to @ref TM_TOUCH_t structure with settings
 * @retval Touch status:
 *            - 0: OK
 *            - > 0: Error
 */
uint8_t TM_TOUCH_TS3510_Init(TM_TOUCH_t* TS);

/**
 * @brief  Reads touch data from TS3510 touch controller
 * @param  *TS: Pointer to @ref TM_TOUCH_t to save data into
 * @retval Touch status:
 *            - 0: OK
 *            - > 0: Error
 */
uint8_t TM_TOUCH_TS3510_Read(TM_TOUCH_t* TS);

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
