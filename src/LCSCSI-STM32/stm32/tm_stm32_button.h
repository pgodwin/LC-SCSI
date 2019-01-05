/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/07/hal-library-13-buttons-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   Buttons library for STM32Fxxx devices
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
#ifndef TM_BUTTON_H
#define TM_BUTTON_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_BUTTON
 * @brief    Button library for STM32Fxxx devices - http://stm32f4-discovery.net/2015/07/hal-library-13-buttons-for-stm32fxxx/
 * @{
 *
 * This library works with external buttons.
 * It can detect button on pressed, normal button press and long button press
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
 - TM GPIO
 - TM DELAY
 - stdlib.h
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_gpio.h"
#include "tm_stm32_delay.h"
#include "stdlib.h"

/**
 * @defgroup TM_BUTTON_Macros
 * @brief    Library defines
 * @{
 */

/* Number of maximal supported buttons */
#ifndef BUTTON_MAX_BUTTONS
#define BUTTON_MAX_BUTTONS        10
#endif

/* Time for debounce */
#ifndef BUTTON_DEBOUNCE_TIME
#define BUTTON_DEBOUNCE_TIME      5
#endif

/* Number of milliseconds for normal press detection */
#ifndef BUTTON_NORMAL_PRESS_TIME
#define BUTTON_NORMAL_PRESS_TIME  100
#endif

/* Number of milliseconds for long press detection */
#ifndef BUTTON_LONG_PRESS_TIME
#define BUTTON_LONG_PRESS_TIME    1500
#endif

/* Library allocation function */
#ifndef LIB_ALLOC_FUNC
#define LIB_ALLOC_FUNC            malloc
#endif

/* Library free function */
#ifndef LIB_FREE_FUNC
#define LIB_FREE_FUNC             free
#endif

/**
 * @}
 */
 
/**
 * @defgroup TM_BUTTON_Typedefs
 * @brief    Library Typedefs
 * @{
 */
 
/**
 * @brief  Button possible press types
 */
typedef enum {
  TM_BUTTON_PressType_OnPressed = 0x00, /*!< Button pressed */
  TM_BUTTON_PressType_Debounce,         /*!< Button debounce */
	TM_BUTTON_PressType_Normal,           /*!< Normal press type, released */
	TM_BUTTON_PressType_Long              /*!< Long press type */
} TM_BUTTON_PressType_t;

/** 
 * @brief  Button structure 
 */
typedef struct _TM_BUTTON_t {
	GPIO_TypeDef* GPIOx;                                                /*!< GPIOx PORT for button */
	uint16_t GPIO_Pin;                                                  /*!< GPIO pin for button */
	uint8_t GPIO_State;                                                 /*!< GPIO state for pin when pressed */
	void (*ButtonHandler)(struct _TM_BUTTON_t*, TM_BUTTON_PressType_t); /*!< Button function handler */
	uint32_t StartTime;                                                 /*!< Time when button was pressed */
	uint8_t LastStatus;                                                 /*!< Button status on last check */
	uint8_t State;                                                      /*!< Current button state */
	uint16_t PressDebounceTime;                                         /*!< Time in ms for normal press for button */
	uint16_t PressNormalTime;                                           /*!< Time in ms for normal press for button */
	uint16_t PressLongTime;                                             /*!< Time in ms for long press for button */
} TM_BUTTON_t;

/**
 * @}
 */

/**
 * @defgroup TM_BUTTON_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes a new button to library
 * @note   This library uses @ref malloc() to allocate memory, so make sure you have enough heap memory
 * @param  *GPIOx: Pointer to GPIOx where button is located
 * @param  GPIO_Pin: GPIO pin where button is located
 * @param  ButtonState: Button state when it is pressed.
 *            - 0: Button is low when pressed
 *            - > 0: Button is high when pressed
 * @param  *ButtonHandler:
 * @retval Button creation status:
 *            - 0: Button was not created 
 *            - > 0: Button created and saved to library, button pointer is returned
 */
TM_BUTTON_t* TM_BUTTON_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t ButtonState, void (*ButtonHandler)(TM_BUTTON_t*, TM_BUTTON_PressType_t));

/**
 * @brief  Sets press timing values
 * @param  *ButtonStruct: Pointer to @ref TM_BUTTON_t structure for button used
 * @param  Normal: Time that button must be pressed to indicate normal press. Value is in milliseconds
 * @param  Normal: Time that button must be pressed to indicate long press. Value is in milliseconds
 * @retval Pointer to @ref TM_BUTTON_t
 */
TM_BUTTON_t* TM_BUTTON_SetPressTime(TM_BUTTON_t* ButtonStruct, uint16_t Normal, uint16_t Long);

/**
 * @brief  Updates buttons. This function have to be called periodically
 * @note   Function will automatically call callback functions for buttons if needed
 * @param  None
 * @retval None
 */
void TM_BUTTON_Update(void);

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
